#ifndef GAMEROOMHANDLE_H_INCLUDED
#define GAMEROOMHANDLE_H_INCLUDED





class GameRoomHandle{
    public:
    GameRoom* gameRoom;
    vector<Client*>& allClients;
    vector<GameRoom*>& allGameRooms;
    ServerEventHandler eventHandler;

    // Right, Up, Left, Down Rooms
    int nextRooms[4];

    GameRoomHandle(GameRoom* _gameRoom, vector<GameRoom*>& _allGameRooms, vector<Client*>& _allClients) : allClients(_allClients) , allGameRooms(_allGameRooms){
        gameRoom = _gameRoom;

        eventHandler.init(gameRoom);

        cout << "RoomHandler GameRoomId: " << gameRoom->id << endl;
        cout << "RoomHandler EventHandler GameRoomId: " << eventHandler.gameRoom->id << endl;

        for(int i = 0; i < 4 ; i++){
            nextRooms[i] = gameRoom->id;
        }
    }





    void update(){


        if(gameRoom->roomInfo.shakeDuration > 0){
            gameRoom->roomInfo.shakeIntensity *= 0.98;

            gameRoom->roomInfo.shakeX = (rand()%((int)gameRoom->roomInfo.shakeIntensity)) - (gameRoom->roomInfo.shakeIntensity/2);
            gameRoom->roomInfo.shakeY = (rand()%((int)gameRoom->roomInfo.shakeIntensity)) - (gameRoom->roomInfo.shakeIntensity/2);

            gameRoom->roomInfo.shakeDuration--;
        } else {
            gameRoom->roomInfo.shakeX = 0;
            gameRoom->roomInfo.shakeY = 0;
        }

        gameRoom->roomInfo.hspd *= 0.98;
        gameRoom->roomInfo.vspd *= 0.98;

        gameRoom->roomInfo.movX += gameRoom->roomInfo.hspd;
        gameRoom->roomInfo.movY += gameRoom->roomInfo.vspd;

        gameRoom->roomInfo.x = gameRoom->roomInfo.movX + gameRoom->roomInfo.shakeX;
        gameRoom->roomInfo.y = gameRoom->roomInfo.movY + gameRoom->roomInfo.shakeY;






        int objectHandleNumber = (int)gameRoom->gameObjectHandles.size();
        for(int i = 0; i < objectHandleNumber; i++){

            GameObjectHandle* objHandle = gameRoom->gameObjectHandles[i];
            GameObject* obj = objHandle->obj;

            if(obj->active){

                /// Updating Objects
                objHandle->update(gameRoom->roomInfo);



                /// Room Detection of (Out of Bounds objects)
                if(obj->active){

                    if(obj->vehicleId == -1){
                        if(obj->x - obj->colBox.x + obj->colBox.width < 0){
                            if(nextRooms[2] != gameRoom->id){
                                GameRoom* room = findRoomById(allGameRooms, nextRooms[2]);

                                cout << "Teleport Object " << obj->id << endl;

                                if(room != nullptr){
                                    obj->x = room->roomInfo.width - obj->colBox.x;


                                    changeInstanceRoom(room, obj);

                                    while(obj->passangerId != -1){

                                        obj = findGameObjectById(gameRoom->gameObjects, obj->passangerId);
                                        if(obj != nullptr){
                                            changeInstanceRoom(room, obj);
                                        } else {
                                            break;
                                        }

                                    }
                                }
                            } else {
                                obj->x = gameRoom->roomInfo.width - obj->colBox.x;;
                            }
                        } else if(obj->x + obj->colBox.x > gameRoom->roomInfo.width){
                            if(nextRooms[0] != gameRoom->id){
                                GameRoom* room = findRoomById(allGameRooms, nextRooms[0]);

                                if(room != nullptr){
                                    obj->x = -obj->colBox.width + obj->colBox.x;

                                    changeInstanceRoom(room, obj);

                                    while(obj->passangerId != -1){

                                        obj = findGameObjectById(gameRoom->gameObjects, obj->passangerId);
                                        if(obj != nullptr){
                                            changeInstanceRoom(room, obj);
                                        } else {
                                            break;
                                        }

                                    }
                                }
                            } else {
                                obj->x = -obj->colBox.width + obj->colBox.x;
                            }
                        } else if(obj->y < -100){

                        } else if(obj->y > 700){

                        }
                    }

                /// Sending Delete Events to client
                // for objects that are inactive
                } else if(!obj->deleteSended){
                    obj->deleteSended = true;

                    int clientNum = (int)allClients.size();
                    for(int j = 0; j < clientNum; j++){
                        if(allClients[j]->roomId == gameRoom->id){
                            sendInstanceDestroy(allClients[j]->packet, obj);
                        }
                    }
                }


            /// Sending Delete Events to client
            // for objects that are inactive
            } else if(!obj->deleteSended){
                obj->deleteSended = true;

                int clientNum = (int)allClients.size();
                for(int j = 0; j < clientNum; j++){
                    if(allClients[j]->roomId == gameRoom->id){
                        sendInstanceDestroy(allClients[j]->packet, obj);
                    }
                }
            }
        }




        /// Removing Objects (A little Convoluted / May need Rework!)
        /// Removing Inactive Object Handles
        objectHandleNumber = (int)gameRoom->gameObjectHandles.size();
        for(int i = objectHandleNumber-1; i > -1; i--){

            GameObjectHandle* objHandle = gameRoom->gameObjectHandles[i];
            GameObject* obj = objHandle->obj;

            if(!obj->active){
                gameRoom->gameObjectHandles.erase(gameRoom->gameObjectHandles.begin()+i);
                delete objHandle;
            }
        }

        /// Removing Inactive Objects
        int objectNumber = (int)gameRoom->gameObjects.size();
        for(int i = objectNumber-1; i > -1; i--){

            GameObject* obj = gameRoom->gameObjects[i];

            if(!obj->active){
                gameRoom->gameObjects.erase(gameRoom->gameObjects.begin()+i);
                delete obj;
            }
        }








        /// Client Interaction
        // Sending and Receiving packets

        int clientNum = (int)allClients.size();
        for(int i = 0; i < clientNum; i++){

            /// Only Manages the Clients that are in this room
            if(allClients[i]->roomId == gameRoom->id){

                /// Add the posterior room events to the Client packet
                if(gameRoom->roomPacketPosterior.getDataSize() > 0){
                    allClients[i]->packet.append(gameRoom->roomPacketPosterior.getData(), gameRoom->roomPacketPosterior.getDataSize());
                }

                /// Add the room events to the Client packet
                if(gameRoom->roomPacket.getDataSize() > 0){
                    allClients[i]->packet.append(gameRoom->roomPacket.getData(), gameRoom->roomPacket.getDataSize());
                }


                /// If the Client responded to the last packet sent
                if(allClients[i]->lastResponse){

                    /// Sending The Objects Updates
                    int gameObjectNumber = (int)gameRoom->gameObjects.size();
                    for(int j = 0; j < gameObjectNumber; j++){
                        sendInstanceUpdate(allClients[i]->packet, gameRoom->gameObjects[j]);
                    }
                    sendRoomInfo(allClients[i]->packet, &(gameRoom->roomInfo));

                    /// Add the instantaneous room events to the Client packet
                    /// Right Now they are only audio play requests
                    if(gameRoom->roomPacketInstant.getDataSize() > 0){
                        allClients[i]->packet.append(gameRoom->roomPacketInstant.getData(), gameRoom->roomPacketInstant.getDataSize());
                    }


                    if(gameRoom->soundRequests.getDataSize() > 0){
                        allClients[i]->packet.append(gameRoom->soundRequests.getData(), gameRoom->soundRequests.getDataSize());
                    }



                    /// Sending Client Packet
                    sf::Socket::Status sendStatus = allClients[i]->sendPacket();

                    // Disconnecting Client if connection lost
                    if(sendStatus == sf::Socket::Disconnected){
                        cout << "Client " << allClients[i]->id << " Disconnected" << endl;
                        disconnectClient(i);
                    }
                }
            }
        }


        /// Posterior Room Events Execution
        while(gameRoom->roomPacketPosterior && !gameRoom->roomPacketPosterior.endOfPacket()){
            eventHandler.executeEvent(gameRoom->roomPacketPosterior);
        }

        gameRoom->roomPacketPosterior.clear();


        /// Room Events Execution
        while(gameRoom->roomPacket && !gameRoom->roomPacket.endOfPacket()){
            eventHandler.executeEvent(gameRoom->roomPacket);
        }

        gameRoom->roomPacket.clear();
        gameRoom->roomPacketInstant.clear();
        gameRoom->soundRequests.clear();

    }





    /// Changes the Room of The Instance
    void changeInstanceRoom(GameRoom* sendRoom, GameObject* obj){

        /// Getting the object index
        int objIndex = findGameObjectIndexById(gameRoom->gameObjects, obj->id);

        // Do nothing if the object was not found
        if(objIndex != -1){

            /// If the object is followed by a client
            if(obj->clientFollow){

                /// Find the client
                Client* client = findClientById(allClients , obj->clientId);

                // Do nothing if the client was not found
                if(client != nullptr){

                    // Do nothing if the sendRoom is this Room
                    if(client->roomId != sendRoom->id){

                        /// Sends the Room Change Events
                        sendLoadRoom(client->packet, sendRoom);
                        client->roomId = sendRoom->id;
                    }
                }
            }

            /// Creates the instance in the sendRoom
            sendInstanceCopy(sendRoom->roomPacket, obj);

            /// Destroys the instance in this room
            sendInstanceDestroy(gameRoom->roomPacket, obj);
        }
    }





    /// Disconnects the Client
    void disconnectClient(int clientIndex){

        Client* client = allClients[clientIndex];

        client->socket->disconnect();



        int gameRoomNumber = (int)allGameRooms.size();
        for(int i = 0; i < gameRoomNumber; i++){
            int objIndex = findGameObjectIndexByClientId(allGameRooms[i]->gameObjects, client->id);

            if(objIndex != -1){

                GameObject* obj = allGameRooms[i]->gameObjects[objIndex];
                if(obj->type == PLAYER){
                    sendInstanceDestroy(allGameRooms[i]->roomPacket, obj);
                }
            }
        }


        delete client->socket;

        allClients.erase(allClients.begin() + clientIndex);
        delete client;
    }

};



#endif // GAMEROOMHANDLE_H_INCLUDED

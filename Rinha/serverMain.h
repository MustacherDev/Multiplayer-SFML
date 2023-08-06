#ifndef SERVERMAIN_H_INCLUDED
#define SERVERMAIN_H_INCLUDED


void serverMain(){
// SERVER PART OF THE PROGRAM

    // Initializing Server
    sf::IpAddress ip = sf::IpAddress::getLocalAddress();


    vector<Client*> allClients;
    int clientNumber = 0;
    int uniqueClientNumber = 0;

    vector<GameRoom*> allGameRooms;

    /// GameRoom 1
    GameRoom* gameRoom1 = new GameRoom(0);
    gameRoom1->roomInfo.backgroundIndex = 0;

    // Object 1
    GameObject gameObject1(BOAT, 100, 100, getNewUID());
    gameObject1.roomId = 0;
    gameObject1.physics.vRest = 0;
    gameObject1.physics.hDamp = 0.98;
    gameObject1.hspd = -2;

    sendInstanceCopy(gameRoom1->roomPacket, &gameObject1);


    // Object2
    GameObject gameObject2(PLAYER, 400, 100, getNewUID());
    gameObject2.roomId = 0;
    gameObject2.sprite.index = SPRBRICK;

    sendInstanceCopy(gameRoom1->roomPacket, &gameObject2);

    // ObjectX
    GameObject gameObjectX(WALL, -64, gameRoom1->roomInfo.height-64, getNewUID());
    gameObjectX.roomId = 0;
    gameObjectX.sprite.index = SPRGRASS;
    gameObjectX.colBox.width = gameRoom1->roomInfo.width + 128;
    gameObjectX.sprite.xScl = ((float)gameRoom1->roomInfo.width + 128)/32;
    gameObjectX.repeatMap = true;

    sendInstanceCopy(gameRoom1->roomPacket, &gameObjectX);







    /// GameRoom 2
    GameRoom* gameRoom2 = new GameRoom(1);

    gameRoom2->roomInfo.backgroundIndex = 1;
    gameRoom2->roomInfo.vGravity = 0.05;
    gameRoom2->roomInfo.width = 3000;
    gameRoom2->roomInfo.height = 1000;

    GameObject gameObject3(BOAT, -140, 100, getNewUID());
    gameObject3.roomId = 1;
    gameObject3.hspd = -1;
    gameObject3.physics.hDamp = 1;

    sendInstanceCopy(gameRoom2->roomPacket, &gameObject3);

    GameObject gameObject4(BOMB, 100, 10, getNewUID());
    gameObject4.roomId = 1;
    gameObject4.hspd = 3;
    gameObject4.physics.hDamp = 1;
    gameObject4.physics.vRest = 1;
    gameObject4.physics.vDamp = 1;

    sendInstanceCopy(gameRoom2->roomPacket, &gameObject4);

    // ObjectX2
    GameObject gameObjectX2(WALL, -64, gameRoom2->roomInfo.height-64, getNewUID());
    gameObjectX2.roomId = 1;
    gameObjectX2.sprite.index = SPRSAND;
    gameObjectX2.colBox.width = gameRoom2->roomInfo.width + 128;
    gameObjectX2.sprite.xScl = ((float)gameRoom2->roomInfo.width + 128)/32;
    gameObjectX2.repeatMap = true;

    sendInstanceCopy(gameRoom2->roomPacket, &gameObjectX2);

    {
        GameObject obj(WALL, 0, gameRoom1->roomInfo.height - 64, getNewUID());
        obj.roomId = 1;
        obj.sprite.index = SPRSAND;
        obj.colBox.height = (float)gameRoom2->roomInfo.height - (float)gameRoom1->roomInfo.height;
        obj.sprite.xScl = 1;
        obj.sprite.yScl = ((float)gameRoom2->roomInfo.height-(float)gameRoom1->roomInfo.height) / 32;
        obj.repeatMap = true;

        sendInstanceCopy(gameRoom2->roomPacket, &obj);
    }




    // Inserting Rooms on the vector
    allGameRooms.push_back(gameRoom1);
    allGameRooms.push_back(gameRoom2);
    int gameRoomNumber = 2;



    vector<GameRoomHandle*> allGameRoomHandles;

    /// Room Handles
    GameRoomHandle gameRoom1Handle(gameRoom1, allGameRooms, allClients);
    gameRoom1Handle.nextRooms[0] = gameRoom2->id;
    allGameRoomHandles.push_back(&gameRoom1Handle);

    GameRoomHandle gameRoom2Handle(gameRoom2, allGameRooms, allClients);
    gameRoom2Handle.nextRooms[2] = gameRoom1->id;
    allGameRoomHandles.push_back(&gameRoom2Handle);



    //gameObject1->printSelf();
    //system("pause");



    /// Listener Configuration
    sf::TcpListener listener;
    listener.setBlocking(false);


    if(listener.listen(2000) != sf::Socket::Done){
        println("Unable to initialize server on port 2000");
        system("pause");
        return;
    } else {
        println("Server on Local Address : " << ip.toString() << ":" << listener.getLocalPort());
    }




    // Server Loop
    bool loop = true;


    /// Server Execution Timing
    sf::Clock clock;
    sf::Time elapsedTime = sf::milliseconds(0);

    int frameInterval = 10;


    /// Main Server Loop
    while(loop){

        clock.restart();

        // Capping the loop cycles to be limited by time flow
        // That is, Only run after some time step
        if(elapsedTime.asMilliseconds() > frameInterval){


            /// Detecting and Managing New Client Connections

            // Allocate memory for the socket
            sf::TcpSocket* possibleNewSocket = new sf::TcpSocket();

            if(listener.accept(*possibleNewSocket) == sf::Socket::Done){

                possibleNewSocket->setBlocking(false);

                int uId = uniqueClientNumber;


                Client* client = new Client(uId, possibleNewSocket);
                allClients.push_back(client);

                clientNumber++;
                uniqueClientNumber++;


                // Text
                println("Connection");
                println("Client " << uId << " Connected");



                client->roomId = 0;


                /// Sending Initial Data to Client

                client->packet << client->id;
                client->sendPacket();

            } else {
                /// Error, no new connection, delete the socket
                delete possibleNewSocket;
            }














            /// Receiving Packets from Clients

            for(int i = 0; i < clientNumber; i++){

                sf::Packet receivePacket;
                sf::Socket::Status receiveStatus = allClients[i]->socket->receive(receivePacket);



                if(receiveStatus == sf::Socket::Done){


                    while(receivePacket && !receivePacket.endOfPacket()){
                        int eventType;
                        receivePacket >> eventType;


                        if(eventType == CLIENTEVENTJUMP){
                            GameRoom* room = findRoomById(allGameRooms, allClients[i]->roomId);
                            GameObject* obj = findGameObjectByClientId(room->gameObjects, allClients[i]->id);
                            //cout << "Number " << number << " received from client " << allClients[i]->id << endl;

                            if(obj != nullptr){
                                cout << "Jumping -> ";
                                obj->printSelf();


                                if(obj->vehicleId != -1){
                                    GameObject* other = findGameObjectById(room->gameObjects, obj->vehicleId);
                                    if(other != nullptr){
                                        other->passangerId = -1;
                                        other->physics.doHDamp = true;
                                    }
                                    obj->vehicleId = -1;
                                    obj->y -= obj->colBox.height;
                                    obj->vspd = 0;
                                    obj->vspd -= obj->playerObj.jumpSpd;
                                } else {
                                    if(obj->playerObj.jumps > 0){
                                        obj->playerObj.jumps--;
                                        obj->vspd -= obj->playerObj.jumpSpd;
                                    }
                                }

                            }
                        } else if(eventType == CLIENTEVENTRIGHTPRESS){
                            GameRoom* room = findRoomById(allGameRooms, allClients[i]->roomId);
                            GameObject* obj = findGameObjectByClientId(room->gameObjects, allClients[i]->id);

                            if(obj != nullptr){

                                //obj->hspd = 3;
                                obj->horizontalMov(obj->playerObj.hspdBase);
                                obj->physics.doHDamp = false;

                                if(obj->vehicleId != -1){
                                    GameObject* other = findGameObjectById(room->gameObjects, obj->vehicleId);
                                    if(other != nullptr){
                                        other->physics.doHDamp = true;
                                        other->horizontalMov(3);
                                    }
                                }
                            }
                        } else if(eventType == CLIENTEVENTLEFTPRESS){
                            GameRoom* room = findRoomById(allGameRooms, allClients[i]->roomId);
                            GameObject* obj = findGameObjectByClientId(room->gameObjects, allClients[i]->id);

                            if(obj != nullptr){
                                //obj->hspd = -3;
                                obj->horizontalMov(-obj->playerObj.hspdBase);
                                obj->physics.doHDamp = false;

                                if(obj->vehicleId != -1){
                                    GameObject* other = findGameObjectById(room->gameObjects, obj->vehicleId);
                                    if(other != nullptr){
                                        other->physics.doHDamp = false;
                                        other->horizontalMov(-3);
                                    }
                                }
                            }
                        } else if(eventType == CLIENTEVENTSTOPMOVEMENT){
                            GameRoom* room = findRoomById(allGameRooms, allClients[i]->roomId);
                            GameObject* obj = findGameObjectByClientId(room->gameObjects, allClients[i]->id);

                            if(obj != nullptr){
                                //obj->hspd = 0;
                                obj->physics.doHDamp = true;
                                obj->horizontalMov(0);
                                if(obj->vehicleId != -1){
                                    GameObject* other = findGameObjectById(room->gameObjects, obj->vehicleId);
                                    if(other != nullptr){
                                        other->physics.doHDamp = true;
                                    }
                                }
                            }
                        } else if (eventType == CLIENTEVENTENTER){

                            /// Creating New Player if new client
                            Client* thisClient = allClients[i];

                            int uId = thisClient->id;

                            // Creating New Player Object

                            int playerSprite;
                            string playerName;

                            receivePacket >> playerSprite;
                            receivePacket >> playerName;

                            GameObject newPlayer(PLAYER, 150, 0, getNewUID());
                            newPlayer.playerConstructor(uId, sf::Color((uId*218)%256, (uId*189)%256, (uId*320)%256));
                            newPlayer.clientId = uId;
                            newPlayer.sprite.index = playerSprite;
                            newPlayer.nameTag = playerName;

                            // Sending Object Copy
                            sendInstanceCopy(allGameRooms[0]->roomPacket, &newPlayer);

                            //cout << "New Player attempt" << endl;


                            sendLoadRoom(thisClient->packet, allGameRooms[0]);

                            sf::Socket::Status sendStatus = thisClient->sendPacket();

                        } else if(eventType == CLIENTEVENTCREATEOBJECTREQUEST){

                            int _clientId;
                            GameObject obj = GameObject(0, 0, 0, -1);

                            receivePacket >> _clientId;
                            receivePacket >> obj;

                            GameRoom* room = findRoomById(allGameRooms, allClients[i]->roomId);

                            sendCreateObjectRequest(room->roomPacket, &obj, _clientId);

                        } else if(eventType == CLIENTEVENTACTSTART){
                            GameRoom* room = findRoomById(allGameRooms, allClients[i]->roomId);
                            GameObject* obj = findGameObjectByClientId(room->gameObjects, allClients[i]->id);


                            if(obj != nullptr){
                                GameObjectHandle* handle = findGameObjectHandleById(room->gameObjectHandles, obj->id);
                                if(obj->active && obj->type == PLAYER){
                                    if(obj->playerObj.holdingId == -1){
                                        float radius = 100;
                                        float xx = obj->x - radius;
                                        float yy = obj->y - radius;
                                        int objNum = room->gameObjects.size();
                                        for(int j = 0; j < objNum; j++){
                                            GameObject* other = room->gameObjects[i];
                                            if(other->active && other->id != obj->id){
                                                if(other->type != WALL){
                                                    if(other->holderId == -1 && other->vehicleId == -1){
                                                        if(handle->customCollisionDetection(xx, yy, radius*2, radius*2, other)){
                                                            other->holderId = obj->id;
                                                            obj->playerObj.holdingId = other->id;
                                                            break;
                                                        }
                                                    }
                                                }
                                            }

                                        }

                                    }
                                }
                            }

                        } else if(eventType == CLIENTEVENTACTSTOP){

                        }
                    }


                    // Client sent packet that means it's responsive and ready for another one
                    allClients[i]->lastResponse = true;

                } else if(receiveStatus == sf::Socket::Disconnected){

                    /// Disconnects client if connection is lost
                    println("Client " << allClients[i]->id << " Disconnected");

                    disconnectClient(allGameRooms, allClients, i);
                    clientNumber--;
                }
            }






            /// GameRoom Updating
            int gameRoomHandleNumber = (int)allGameRoomHandles.size();
            for(int i = 0; i < gameRoomHandleNumber; i++){

                if(allGameRoomHandles[i]->gameRoom->active){
                    allGameRoomHandles[i]->update();
                }
            }


            elapsedTime -= sf::milliseconds(frameInterval);


        }


        elapsedTime += clock.getElapsedTime();
    }
}






#endif // SERVERMAIN_H_INCLUDED

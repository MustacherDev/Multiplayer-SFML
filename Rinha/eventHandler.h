#ifndef EVENTHANDLER_H_INCLUDED
#define EVENTHANDLER_H_INCLUDED


class ServerEventHandler{

    public:
    GameRoom* gameRoom = nullptr;


    ServerEventHandler(){
    }

    void init( GameRoom* _gameRoom){
        gameRoom = _gameRoom;

        cout << "ServerHandler GameRoomId: " << gameRoom->id << " Address:" << &(*gameRoom) << endl;
    }







    /// Translating Packet Info to Events
    void executeEvent(sf::Packet& packet){
        int eventType;

        packet >> eventType;




        if       (eventType == EVENTUPDATE){


            updateObject(packet);

        } else if(eventType == EVENTCREATE){


            createObject(packet);

        } else if(eventType == EVENTDESTROY){


            destroyObject(packet);

        } else if(eventType == EVENTNULL){


            cout << "Event 3 Does not Exist!" << endl;
            system("pause");

        } else if(eventType == EVENTDESTROYALL){


            destroyAllObjects(packet);

        } else if(eventType == EVENTUPDATEROOMINFO){


            updateRoomInfo(packet);

        } else if(eventType == EVENTPLAYSOUND){


            playSound(packet);

        } else if(eventType == EVENTSHAKEROOM){

            shakeRoom(packet);
            cout << "Room Shaked" << endl;

        } else if(eventType == EVENTPARTICLECREATE){

            createParticle(packet);

        }
        else if (eventType == EVENTPARTICLEPATTERNCREATE) {
            createPatternParticle(packet);
        }
        else if(eventType == CLIENTEVENTCREATEOBJECTREQUEST){

            createObjectRequest(packet);

        } else {
            cout << "Event " << eventType << " Does not Exist!" << endl;
        }
    }








    /// Functions that execute the Events

    void updateObject(sf::Packet& packet){
        int uId;
        packet >> uId;

        GameObject* obj = findGameObjectById(gameRoom->gameObjects,uId);

        if(obj != nullptr){
            packet >> *obj;
        }

    }


    void createObjectBase(float x, float y, int id, int objType){
        GameObject* newObj;

        // Different Creation Methods for different Object Types
        if(objType == PLAYER){
            cout << "PLAYER GameRoom " << gameRoom->id << endl;

            /// Creating and Initializing Player Object
            newObj = new GameObject(PLAYER,x, y, id);
            newObj->playerConstructor(0, sf::Color::Black);
            newObj->roomId = gameRoom->id;
            gameRoom->gameObjects.push_back(newObj);

            cout << "New Object Player Created" << endl;


        } else if(objType == BOAT) {
            cout << "BOAT GameRoom " << gameRoom->id << endl;

            /// Creating and Initializing Boat Object
            newObj = new GameObject(BOAT,x, y,id);
            newObj->roomId = gameRoom->id;
            gameRoom->gameObjects.push_back(newObj);

            cout << "New Object Boat Created" << endl;

        } else if(objType == BOMB){
             cout << "BOMB GameRoom " << gameRoom->id << endl;

            /// Creating and Initializing Boat Object
            newObj = new GameObject(BOMB,x, y,id);
            newObj->roomId = gameRoom->id;
            gameRoom->gameObjects.push_back(newObj);

            cout << "New Object Bomb Created" << endl;
        } else if(objType < OBJTOTAL){

            string objName = "???";

            switch (objType) {
            case WALL:
                objName = "WALL";
                break;

            case WARPER:
                objName = "WARPER";
                break;
            }

            println(objName << " GameRoom " << gameRoom->id);

            /// Creating and Initializing Boat Object
            newObj = new GameObject(objType, x, y,id);
            newObj->roomId = gameRoom->id;
            gameRoom->gameObjects.push_back(newObj);

            cout << "New Object " << objName << " Created" << endl;
        } else { 

            /// When the type is not defined
            cout << "Server: Could not create Object of type " << objType << endl;
            return;
        }

        /// Creating Object Handle
        GameObjectHandle* newHandle = new GameObjectHandle(newObj, gameRoom->gameObjects, gameRoom->roomPacket, gameRoom->roomPacketInstant, gameRoom->soundRequests);
        gameRoom->gameObjectHandles.push_back(newHandle);
    }

    void createObject(sf::Packet& packet){

        int objType;
        packet >> objType;

        float x;
        float y;
        int id;
        int roomId;

        packet >> x >> y >> id >> roomId;

        cout << "Creating object " << id << "!  X:" << x << " Y:" << y << " roomId:" << roomId << " OBJTYPE:" << objType << endl;

        createObjectBase(x, y, id, objType);
    }



    void destroyObject(sf::Packet& packet){

        int objId;
        packet >> objId;

        GameObject* obj = findGameObjectById(gameRoom->gameObjects,objId);
        if(obj != nullptr){
            obj->active = false;
        }
    }


    void destroyAllObjects(sf::Packet& packet){
        int objectNumber = (int)gameRoom->gameObjects.size();
        for(int i = 0; i < objectNumber; i++){
            GameObject* obj = gameRoom->gameObjects[i];
            obj->active = false;
        }
    }

    void updateRoomInfo(sf::Packet& packet){
        struct RoomInfo roomInfo;
        packet >> roomInfo;
    }


    void playSound(sf::Packet& packet){
        int soundId;
        packet >> soundId;
        /// Server Side so don't play any sound
    }

    void shakeRoom(sf::Packet& packet){
        float shakeIntensity;
        int shakeDuration;
        packet >> shakeIntensity;
        packet >> shakeDuration;

        gameRoom->roomInfo.shakeIntensity += shakeIntensity;
        gameRoom->roomInfo.shakeDuration = (gameRoom->roomInfo.shakeDuration + shakeDuration)/2;
    }

    void createPatternParticle(sf::Packet& packet) {

        float xx, yy;
        int patternType, partNum;

        packet >> patternType;
        packet >> partNum;
        packet >> xx;
        packet >> yy;

        // Nothing happens because it's client side event only
    }

    void createParticle(sf::Packet& packet){
        ParticleObject part;

        packet >> part;

        // Nothing happens because it's client side event only
    }

    void createObjectRequest(sf::Packet& packet){


        int clientId;

        packet >> clientId;


        GameObject obj = GameObject(0, 0, 0, -1);

        packet >> obj;

        obj.id = getNewUID();

        sendInstanceCopy(gameRoom->roomPacketPosterior, &obj);

    }
};














class ClientEventHandler{

    public:
    vector<GameObject*>& allGameObjects;
    vector<ParticleObject*>& allParticles;
    ResourceHandler& resources;
    RoomInfo& roomInfo;

    ClientEventHandler(vector<GameObject*>& _allGameObjects, vector<ParticleObject*>& _allParticles, RoomInfo& _roomInfo, ResourceHandler& _resources) : allGameObjects(_allGameObjects), allParticles(_allParticles), roomInfo(_roomInfo), resources(_resources)
    {
        roomInfo.id = -1;
        roomInfo.x = 0;
        roomInfo.y = 0;
        roomInfo.hspd = 0;
        roomInfo.vspd = 0;
        roomInfo.hGravity = 0;
        roomInfo.vGravity = 0.1;
    }


    void executeEvent(sf::Packet& packet){
        int eventType;

        packet >> eventType;

        //cout << "Client Event " << eventType;

        if       (eventType == EVENTUPDATE){
            updateObject(packet);

        } else if(eventType == EVENTCREATE){
            createObject(packet);

        } else if(eventType == EVENTDESTROY){
            destroyObject(packet);

        } else if(eventType == EVENTNULL){
            cout << "Event 3 Does not Exist!" << endl;
            system("pause");

        } else if(eventType == EVENTDESTROYALL){
            destroyAllObjects(packet);

        } else if(eventType == EVENTUPDATEROOMINFO){
            updateRoomInfo(packet);

        } else if(eventType == EVENTPLAYSOUND){
            playSound(packet);

        } else if(eventType == EVENTSHAKEROOM){
            shakeRoom(packet);

        } else if(eventType == EVENTPARTICLECREATE){
            createParticle(packet);

        }
        else if (eventType == EVENTPARTICLEPATTERNCREATE) {
            createPatternParticle(packet);
        }
        else if (eventType == CLIENTEVENTCREATEOBJECTREQUEST) {
            createObjectRequest(packet);

        } else {
            cout << "Event " << eventType << " Does not Exist!" << endl;
        }
    }

    void updateObject(sf::Packet& packet){
        int uId;
        packet >> uId;

        GameObject* obj = findGameObjectById(allGameObjects,uId);

        if(obj != nullptr){
            packet >> *obj;
        }
    }

    void createObject(sf::Packet& packet){

        int objType;
        packet >> objType;

        float x;
        float y;
        int id;
        int roomId;

        packet >> x >> y >> id >> roomId;

        cout << "Creating object " << id << "!  X:" << x << " Y:" << y << " roomId:" << roomId << endl;

        if(objType == PLAYER){
            GameObject* newPlayer = new GameObject(PLAYER,x, y,id);
            newPlayer->playerConstructor(0, sf::Color::Black);
            newPlayer->roomId = roomId;
            allGameObjects.push_back(newPlayer);

            cout << "New Object Player Created" << endl;
        } else if(objType == BOAT) {
            GameObject* newBoat = new GameObject(BOAT,x, y,id);
            newBoat->roomId = roomId;
            allGameObjects.push_back(newBoat);

            cout << "New Object Boat Created" << endl;
        } else if(objType == BOMB){

            /// Creating and Initializing Boat Object
            GameObject* newBomb = new GameObject(BOMB,x, y,id);
            newBomb->roomId = roomId;
            allGameObjects.push_back(newBomb);

            cout << "New Object Bomb Created" << endl;
        } else if(objType < OBJTOTAL){

            /// Creating and Initializing Boat Object
            GameObject* newObj = new GameObject(objType, x, y,id);
            newObj->roomId = roomId;
            allGameObjects.push_back(newObj);

            cout << "New Object Unnamed Created" << endl;
        } else {
            cout << "Server: Could not create Object of type " << objType << endl;
        }

    }

    void destroyObject(sf::Packet& packet){

        int objId;
        packet >> objId;

        GameObject* obj = findGameObjectById(allGameObjects,objId);
        if(obj != nullptr){
            obj->active = false;
        }
    }


    void destroyAllObjects(sf::Packet& packet){
        int objectNumber = (int)allGameObjects.size();
        for(int i = 0; i < objectNumber; i++){
            GameObject* obj = allGameObjects[i];
            obj->active = false;
        }

        int particleNumber = (int)allParticles.size();
        for(int i = 0; i < particleNumber; i++){
            ParticleObject* obj = allParticles[i];
            obj->active = false;
        }
    }

    void updateRoomInfo(sf::Packet& packet){
        packet >> roomInfo;
    }

    void playSound(sf::Packet& packet){
        int soundId;
        packet >> soundId;

        if(soundId != -1){
            resources.soundPlayer.setBuffer(*(resources.sounds[soundId]));
            resources.soundPlayer.play();
        }

    }

    void shakeRoom(sf::Packet& packet){
        float shakeIntensity;
        int shakeDuration;
        packet >> shakeIntensity;
        packet >> shakeDuration;

    }

    void createParticle(sf::Packet& packet){
        ParticleObject* part = new ParticleObject();

        packet >> *(part);

        allParticles.push_back(part);

    }

    void createPatternParticle(sf::Packet& packet) {

        float x, y;
        int patternType, partNum;

        packet >> patternType;
        packet >> partNum;
        packet >> x;
        packet >> y;


        ParticlePattern pattern = ParticlePattern(patternType);



        for (int j = 0; j < partNum; j++) {
            float xx = x + randFloatRange(pattern.xOffMin, pattern.xOffMax);
            float yy = y + randFloatRange(pattern.yOffMin, pattern.yOffMax);

            float angle = deg2Rad(randFloatRange(pattern.angMin, pattern.angMax));
            float angleSpd = deg2Rad(randFloatRange(pattern.angSpdMin, pattern.angSpdMax));

            float hspd = randFloatRange(pattern.hspdMin, pattern.hspdMax);
            float vspd = randFloatRange(pattern.vspdMin, pattern.vspdMax);

            int life = randIntRange(pattern.lifeMin, pattern.lifeMax);

            int spriteIndex = randIntRange(pattern.sprIndexMin, pattern.sprIndexMax);

            ParticleObject* part = new ParticleObject(xx, yy, hspd, vspd, spriteIndex, life);
            part->sprite.xScl = randFloatRange(pattern.sprXSclMin, pattern.sprXSclMax);
            part->sprite.yScl = randFloatRange(pattern.sprYSclMin, pattern.sprYSclMax);

            allParticles.push_back(part);

        }
    }

    void createObjectRequest(sf::Packet& packet){

        int clientId;

        packet >> clientId;


        GameObject obj = GameObject(0, 0, 0, -1);

        packet >> obj;
    }

};


#endif // EVENTHANDLER_H_INCLUDED

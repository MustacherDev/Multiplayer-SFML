#ifndef GAMEHELPER_H_INCLUDED
#define GAMEHELPER_H_INCLUDED


using namespace std;


/// Enumerators
enum objectTypes{
    GAMEOBJECT,
    PLAYER,
    BOAT,
    BOMB,
    BOX,
    WALL,
    WARPER,
    OBJTOTAL
};

enum spriteIndexes{
    SPRNULL,
    SPRXAROP,
    SPRBOAT,
    SPRBOMB,
    SPRRAT,
    SPRSQUID,
    SPRDEATH,
    SPRBRICK,
    SPRSUN,
    SPRMOON,
    SPRSTEELBAG,
    SPRSACK,
    SPRGRASS,
    SPRARROW,
    SPRDUST1,
    SPRDUST2,
    SPRDUST3,
    SPRSAND,
    SPRTOTAL
};

enum particlePatterns {
    PARTPATTERNEXPLOSION,
    PARTPATTERNTOTAL
};


enum events{
    EVENTUPDATE,
    EVENTCREATE,
    EVENTDESTROY,
    EVENTNULL,
    EVENTDESTROYALL,
    EVENTUPDATEROOMINFO,
    EVENTPLAYSOUND,
    EVENTSHAKEROOM,
    EVENTPARTICLECREATE,
    EVENTPARTICLEPATTERNCREATE,
    EVENTCREATEOBJECTREQUEST,
    EVENTCHANGEINSTANCEROOM,
    CLIENTEVENTENTER,
    CLIENTEVENTLEFTPRESS,
    CLIENTEVENTRIGHTPRESS,
    CLIENTEVENTJUMP,
    CLIENTEVENTSTOPMOVEMENT,
    CLIENTEVENTCREATEOBJECTREQUEST,
    CLIENTEVENTACTSTART,
    CLIENTEVENTACTSTOP

};





/// Color -> Packet Operator Overloading
sf::Packet & operator << (sf::Packet& packet, sf::Color& color){
    return packet << color.r << color.g << color.b;
}

sf::Packet & operator >> (sf::Packet& packet, sf::Color& color){
    return packet >> color.r >> color.g >> color.b;
}








/// Unique ID Function
int getNewUID(){
    static int uniqueId = 0;
    return uniqueId++;
}








/// Client Class
class Client{

    public:
    sf::TcpSocket* socket;
    sf::Packet packet;
    int id;
    bool lastResponse;
    int roomId;

    Client(int idGiven, sf::TcpSocket* socketGiven){
        socket = socketGiven;
        id = idGiven;
        lastResponse = true;
        roomId = 0;
    }

    ~Client(){
        //delete socket;
    }


    sf::Socket::Status sendPacket(){

        sf::Socket::Status sendStatus = socket->send(packet);

        while(sendStatus == sf::Socket::Partial){
            cout << "Partial Packet Sent in Client " << id << endl;
            sendStatus = socket->send(packet);
        }


        lastResponse = false;
        packet.clear();

        return sendStatus;
    }
};








/// Resource Handler, stores vectors to resources
class ResourceHandler{
    public:
    vector<sf::Sprite*> sprites;
    vector<sf::Sprite*> backgrounds;
    vector<sf::SoundBuffer*> sounds;
    sf::Sound soundPlayer;
    vector<sf::Font*> fonts;
};








/// Room Information class
class RoomInfo{
    public:
    int id = -1;
    float hGravity;
    float vGravity;
    float x;
    float y;
    float movX = 0;
    float movY = 0;
    float hspd;
    float vspd;
    float shakeIntensity = 0;
    int shakeDuration = 0;
    float shakeX = 0;
    float shakeY = 0;
    int backgroundIndex;
    float width = initWindowWidth;
    float height = initWindowHeight;

};


/// Room Information -> Packet Operator Overloading
sf::Packet & operator << (sf::Packet& packet, RoomInfo& info){
    return packet << info.id << info.hGravity << info.vGravity << info.x << info.y << info.hspd << info.vspd << info.shakeIntensity << info.shakeDuration << info.shakeX << info.shakeY << info.movX << info.movY << info.backgroundIndex << info.width << info.height;
}

sf::Packet & operator >> (sf::Packet& packet, RoomInfo& info){
    return packet >> info.id >> info.hGravity >> info.vGravity >> info.x >> info.y >> info.hspd >> info.vspd >> info.shakeIntensity >> info.shakeDuration >> info.shakeX >> info.shakeY >> info.movX >> info.movY >> info.backgroundIndex >> info.width >> info.height;
}





/// View Object Client Only
class View{
    public:
    float x;
    float y;
    float hspd;
    float vspd;
    float hacc;
    float vacc;

    float targetX;
    float targetY;

    View(){
        x = 0;
        y = 0;
        hspd = 0;
        vspd = 0;
        hacc = 0;
        vacc = 0;

        targetX = 0;
        targetY = 0;
    }

    void updateTargetPoint(float tX, float tY){
        targetX = tX;
        targetY = tY;
    }

    void update(RoomInfo& info){

        hacc = ((targetX - (initWindowWidth /2)) - x)/10000;
        vacc = ((targetY - (initWindowHeight/2)) - y)/10000;

        hspd += hacc;
        vspd += vacc;

        hspd *= 0.98;
        vspd *= 0.98;

        x += hspd;
        y += vspd;

        if(x < 0){
            x = 0;
        }

        if(x > info.width - initWindowWidth){
            x = info.width - initWindowWidth;
        }

        if(y < 0){
            y = 0;
        }

        if(y > info.height - initWindowHeight){
            y = info.height - initWindowHeight;
        }
    }
};



/// Physics structure and Packet Operator Overloading
struct Physics{
    float hRest = 0.6;
    float vRest = 0.6;

    float hDamp = 0.98;
    float vDamp = 0.98;

    float hspdMax = 10;
    float vspdMax = 10;

    bool doHDamp = true;
    bool doVDamp = false;

    bool doGravity = true;

    bool bounceRot = false;
};

sf::Packet & operator << (sf::Packet& packet, struct Physics& phy){
    return packet << phy.hRest << phy.vRest << phy.hDamp << phy.vDamp << phy.doHDamp << phy.doVDamp << phy.bounceRot << phy.doGravity;
}

sf::Packet & operator >> (sf::Packet& packet, struct Physics& phy){
    return packet >> phy.hRest >> phy.vRest >> phy.hDamp >> phy.vDamp >> phy.doHDamp >> phy.doVDamp >> phy.bounceRot >> phy.doGravity;
}







class SpriteData{
    public:
    int index;
    float xOffset;
    float yOffset;
    float xScl;
    float yScl;
    float xRotCenter = 0;
    float yRotCenter = 0;

    SpriteData(){
        index = 0;
        xOffset = 0;
        yOffset = 0;
        xScl = 1;
        yScl = 1;
    }

    SpriteData(int ind){
        index = ind;
        xOffset = 0;
        yOffset = 0;
        xScl = 1;
        yScl = 1;
    }

    SpriteData(int ind, float xScale, float yScale){
        index = ind;
        xOffset = 0;
        yOffset = 0;
        xScl = xScale;
        yScl = yScale;
    }

    SpriteData(int ind, float xScale, float yScale, float xOff, float yOff){
        index = ind;
        xOffset = xOff;
        yOffset = yOff;
        xScl = xScale;
        yScl = yScale;
    }
};


sf::Packet & operator << (sf::Packet& packet, struct SpriteData& spr){
    return packet << spr.index << spr.xOffset << spr.yOffset << spr.xScl << spr.yScl << spr.xRotCenter << spr.yRotCenter;
}

sf::Packet & operator >> (sf::Packet& packet, struct SpriteData& spr){
    return packet >> spr.index >> spr.xOffset >> spr.yOffset >> spr.xScl >> spr.yScl >> spr.xRotCenter >> spr.yRotCenter;
}













class Box{
    public:
    float x = 0;
    float y = 0;
    float width = 10;
    float height = 10;

    Box(){

    }

    Box(float _x, float _y, float _width, float _height){
        x = _x;
        y = _y;
        width = _width;
        height = _height;
    }

    bool contain(float xx, float yy){
        if(xx > x && xx < x + width){
            if(yy > y && yy < y + height){
                return true;
            }
        }
        return false;
    }

    bool intersect(Box &otherBox){
        if(x < otherBox.x + otherBox.width && x + width > otherBox.x){
            if(y < otherBox.y + otherBox.height && y + height > otherBox.y){
                return true;
            }
        }
        return false;
    }

    Box add(float xx, float yy){
        return Box(x + xx, y + yy, width, height);
    }
};

sf::Packet & operator << (sf::Packet& packet, Box& box){
    return packet << box.x << box.y << box.width << box.height;
}

sf::Packet & operator >> (sf::Packet& packet, Box& box){
    return packet >> box.x >> box.y >> box.width >> box.height;
}







class PlayerObject{
    public:

    int jumps = 4;
    int jumpsMax = 4;
    int jumpSpd = 6;

    int hspdBase = 2;

    int holdingId = -1;

    PlayerObject(){
        jumps = 4;
        jumpsMax = 4;
        jumpSpd = 6;

        hspdBase = 3;
    }

};


sf::Packet & operator << (sf::Packet& packet, PlayerObject& obj){
    return packet << obj.jumps << obj.jumpsMax << obj.jumpSpd << obj.hspdBase << obj.holdingId;
}

sf::Packet & operator >> (sf::Packet& packet, PlayerObject& obj){
    return packet >> obj.jumps >> obj.jumpsMax >> obj.jumpSpd >> obj.hspdBase >> obj.holdingId;
}






class BombObject{
    public:

    bool primed = false;
    int explodeTime = 100;
    int timer = 0;


    BombObject(){

    }

};


sf::Packet & operator << (sf::Packet& packet, BombObject& obj){
    return packet << obj.primed << obj.explodeTime << obj.timer;
}

sf::Packet & operator >> (sf::Packet& packet, BombObject& obj){
    return packet >> obj.primed >> obj.explodeTime >> obj.timer;
}





class WarperObject{
    public:

    int warpRoomId = -1;
    int warperId = -1;
    int cooldown = 0;
    float destX = 0;
    float destY = 0;

    WarperObject(){

    }

};


sf::Packet & operator << (sf::Packet& packet, WarperObject& obj){
    return packet << obj.warpRoomId << obj.warperId << obj.cooldown << obj.destX << obj.destY;
}

sf::Packet & operator >> (sf::Packet& packet, WarperObject& obj){
    return packet >> obj.warpRoomId >> obj.warperId >> obj.cooldown >> obj.destX >> obj.destY;
}




class ParticlePattern {
public:
    float hspdMin = 0;
    float hspdMax = 0;
    float vspdMin = 0;
    float vspdMax = 0;

    int lifeMin = 0;
    int lifeMax = 0;

    int sprIndexMin = 0;
    int sprIndexMax = 0;

    float sprXSclMin = 0;
    float sprXSclMax = 0;
    float sprYSclMin = 0;
    float sprYSclMax = 0;

    float xOffMin = 0;
    float xOffMax = 0;
    float yOffMin = 0;
    float yOffMax = 0;

    float angSpdMin = 0;
    float angSpdMax = 0;
    float angMin = 0;
    float angMax = 0;

 

    ParticlePattern(int type) {
        if (type == PARTPATTERNEXPLOSION) {
            //float xx = obj->x + ((rand() % 80) - 40);
            setXOff(0, 40);
            //float yy = obj->y + ((rand() % 80) - 40);
            setYOff(0, 40);

            //float spd = ((float)((rand() % 30) + 20)) / 150;
            //float randAngleDegrees = rand() % 180;
            //float angle = PI * (randAngleDegrees) / 180;
            setAng(180, 180);

            //float hspd = cos(angle) * spd;
            setHspd(0.15, 0.1);
            //float vspd = sin(angle) * spd;
            setVspd(0.15, 0.1);

            //int life = (rand() % 150) + 400;
            setLife(475, 75);

            //int spriteIndex = SPRDUST1 + (rand() % 3);
            setSprIndex(SPRDUST1 +1, 1);
            setSprXScl(2);
            setSprYScl(2);
        }
    }

    void setHspd(float val, float amp = 0) {
        hspdMin = val - amp;
        hspdMax = val + amp;
    }

    void setVspd(float val, float amp = 0) {
        vspdMin = val - amp;
        vspdMax = val + amp;
    }

    void setAngSpd(float val, float amp = 0) {
        angSpdMin = val - amp;
        angSpdMax = val + amp;
    }

    void setAng(float val, float amp = 0) {
        angMin = val - amp;
        angMax = val + amp;
    }

    void setXOff(float val, float amp = 0) {
        xOffMin = val - amp;
        xOffMax = val + amp;
    }

    void setYOff(float val, float amp = 0) {
        yOffMin = val - amp;
        yOffMax = val + amp;
    }

    void setLife(int val, int amp = 0){
        lifeMin = val - amp;
        lifeMax = val + amp;
    }

    void setSprXScl(float val, float amp = 0) {
        sprXSclMin = val - amp;
        sprXSclMax = val + amp;
    }

    void setSprYScl(float val, float amp = 0) {
        sprYSclMin = val - amp;
        sprYSclMax = val + amp;
    }


    void setSprIndex(int val, int amp = 0) {
        sprIndexMin = val - amp;
        sprIndexMax = val + amp;
    }

};







class ParticleObject{
    public:
    float x;
    float y;
    float hspd;
    float vspd;
    float hacc;
    float vacc;
    Physics physics;

    SpriteData sprite;

    int depth = 0;
    bool active = true;

    bool shake = false;
    float shakeIntensity = 0;

    int life = 100;

    int id = -1;

    ParticleObject(){
        x = 0;
        y = 0;
        hspd = 0;
        vspd = 0;
        hacc = 0;
        vacc = 0;
        sprite.index = 0;

    }

    ParticleObject(float xx, float yy, float _hspd, float _vspd, int spriteIndex, int _life){
        x = xx;
        y = yy;
        hspd = _hspd;
        vspd = _vspd;
        hacc = 0;
        vacc = 0;
        sprite.index = spriteIndex;
        life = _life;
        id = -1;

        physics.hDamp = 0.997;
        physics.vDamp = 0.997;
        physics.doHDamp = true;
        physics.doVDamp = true;
    }

    void update(){

        hspd += hacc;
        vspd += vacc;

        if(physics.doHDamp){
            hspd *= physics.hDamp;
        }

        if(physics.doVDamp){
            vspd *= physics.vDamp;
        }

        x += hspd;
        y += vspd;

        life--;

        if(life < 0){
            active = false;
        }

    }

    void draw(sf::RenderWindow& windowDraw, ResourceHandler& resources, RoomInfo& roomInfo){
        View view = View();

        draw(windowDraw, resources, roomInfo, view);
    }


    void draw(sf::RenderWindow& windowDraw, ResourceHandler& resources, RoomInfo& roomInfo, View& view){

        float xx = x+roomInfo.x-view.x;
        float yy = y+roomInfo.y-view.y;

        drawSprite(windowDraw, resources, sprite.index, xx, yy, sprite.xScl, sprite.yScl, 0, 0);

    }



    void drawSprite(sf::RenderWindow& windowDraw, ResourceHandler& resources, int spriteIndex, float x, float y, float xScl, float yScl, int facing, float ang){


        sf::FloatRect _rect = resources.sprites[spriteIndex]->getLocalBounds();
        float sprWidth = _rect.width;
        float sprHeight = _rect.height;

        if(facing == 1){

            resources.sprites[spriteIndex]->setOrigin(sprWidth/2, sprHeight/2);
            resources.sprites[spriteIndex]->setRotation(ang);

            resources.sprites[spriteIndex]->setPosition(x + ((sprWidth/2) -sprite.xOffset)*xScl, y + ((sprHeight/2)-sprite.yOffset)*yScl);
            resources.sprites[spriteIndex]->setScale(-xScl, yScl);

            windowDraw.draw(*(resources.sprites[spriteIndex]));
        } else {

            resources.sprites[spriteIndex]->setOrigin(sprWidth/2, sprHeight/2);
            resources.sprites[spriteIndex]->setRotation(ang);


            resources.sprites[spriteIndex]->setScale(xScl, yScl);
            resources.sprites[spriteIndex]->setPosition(x + ((sprWidth/2) - sprite.xOffset )*xScl, y + ((sprHeight/2) - sprite.yOffset )*yScl);
            windowDraw.draw(*(resources.sprites[spriteIndex]));
        }



    }

};

sf::Packet & operator << (sf::Packet& packet, ParticleObject& obj){
    return packet << obj.x << obj.y << obj.hspd << obj.vspd << obj.hacc << obj.vacc << obj.depth << obj.id << obj.life << obj.active << obj.physics << obj.sprite << obj.shake << obj.shakeIntensity;
}

sf::Packet & operator >> (sf::Packet& packet, ParticleObject& obj){
    return packet >> obj.x >> obj.y >> obj.hspd >> obj.vspd >> obj.hacc >> obj.vacc >> obj.depth >> obj.id >> obj.life >> obj.active >> obj.physics >> obj.sprite >> obj.shake >> obj.shakeIntensity;
}








#include "gameObject.h"


/// Object Search Functions
// Very ugly

GameObject* findGameObjectById(vector<GameObject*>& _allGameObjects, int uId){
    int length = _allGameObjects.size();


    for(int i = 0; i < length; i++){
        if(_allGameObjects[i]->id == uId){
            return _allGameObjects[i];
        }
    }

    println("findGameObjectById: ID " << uId <<  " Not found");
    return nullptr;
}


int findGameObjectIndexById(vector<GameObject*>& _allGameObjects, int uId){
    int length = _allGameObjects.size();


    for(int i = 0; i < length; i++){
        if(_allGameObjects[i]->id == uId){
            return i;
        }
    }
    println("findGameObjectIndexById: ID " << uId << " Not found");
    //system("pause");
    return -1;
}



GameObject* findGameObjectByClientId(vector<GameObject*>& _allGameObjects, int clientId){
    int length = _allGameObjects.size();


    for(int i = 0; i < length; i++){
        if(_allGameObjects[i]->clientId == clientId){
            return _allGameObjects[i];
        }
    }

    println("findGameObjectByClientId: ClientID " << clientId << " Not found");

    return nullptr;

    for(int i = 0; i < length; i++){
        cout << "Object " << i << " ClientId: " << _allGameObjects[i]->clientId << endl;
     }
}




int findGameObjectIndexByClientId(vector<GameObject*>& _allGameObjects, int clientId){
    int length = _allGameObjects.size();

    for(int i = 0; i < length; i++){
        if(_allGameObjects[i]->clientId == clientId){
            return i;
        }
    }

    println("findGameObjectIndexByClientId: ClientID " << clientId << " Not found");
 
    return -1;
}


Client* findClientById(vector<Client*>& _allClients, int uId){
    int length = _allClients.size();

    for(int i = 0; i < length; i++){
        if(_allClients[i]->id == uId){
            return _allClients[i];
        }
    }

    println("findClientById: ID of Client: " << uId << " Not found");
    return nullptr;


    for(int i = 0; i < length; i++){
        cout << "Client " << i << " ClientId: " << _allClients[i]->id << endl;
     }
    //system("pause");

    
}















/// Event to Packet Information functions

void sendInstanceCopy( sf::Packet& packet, GameObject* object){

    /// Create Object
    packet << EVENTCREATE;
    packet << object->type;
    packet << object->x;
    packet << object->y;
    packet << object->id;
    packet << object->roomId;

    /// Updates Object
    packet << EVENTUPDATE;
    packet << object->id;
    packet << *object;
}

void sendInstanceUpdate(sf::Packet& packet, GameObject* object){

    packet << EVENTUPDATE;
    packet << object->id;
    packet << *object;
}

void sendInstanceDestroy(sf::Packet& packet, GameObject* object){

    packet << EVENTDESTROY;
    packet << object->id;
}

void sendInstanceDestroyAll(sf::Packet& packet){

    /// Destroys all objects of the room for Server Side
    /// Destroys all objects in the Client Side
    packet << EVENTDESTROYALL;
}

void sendRoomInfo(sf::Packet& packet, RoomInfo* roomInfo){

    /// Updates the Room Information object for the Client Side
    packet << EVENTUPDATEROOMINFO;
    packet << *roomInfo;
}

void sendPlaySound(sf::Packet& packet, int soundId){

    /// Updates the Room Information object for the Client Side
    packet << EVENTPLAYSOUND;
    packet << soundId;
}

void sendShakeRoom(sf::Packet& packet, float shakeIntensity, int shakeDuration){

    /// Changes randomly the X and Y of the Room to make a shake effect
    packet << EVENTSHAKEROOM;
    packet << shakeIntensity;
    packet << shakeDuration;
}

void sendParticleCreate(sf::Packet& packet, ParticleObject* obj){

    /// Send a Create Particle Event
    // Works Client Only
    packet << EVENTPARTICLECREATE;
    packet << *(obj);

}

void sendParticlePatternCreate(sf::Packet& packet, int patternType, int partNum, float x, float y) {

    packet << EVENTPARTICLEPATTERNCREATE;
    packet << patternType;
    packet << partNum;
    packet << x;
    packet << y;
}

void sendCreateObjectRequest(sf::Packet& packet, GameObject* obj, int clientId){

    /// Send a Create Particle Event
    // Works Server Only
    packet << CLIENTEVENTCREATEOBJECTREQUEST;
    packet << clientId;
    packet << *(obj);
}

















#include "gameObjectHandle.h"



GameObjectHandle* findGameObjectHandleById(vector<GameObjectHandle*>& _allGameObjectHandles, int id){
    int length = _allGameObjectHandles.size();

    for(int i = 0; i < length; i++){
        if(_allGameObjectHandles[i]->obj->id == id){
            return _allGameObjectHandles[i];
        }
    }

    cout << "ID of Handle:" << id << " Not found! Array Size " << length << endl;
    for(int i = 0; i < length; i++){
        cout << "Handle " << i << " Id: " << _allGameObjectHandles[i]->obj->id << endl;
     }
    //system("pause");

    return nullptr;
}




class GameRoom{
    public:
    vector<GameObject*> gameObjects;
    vector<GameObjectHandle*> gameObjectHandles;
    sf::Packet roomPacket;
    sf::Packet roomPacketInstant;
    sf::Packet soundRequests;

    // Packet used to store events called by other events
    sf::Packet roomPacketPosterior;

    RoomInfo roomInfo;

    int id;

    bool active = true;

    GameRoom(int _id){
        roomInfo.id = _id;
        roomInfo.x = 0;
        roomInfo.y = 0;
        roomInfo.hspd = 0;
        roomInfo.vspd = 0;
        roomInfo.hGravity = 0;
        roomInfo.vGravity = 0.2;
        roomInfo.backgroundIndex = -1;

        roomInfo.width = initWindowWidth;
        roomInfo.height = initWindowHeight;

        id = _id;
    }

};


GameRoom* findRoomById(vector<GameRoom*>& _allGameRooms, int uId){
    int length = _allGameRooms.size();

    for(int i = 0; i < length; i++){
        if(_allGameRooms[i]->id == uId){
            return _allGameRooms[i];
        }
    }

    cout << "Room:" << uId << " Not found! Array Size" << length << endl;
    //system("pause");

    return nullptr;
}























/// This function sends a copy of all Objects in a Room and the Room Info object to a Packet
void sendLoadRoom(sf::Packet& packet, GameRoom* gameRoom){


    /// Destroying all objects in the Client Side
    sendInstanceDestroyAll(packet);

    /// Updating RoomInfo
    sendRoomInfo(packet, &(gameRoom->roomInfo));

    /// Sending all Objects of the new Room
    vector<GameObject*>& roomGameObjects = gameRoom->gameObjects;
    int gameObjectNumber = (int)roomGameObjects.size();
    for(int i = 0; i < gameObjectNumber; i++){
        sendInstanceCopy(packet, roomGameObjects[i]);
    }

}







/// Disconnecting the Client and Cleaning up Client Objects
void disconnectClient(vector<GameRoom*>& allGameRooms ,vector<Client*>& allClients, int clientIndex){

    cout << "Disconnecting player" << endl;

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














/// Game Room Handle & Event Handler

// needs a serverEventHandler
#include "eventHandler.h"
// needed for accessing all Game Rooms
#include "gameRoomHandle.h"






#endif // GAMEHELPER_H_INCLUDED

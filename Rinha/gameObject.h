#ifndef GAMEOBJECT_H_INCLUDED
#define GAMEOBJECT_H_INCLUDED




/// Game Object Class
class GameObject{
    public:

    /// Basic All Object Variables
    int type = 0;
    int id;
    float x;
    float y;
    float hspd;
    float vspd;
    float hacc;
    float vacc;
    float ang;
    float angSpd;

    int roomId;
    bool clientFollow;
    bool active = true;
    bool deleteSended = false;
    int clientId;

    struct Physics physics;

    bool hasCollision;
    Box colBox;

    int facing;

    int holderId = -1;


    // Sprite class
    SpriteData sprite;
    bool repeatMap = false;


    /// Objects that hold variables for the correspondent objects
    PlayerObject playerObj;
    BombObject bombObj;

    WarperObject warpObj;


    sf::Color color;


    /// Vehicle Variables
    int passangerId;
    int vehicleId;
    bool exitVehicle;


    /// Name for objects
    string nameTag;



    GameObject(int _objType, float xx, float yy, int _id){

        /// So much variables
        x = xx;
        y = yy;

        hspd = 0;
        vspd = 0;

        hacc = 0;
        vacc = 0;

        id = _id;
        clientId = -1;
        type = _objType;
        color = sf::Color::Blue;
        roomId = 0;
        clientFollow = false;

        sprite = SpriteData(SPRNULL);
        sprite.xScl = 2;
        sprite.yScl = 2;

        playerObj = PlayerObject();

        bombObj = BombObject();

        warpObj = WarperObject();

        physics.hDamp = 0.98;
        physics.vDamp = 0.99;
        physics.doHDamp = true;
        physics.doVDamp = true;
        physics.hRest = 0.5;
        physics.vRest = 0.5;


        ang = 0;
        angSpd = 0;



        facing = 0;

        hasCollision = false;

        if(type == PLAYER ){
            colBox = Box(8, 8, 48, 48);
            hasCollision = true;
        } else if (type == WALL){
            colBox = Box(0, 0, 64, 64);
        } else {
            colBox = Box(4, 32, 56, 32);
        }

        passangerId = -1;
        vehicleId = -1;
        exitVehicle = false;

        nameTag = "";

        if(type == BOMB){
            sprite.index = SPRBOMB;
            physics.bounceRot = true;
            hasCollision = true;
        }

        if(type == BOAT){

            hasCollision = true;
        }

        if(type == WALL){
            sprite.index = SPRNULL;
            sprite.xScl = 2;
            sprite.yScl = 2;
            physics.doGravity = false;
        }
    }



    /// Planning on doing each object a constructor
    void playerConstructor(int _clientId, sf::Color _color){
        type = PLAYER;
        sprite.index = SPRXAROP;
        clientId = _clientId;
        clientFollow = true;
        hasCollision = true;
        color = _color;

        sprite.xScl = 2;
        sprite.yScl = 2;
    }



    /// What a horrific function
    void horizontalMov(float _hspd){
        hacc = _hspd/3;
        if(hspd != 0){
            facing = (hspd > 0) ? 0 : 1;
        }
    }

    /// Another strange function
    // should probably
    void passangerExit(int id){
        if(id == passangerId){
            passangerId = -1;
        }
    }



    /// Draw function for Client Side

    void draw(sf::RenderWindow& windowDraw, ResourceHandler& resources, RoomInfo& roomInfo){
        View view = View();

        draw(windowDraw, resources, roomInfo, view);
    }

    void draw(sf::RenderWindow& windowDraw, ResourceHandler& resources, RoomInfo& roomInfo, View& view) {

        float xx = x + roomInfo.x - view.x;
        float yy = y + roomInfo.y - view.y;

        if (type == PLAYER) {

            drawSprite(windowDraw, resources, sprite.index, xx, yy, sprite.xScl, sprite.yScl, facing, ang);

            drawName(nameTag, windowDraw, resources, xx, yy);

        }
        else if (type == BOAT) {

            drawSprite(windowDraw, resources, SPRBOAT, xx, yy, sprite.xScl, sprite.yScl, facing, ang);

            //string str = "Boat";
            //drawName(str, windowDraw, resources);

        }
        else if (type == BOMB) {

            float colorVal = (1 - normalizeValue(bombObj.timer, 0, bombObj.explodeTime)) * 255;
            resources.sprites[SPRBOMB]->setColor(sf::Color(255, colorVal, colorVal));

            drawSprite(windowDraw, resources, SPRBOMB, xx, yy, sprite.xScl, sprite.yScl, facing, ang);

            resources.sprites[SPRBOMB]->setColor(sf::Color(255, 255, 255));

        }
        else if (type == WALL){
            if (repeatMap) {
                float sprWid = resources.sprites[sprite.index]->getLocalBounds().width;
                float sprHei = resources.sprites[sprite.index]->getLocalBounds().height;
                float boundWid = colBox.width;
                float boundHei = colBox.height;



                if (boundHei / sprHei > boundWid / sprWid) {
                    float xscl = boundWid / sprWid;

                    float yRepeat = boundHei / (sprHei * xscl);

                    for (int i = 0; i < yRepeat; i++) {
                        drawSprite(windowDraw, resources, sprite.index, xx, yy+(sprHei*xscl*i), xscl, xscl, facing, ang);
                    }
                }
                else {
                    float yscl = boundHei / sprHei;

                    float xRepeat = boundWid / (sprWid * yscl);

                    for (int i = 0; i < xRepeat; i++) {
                        drawSprite(windowDraw, resources, sprite.index, xx + (sprWid * yscl * i), yy, yscl, yscl, facing, ang);
                    }
                }

            }
            else {
                drawSprite(windowDraw, resources, sprite.index, xx, yy, sprite.xScl, sprite.yScl, facing, ang);
                drawBox(xx, yy, windowDraw);
            }
        } else if (type < OBJTOTAL) {

            drawSprite(windowDraw, resources, sprite.index, xx, yy, sprite.xScl, sprite.yScl, facing, ang);

        } else {
            sf::CircleShape shape(20);

            shape.setFillColor(color);
            shape.setPosition(xx, yy);
            windowDraw.draw(shape);


            string str = "Not Defined";


            drawName(str, windowDraw, resources, xx, yy);
        }


        //drawBox(xx, yy, windowDraw);
    }



    void drawName(string str, sf::RenderWindow& windowDraw, ResourceHandler& resources, float xx, float yy){
        sf::Text text;

        text.setFont(*resources.fonts[0]);
        //text.setColor(sf::Color::Black);
        text.setString(str);
        text.setPosition(xx, yy-20);
        text.setFillColor(sf::Color::White);
        text.setOutlineThickness(1);
        text.setOutlineColor(sf::Color::Black);
        text.setCharacterSize(20);

        windowDraw.draw(text);
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

    void drawBox(float xx, float yy, sf::RenderWindow& windowDraw){
        sf::RectangleShape rect(sf::Vector2f(colBox.width, colBox.height));
        rect.setFillColor(sf::Color(0, 0, 0, 0));
        rect.setOutlineColor(sf::Color::Black);
        rect.setOutlineThickness(2);
        rect.setPosition(colBox.x + xx, colBox.y + yy);
        windowDraw.draw(rect);
    }



    void printSelf(){
        cout << "Game Object " << id << "   Type: " << type << " X:" << x << " Y:" << y << " roomId:" << roomId << " clientId:" << clientId  << endl;
    }

};




sf::Packet & operator << (sf::Packet& packet, GameObject& obj){

    packet << obj.x << obj.y << obj.hspd << obj.vspd << obj.hacc << obj.vacc << obj.ang << obj.angSpd << obj.color << obj.id << obj.clientId << obj.roomId;
    packet << obj.clientFollow << obj.type << obj.physics << obj.colBox << obj.sprite << obj.playerObj << obj.bombObj << obj.warpObj << obj.facing << obj.holderId;
    packet << obj.passangerId << obj.vehicleId << obj.nameTag << obj.repeatMap;

    return packet;
}

sf::Packet & operator >> (sf::Packet& packet, GameObject& obj){

    packet >> obj.x >> obj.y >> obj.hspd >> obj.vspd >> obj.hacc >> obj.vacc >> obj.ang >> obj.angSpd >> obj.color >> obj.id >> obj.clientId >> obj.roomId;
    packet >> obj.clientFollow >> obj.type >> obj.physics >> obj.colBox >> obj.sprite >> obj.playerObj >> obj.bombObj >> obj.warpObj >> obj.facing >> obj.holderId;
    packet >> obj.passangerId >> obj.vehicleId >> obj.nameTag >> obj.repeatMap;

    return packet;
}



#endif // GAMEOBJECT_H_INCLUDED

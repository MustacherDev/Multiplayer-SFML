#ifndef GAMEOBJECTHANDLE_H_INCLUDED
#define GAMEOBJECTHANDLE_H_INCLUDED

/// Game Object Class
class GameObjectHandle{
    public:

    sf::Packet& roomPacket;
    sf::Packet& soundRequests;
    sf::Packet& roomPacketInstant;


    vector<GameObject*>& gameObjects;
    GameObject* obj;




    GameObjectHandle(GameObject* _obj, vector<GameObject*>& _gameObjects, sf::Packet& _roomPacket, sf::Packet& _roomPacketInstant, sf::Packet& _soundRequestsPacket) : gameObjects(_gameObjects), roomPacket(_roomPacket), roomPacketInstant(_roomPacketInstant), soundRequests(_soundRequestsPacket){
        obj = _obj;
    }



    void update(RoomInfo& roomInfo){

        /// Player Object
        if(obj->type == PLAYER){
            if(obj->vehicleId == -1){
                int objectNum = gameObjects.size();

                for(int i = 0; i < objectNum; i++){
                    GameObject* other = gameObjects[i];
                    if(other->id != obj->id){
                        if(other->active){
                            if(other->type == BOAT){
                                if(other->passangerId == -1){
                                    if(collisionDetection(obj->x, obj->y, other)){
                                        other->passangerId = obj->id;
                                        obj->vehicleId = other->id;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } else if(obj->type == BOAT){
            if(obj->passangerId != -1){
                GameObject* other = findGameObjectById(gameObjects, obj->passangerId);
                if(other == nullptr){
                    obj->passangerId = -1;
                }
            }
        } else if (obj->type == BOMB){
            int objectNum = gameObjects.size();

            obj->bombObj.timer++;

            if(obj->bombObj.timer > obj->bombObj.explodeTime){
                obj->active = false;
                sendPlaySound(soundRequests, 0);
                sendShakeRoom(roomPacket, 20, 100);

                int smokeNum = ((rand()%20) + 30);
                for(int j = 0; j < smokeNum; j++){
                    float xx = obj->x + ((rand()%80) - 40);
                    float yy = obj->y + ((rand()%80) - 40);

                    float spd = ((float)((rand()%30) + 20))/150;
                    float randAngleDegrees = rand()%180;
                    float angle = PI*(randAngleDegrees)/180;

                    float hspd = cos(angle)*spd;
                    float vspd = sin(angle)*spd;


                    int life = (rand()%150) + 400;

                    int spriteIndex = SPRDUST1 + (rand()%3);

                    ParticleObject part = ParticleObject(xx, yy, hspd, vspd, spriteIndex, life, getNewUID());
                    part.sprite.xScl = 2;
                    part.sprite.yScl = 2;



                    sendParticleCreate(roomPacket, &part);
                }


                for(int j = 0; j < objectNum; j++){
                    GameObject* other2 = gameObjects[j];

                    cout << "Finding Speedy" << endl;

                    if(other2->active){
                        if(other2->type == PLAYER || other2->type == BOAT || other2->type == BOMB){
                            float areaRadius = 150;
                            float xx = obj->x - areaRadius;
                            float yy = obj->y - areaRadius;

                            if(customCollisionDetection(xx, yy, areaRadius*2, areaRadius*2, other2)){
                                float dx = other2->x - obj->x;
                                float dy = other2->y - obj->y;

                                float sqrDist = dx*dx + dy*dy;

                                if(sqrDist < areaRadius*areaRadius){
                                    float invSqrDist = 1/sqrDist;

                                    float impHspd = dx*invSqrDist*areaRadius*3;
                                    impHspd = clamp(impHspd, -10, 10);

                                    float impVspd = dy*invSqrDist*areaRadius*3;
                                    impVspd = clamp(impVspd, -10, 10);

                                    other2->hspd += impHspd;
                                    other2->vspd += impVspd;

                                    cout << "Speedy Hspd " << impHspd << " Vspd " << impVspd << endl;
                                }
                            }

                        }
                    }

                }
            } else {

                for(int i = 0; i < objectNum; i++){
                    GameObject* other = gameObjects[i];
                    if(other->id != obj->id){
                        if(other->active){
                            if(other->type != WALL && other->type != BOAT){
                                if(collisionDetection(obj->x, obj->y, other)){
                                    obj->active = false;
                                    sendPlaySound(soundRequests, 0);
                                    sendShakeRoom(roomPacket, 20, 100);

                                    int smokeNum = ((rand()%20) + 30);
                                    for(int j = 0; j < smokeNum; j++){
                                        float xx = obj->x + ((rand()%80) - 40);
                                        float yy = obj->y + ((rand()%80) - 40);

                                        float spd = ((float)((rand()%30) + 20))/150;
                                        float randAngleDegrees = rand()%180;
                                        float angle = PI*(randAngleDegrees)/180;

                                        float hspd = cos(angle)*spd;
                                        float vspd = sin(angle)*spd;


                                        int life = (rand()%150) + 400;

                                        int spriteIndex = SPRDUST1 + (rand()%3);

                                        ParticleObject part = ParticleObject(xx, yy, hspd, vspd, spriteIndex, life, getNewUID());
                                        part.sprite.xScl = 2;
                                        part.sprite.yScl = 2;



                                        sendParticleCreate(roomPacket, &part);
                                    }


                                    for(int j = 0; j < objectNum; j++){
                                        GameObject* other2 = gameObjects[j];

                                        cout << "Finding Speedy" << endl;

                                        if(other2->active){
                                            if(other2->type == PLAYER || other2->type == BOAT || other2->type == BOMB){
                                                float areaRadius = 200;
                                                float xx = obj->x - areaRadius;
                                                float yy = obj->y - areaRadius;

                                                if(customCollisionDetection(xx, yy, areaRadius*2, areaRadius*2, other2)){
                                                    float dx = other2->x - obj->x;
                                                    float dy = other2->y - obj->y;

                                                    float sqrDist = dx*dx + dy*dy;

                                                    if(sqrDist < areaRadius*areaRadius){
                                                        float invSqrDist = 1/sqrDist;

                                                        float impHspd = dx*invSqrDist*areaRadius*3;
                                                        impHspd = clamp(impHspd, -10, 10);

                                                        float impVspd = dy*invSqrDist*areaRadius*3;
                                                        impVspd = clamp(impVspd, -10, 10);

                                                        other2->hspd += impHspd;
                                                        other2->vspd += impVspd;

                                                        cout << "Speedy Hspd " << impHspd << " Vspd " << impVspd << endl;
                                                    }
                                                }

                                            }
                                        }

                                    }

                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }

        /// Add Walls
        /// Add Good Stuff





        if(obj->vehicleId == -1){


            if(obj->holderId == -1){

                if(obj->physics.doGravity){
                    obj->hspd += roomInfo.hGravity;
                    obj->vspd += roomInfo.vGravity;
                }

                obj->hspd += obj->hacc;
                obj->vspd += obj->vacc;

                obj->hacc = 0;
                obj->vacc = 0;

                if(obj->physics.doHDamp){
                    obj->hspd *= obj->physics.hDamp;
                }

                if(obj->physics.doVDamp){
                    obj->vspd *= obj->physics.vDamp;
                }

                obj->hspd = clamp(obj->hspd, -obj->physics.hspdMax, obj->physics.hspdMax);
                obj->vspd = clamp(obj->vspd, -obj->physics.vspdMax, obj->physics.vspdMax);



                /// Room Walls Vertical Collision
                if(obj->y > roomInfo.height - obj->colBox.height - obj->colBox.y){
                    obj->y = roomInfo.height - obj->colBox.height - obj->colBox.y;

                    if(obj->physics.bounceRot){

                        if(abs(obj->vspd) > 0.5){
                            int sign = (obj->hspd > 0) ? 1:-1;
                            obj->angSpd += sign*sqrt(abs(obj->hspd));
                        }
                    }

                    obj->vspd *= -obj->physics.vRest;

                    if(abs(obj->vspd) < 1){
                        obj->vspd = 0;
                    }


                    obj->playerObj.jumps = obj->playerObj.jumpsMax;
                }

                if(obj->y < 0 - obj->colBox.y){
                    obj->y = - obj->colBox.y;
                    obj->vspd *= -obj->physics.vRest;

                }



                /// Vertical Collisions
                if(obj->hasCollision){
                    if(placeMeeting(obj->x, obj->y + obj->vspd, WALL)){
                        int vSign = sign((int)obj->vspd);
                        int dist = 0;


                        if(!vSign > 0){
                            obj->playerObj.jumps = obj->playerObj.jumpsMax;
                        }

                        if(vSign != 0){
                            while(!placeMeeting(obj->x, obj->y + vSign, WALL) && abs(dist) < abs(obj->vspd)){
                                cout << "colliding X:" << obj->x << " Y:" << obj->y  << " Hspd:" << obj->hspd << " Vspd:" << obj->vspd << endl;
                                obj->y += vSign;
                                dist += vSign;
                            }

                            //obj->x = floor(obj->x);
                            //obj->y = floor(obj->y);

                            obj->vspd *= -obj->physics.vRest;

                            if(abs(obj->vspd) < 1){
                                obj->vspd = 0;
                            }

                            if(obj->physics.bounceRot){
                                if(abs(obj->hspd) > 0.5){
                                    obj->angSpd += vSign*sqrt(abs(obj->vspd));
                                }
                            }
                        } else {
                            obj->vspd = 0;
                        }
                    }
                }

                obj->y += obj->vspd;








                /// Horizontal
                if(obj->hasCollision){
                    if(placeMeeting(obj->x + obj->hspd, obj->y, WALL)){
                        int hSign = sign((int)obj->hspd);
                        int dist = 0;

                        //cout << " Enganchando " << endl;

                        if(hSign != 0){
                            while(!placeMeeting(obj->x + hSign, obj->y, WALL) && abs(dist) < abs(obj->hspd)){
                                cout << "colliding X:" << obj->x << " Y:" << obj->y  << " Hspd:" << obj->hspd << " Vspd:" << obj->vspd << endl;
                                obj->x += hSign;
                                dist += hSign;
                            }

                            //obj->x = floor(obj->x);
                            //obj->y = floor(obj->y);


                            obj->hspd *= -obj->physics.hRest;
                            //obj->hspd = 0;

                            if(abs(obj->hspd) < 1){
                                obj->hspd = 0;
                            }


                            if(obj->physics.bounceRot){
                                if(abs(obj->vspd) > 0.5){
                                    obj->angSpd += hSign*sqrt(abs(obj->hspd));
                                }
                            }
                        } else {
                            obj->hspd = 0;
                        }
                    }
                }

                obj->x += obj->hspd;







                obj->angSpd *= 0.997;
                obj->ang += obj->angSpd;
            } else {

                GameObject* other = findGameObjectById(gameObjects, obj->holderId);

                if(other != nullptr){
                    obj->y = other->y-10;
                    obj->x = other->x + (obj->facing*20) - 10;

                } else {

                    obj->holderId = -1;

                }


            }



         } else {
            GameObject* other = findGameObjectById(gameObjects, obj->vehicleId);

            if(other != nullptr){
                obj->y = other->y-10;
                obj->x = other->x;
                obj->exitVehicle = false;
            } else {
                if(obj->exitVehicle){
                    exitVehicle();
                    obj->exitVehicle = false;
                } else {
                    obj->exitVehicle = true;
                }
            }

         }


    }

    void exitVehicle(){

        GameObject* other = findGameObjectById( gameObjects,obj->vehicleId);

        if(other != nullptr){
            other->passangerExit(obj->id);
        }

        obj->vehicleId = -1;
    }




    /// Detects collision with a specific object
    bool collisionDetection(float xx, float yy, GameObject* other){
        Box b1(obj->colBox.x + (int)xx, obj->colBox.y + (int)yy, obj->colBox.width, obj->colBox.height);
        Box b2(other->colBox.x + (int)other->x, other->colBox.y + (int)other->y, other->colBox.width, other->colBox.height);

        return b1.intersect(b2);
    }

    bool customCollisionDetection(float xx, float yy, float width, float height, GameObject* other){
        Box b1((int)xx, (int)yy, (int)width, (int)height);
        Box b2(other->colBox.x + (int)other->x, other->colBox.y + (int)other->y, other->colBox.width, other->colBox.height);

        return b1.intersect(b2);
    }



    /// Returns whether or not the object have collided
    bool placeMeeting(float xx, float yy, int type){

        Box b1(obj->colBox.x + (int)xx, obj->colBox.y + (int)yy, obj->colBox.width, obj->colBox.height);


        int gameObjectNumber = (int)gameObjects.size();

        for(int i = 0; i < gameObjectNumber; i++){
            GameObject* other = gameObjects[i];
            if(other->active){
                if(other->type == type || type == GAMEOBJECT){
                    Box b2(other->colBox.x + (int)other->x, other->colBox.y + (int)other->y, other->colBox.width, other->colBox.height);

                    if(b1.intersect(b2)){
                        return true;
                    }
                }
            }
        }

        return false;

    }


    /// Searches through the gameObjects and return all colliders
    vector<GameObject*> instancePlace(float xx, float yy, int type){
        Box b1(obj->colBox.x + (int)xx, obj->colBox.y + (int)yy, obj->colBox.width, obj->colBox.height);


        vector<GameObject*> colliders;

        int gameObjectNumber = (int)gameObjects.size();

        for(int i = 0; i < gameObjectNumber; i++){
            GameObject* other = gameObjects[i];
            if(other->active){
                if(other->type == type || type == GAMEOBJECT){
                    Box b2(other->colBox.x + (int)other->x, other->colBox.y + (int)other->y, other->colBox.width, other->colBox.height);

                    if(b1.intersect(b2)){
                        colliders.push_back(other);
                    }
                }
            }
        }

        return colliders;
    }

};


#endif // GAMEOBJECTHANDLE_H_INCLUDED

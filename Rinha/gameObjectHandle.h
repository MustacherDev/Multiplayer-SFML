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

    void playerUpdate() {

        // Do nothing if already in vehicle
        if (obj->vehicleId != -1) {
            return;
        }

        int objectNum = gameObjects.size();

        for (int i = 0; i < objectNum; i++) {
            GameObject* other = gameObjects[i];

            // Skip other if it is self, not active, not a boat or has a passanger
            if (other->id == obj->id || !other->active || other->type != BOAT || other->passangerId != -1) {
                continue;
            }

            if (collisionDetection(obj->x, obj->y, other)) {
                other->passangerId = obj->id;
                obj->vehicleId = other->id;
            }
        }
    }

    void boatUpdate() {
        if (obj->passangerId != -1) {
            GameObject* other = findGameObjectById(gameObjects, obj->passangerId);
            if (other == nullptr) {
                obj->passangerId = -1;
            }
        }
    }

    void bombUpdate() {
        
        int objectNum = gameObjects.size();

        obj->bombObj.timer++;

        if (obj->bombObj.timer > obj->bombObj.explodeTime) {
            obj->active = false;

            int smokeNum = ((rand() % 20) + 30);

            sendPlaySound(soundRequests, 0);
            sendShakeRoom(roomPacket, 20, 100);
            sendParticlePatternCreate(roomPacket, PARTPATTERNEXPLOSION, smokeNum, obj->x, obj->y);


            for (int j = 0; j < objectNum; j++) {
                GameObject* other2 = gameObjects[j];

                if (!other2->active) {
                    continue;
                }

                if (other2->type != PLAYER && other2->type != BOAT && other2->type != BOMB) {
                    continue;
                }

                float areaRadius = 150;
                float xx = obj->x - areaRadius;
                float yy = obj->y - areaRadius;

                if (customCollisionDetection(xx, yy, areaRadius * 2, areaRadius * 2, other2)) {
                    float dx = other2->x - obj->x;
                    float dy = other2->y - obj->y;

                    float sqrDist = dx * dx + dy * dy;

                    if (sqrDist < areaRadius * areaRadius) {
                        float invSqrDist = 1 / sqrDist;

                        float impHspd = dx * invSqrDist * areaRadius * 3;
                        impHspd = clamp(impHspd, -10, 10);

                        float impVspd = dy * invSqrDist * areaRadius * 3;
                        impVspd = clamp(impVspd, -10, 10);

                        other2->hspd += impHspd;
                        other2->vspd += impVspd;

                        println("Explosion Force -> Hspd " << impHspd << " / Vspd " << impVspd);
                    }
                }
            }
        }
        else {

            for (int i = 0; i < objectNum; i++) {
                GameObject* other = gameObjects[i];

                if (!other->active || other->id == obj->id || other->type == WALL || other->type == BOAT) {
                    continue;
                }

                if (collisionDetection(obj->x, obj->y, other)) {
                    obj->bombObj.timer = obj->bombObj.explodeTime;
                }
                
            }
            
        }
        

    }

    void warperUpdate() {
        if (randInt(15) == 1) {
            sendParticlePatternCreate(roomPacket, PARTPATTERNEXPLOSION, 1, obj->x, obj->y);
        }
    }

    void update(RoomInfo& roomInfo){

        /// Object Specific Updates
        if(obj->type == PLAYER){
            playerUpdate();
        } else if(obj->type == BOAT){
            boatUpdate();
        } else if (obj->type == BOMB) {
            bombUpdate();
        } else if (obj->type == WARPER) {
            warperUpdate();
        }

        /// Add Walls
        /// Add Good Stuff

        if (obj->warpObj.cooldown > 0) {
            obj->warpObj.cooldown--;
        }


        // If does not have a vehicle
        if(obj->vehicleId == -1){


            if(obj->holderId == -1){


                // Adding gravity force
                if(obj->physics.doGravity){
                    obj->hspd += roomInfo.hGravity;
                    obj->vspd += roomInfo.vGravity;
                }

                // Adding aceleration
                obj->hspd += obj->hacc;
                obj->vspd += obj->vacc;

                obj->hacc = 0;
                obj->vacc = 0;

                // Dampness
                if(obj->physics.doHDamp){
                    obj->hspd *= obj->physics.hDamp;
                }

                if(obj->physics.doVDamp){
                    obj->vspd *= obj->physics.vDamp;
                }

                // Limiting Velocity
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


                        if(!(vSign > 0)){
                            obj->playerObj.jumps = obj->playerObj.jumpsMax;
                        }

                        if(vSign != 0){
                            while(!placeMeeting(obj->x, obj->y + vSign, WALL) && abs(dist) < abs(obj->vspd)){
                                //println("colliding X:" << obj->x << " Y:" << obj->y  << " Hspd:" << obj->hspd << " Vspd:" << obj->vspd);
                                obj->y += vSign;
                                dist += vSign;
                            }

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

                      

                        if(hSign != 0){
                            while(!placeMeeting(obj->x + hSign, obj->y, WALL) && abs(dist) < abs(obj->hspd)){
                                //println("colliding X:" << obj->x << " Y:" << obj->y  << " Hspd:" << obj->hspd << " Vspd:" << obj->vspd);
                                obj->x += hSign;
                                dist += hSign;
                            }

                            obj->hspd *= -obj->physics.hRest;

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

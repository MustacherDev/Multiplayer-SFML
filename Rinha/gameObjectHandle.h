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

                if (other2->type != PLAYER && other2->type != BOAT && other2->type != BOMB && other2->type != BRICK) {
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

                        if (other2->type == BRICK){
                            if (abs(impHspd) + abs(impVspd) > 5) {
                                other2->brickObj.breaked = true;
                            }
                        }
                        else {
                            other2->hspd += impHspd;
                            other2->vspd += impVspd;
                            println("Explosion Force -> Hspd " << impHspd << " / Vspd " << impVspd);
                        }


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
        if (randInt(20) == 1) {
            sendParticlePatternCreate(roomPacket, PARTPATTERNEXPLOSION, 1, obj->x, obj->y);
        }
    }

    void brickUpdate() {
        if (obj->brickObj.breaked) {
            obj->active = false;

            float xScl = obj->spriteData.xScl;
            float yScl = obj->spriteData.yScl;

            float wid = obj->colBox.width;
            float hei = obj->colBox.height;

            for (int i = 0; i < 2; i++) {
                for (int j = 0; j < 2; j++) {
                    ParticleObject part;
                    part.spriteData.index = SPRNULLBITS;
                    part.spriteData.imgNumber = j + 2*i;
                    part.spriteData.xRotCenter = wid / 4;
                    part.spriteData.yRotCenter = hei / 4;
                    part.spriteData.xScl = xScl;
                    part.spriteData.yScl = yScl;
                    part.physics.doGravity = true;
                    part.physics.doHDamp = false;
                    part.physics.vspdMax = 10;
                    part.vacc = 0.02;
                    part.x = obj->x + (j * wid / 2);
                    part.y = obj->y + (i * hei / 2);
                    part.life = 600;

                    part.hspd = -0.5 + j * 1;
                    part.vspd = -3;

                    part.angSpd = part.hspd / 4;

                    sendParticleCreate(roomPacket, &part);
                }
            }
            sendParticlePatternCreate(roomPacket, PARTPATTERNEXPLOSION, 2, obj->x, obj->y);
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
        } else if (obj->type == BRICK) {
            brickUpdate();
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



                std::vector<int> colliderTypes;
                colliderTypes.push_back(WALL);
                colliderTypes.push_back(BRICK);



                /// Vertical Collisions
                if(obj->hasCollision){
                    if(placeMeeting(obj->x, obj->y + obj->vspd, colliderTypes)){
                        int vSign = sign((int)obj->vspd);
                        int dist = 0;


                        if(!(vSign > 0)){
                            obj->playerObj.jumps = obj->playerObj.jumpsMax;
                        }

                        if(vSign != 0){
                            while(!placeMeeting(obj->x, obj->y + vSign, colliderTypes) && abs(dist) < abs(obj->vspd)){
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
                    if(placeMeeting(obj->x + obj->hspd, obj->y, colliderTypes)){
                        int hSign = sign((int)obj->hspd);
                        int dist = 0;

                      

                        if(hSign != 0){
                            while(!placeMeeting(obj->x + hSign, obj->y, colliderTypes) && abs(dist) < abs(obj->hspd)){
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
    bool placeMeeting(float xx, float yy, std::vector<GameObject*> objects) {

        Box b1(obj->colBox.x + (int)xx, obj->colBox.y + (int)yy, obj->colBox.width, obj->colBox.height);


        int gameObjectNumber = (int)objects.size();

        for (int i = 0; i < gameObjectNumber; i++) {
            GameObject* other = objects[i];
            if (other->active) {
                //if (other->type == type || type == GAMEOBJECT) {
                    Box b2(other->colBox.x + (int)other->x, other->colBox.y + (int)other->y, other->colBox.width, other->colBox.height);

                    if (b1.intersect(b2)) {
                        return true;
                    }
                //}
            }
        }

        return false;

    }

    int findIntInVector(int val, std::vector<int> vec) {
        for (int i = 0; i < vec.size(); i++) {
            if (vec[i] == val){
                return i;
            }
        }
        return -1;
    }

    std::vector<GameObject*> fetchGameObjectsByType(std::vector<GameObject*> objects, int type) {

        std::vector<GameObject*> filteredObjects;
        for (GameObject* obj : objects) {
            if (obj->type == type) {
                filteredObjects.push_back(obj);
            }
        }

        return filteredObjects;
    }

    std::vector<GameObject*> fetchGameObjectsByType(std::vector<GameObject*> objects, std::vector<int> types) {

        std::vector<GameObject*> filteredObjects;
        for (GameObject* obj : objects) {
            if (findIntInVector(obj->type, types) != -1) {
                filteredObjects.push_back(obj);
            }
        }

        return filteredObjects;
    }

    /// Returns whether or not the object have collided
    bool placeMeeting(float xx, float yy, int type){
        std::vector<GameObject*> objects = fetchGameObjectsByType(gameObjects, type);

        return placeMeeting(xx, yy, objects);
    }

    /// Returns whether or not the object have collided
    bool placeMeeting(float xx, float yy, std::vector<int> types) {
        std::vector<GameObject*> objects = fetchGameObjectsByType(gameObjects, types);

        return placeMeeting(xx, yy, objects);
    }



    /// Searches through the gameObjects and return all colliders
    std::vector<GameObject*> instancePlace(float xx, float yy, std::vector<GameObject*> objects){
        Box b1(obj->colBox.x + (int)xx, obj->colBox.y + (int)yy, obj->colBox.width, obj->colBox.height);


        std::vector<GameObject*> colliders;

        int gameObjectNumber = (int)objects.size();

        for(int i = 0; i < gameObjectNumber; i++){
            GameObject* other = objects[i];
            if(other->active){       
                Box b2(other->colBox.x + (int)other->x, other->colBox.y + (int)other->y, other->colBox.width, other->colBox.height);

                if(b1.intersect(b2)){
                    colliders.push_back(other);
                }    
            }
        }

        return colliders;
    }

    /// Searches through the gameObjects and return all colliders
    std::vector<GameObject*> instancePlace(float xx, float yy, int type) {
        std::vector<GameObject*> objects = fetchGameObjectsByType(gameObjects, type);

        return instancePlace(xx, yy, objects);
    }

    std::vector<GameObject*> instancePlace(float xx, float yy, std::vector<int> types) {
        std::vector<GameObject*> objects = fetchGameObjectsByType(gameObjects, types);

        return instancePlace(xx, yy, objects);
    }

};


#endif // GAMEOBJECTHANDLE_H_INCLUDED

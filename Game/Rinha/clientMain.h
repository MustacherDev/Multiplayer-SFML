#ifndef CLIENTMAIN_H_INCLUDED
#define CLIENTMAIN_H_INCLUDED




void drawSprite(sf::RenderWindow& windowDraw, ResourceHandler& resources, int spriteIndex, float x, float y, float xScl, float yScl, int facing, float ang){


    sf::FloatRect _rect = resources.sprites[spriteIndex]->getLocalBounds();
    float sprWidth = _rect.width;
    float sprHeight = _rect.height;

    if(facing == 1){

        resources.sprites[spriteIndex]->setOrigin(sprWidth/2, sprHeight/2);
        resources.sprites[spriteIndex]->setRotation(ang);

        resources.sprites[spriteIndex]->setPosition(x + ((sprWidth/2))*xScl, y + ((sprHeight/2))*yScl);
        resources.sprites[spriteIndex]->setScale(-xScl, yScl);

        windowDraw.draw(*(resources.sprites[spriteIndex]));
    } else {

        resources.sprites[spriteIndex]->setOrigin(sprWidth/2, sprHeight/2);
        resources.sprites[spriteIndex]->setRotation(ang);


        resources.sprites[spriteIndex]->setScale(xScl, yScl);
        resources.sprites[spriteIndex]->setPosition(x + ((sprWidth/2))*xScl, y + ((sprHeight/2))*yScl);
        windowDraw.draw(*(resources.sprites[spriteIndex]));
    }



}

class TextBox{
    public:
    Button area;
    string textStr;
    sf::Text text;
    bool hasSprite;
    int textLimit = 20;

    bool active = false;
    bool readingText = false;

    TextBox(){
        textStr = "";
        text = sf::Text();
        area = Button(0, 0, 100, 10, 0);
        hasSprite = false;
    }

    TextBox(float x, float y, float width, float height){
        textStr = "";
        text = sf::Text();
        area = Button(x, y, width, height, 0);
        hasSprite = false;
    }

    void draw(sf::RenderWindow& windowDraw, ResourceHandler& resources){
        if(hasSprite){
            area.draw(windowDraw, resources);
        }

        area.drawBox(windowDraw);

        if(readingText){
            float x = area.box.x;
            float y = area.box.y;
            float wid = area.box.width;
            float hei = area.box.height;

            sf::RectangleShape rect = sf::RectangleShape(sf::Vector2f(wid, hei));

            rect.setPosition(x , y);

            rect.setFillColor(sf::Color::Yellow);
            windowDraw.draw(rect);
        }

        text.setFont(*resources.fonts[0]);
        text.setPosition(area.box.x, area.box.y);
        text.setCharacterSize(20);
        text.setFillColor(sf::Color::Black);

        windowDraw.draw(text);
    }

    void update(float x, float y, bool click){
        if(click){
            if(area.contain(x, y)){
                if(readingText){
                    readingText = false;

                } else {
                    readingText = true;
                }
            } else {
                readingText = false;
            }
        }
    }

    void addText(char character){
        if(textStr.length() < textLimit){
            textStr += character;
            text.setString(textStr);
        }
        cout <<"Texto : " <<  textStr << endl;
    }

    void subText(){
        if(textStr.length() > 0){
            textStr.pop_back();
            text.setString(textStr);
        }
    }
};








void clientMain(){
    // CLIENT PART OF THE PROGRAM

    sf::TcpSocket socket;
    socket.setBlocking(true);

    cout << "Digite (L) para acessar o ip Local com porta 2000" << endl;
    cout << "Digite qualquer coisa para inserir um IP e Porta desejados" << endl;


    char answer1 = 'A';
    cin >> answer1;

    sf::IpAddress ip;
    int port;

    if(answer1 == 'L'){
        ip = sf::IpAddress::getLocalAddress();
        port = 2000;
    } else {
        string ipString;
        cout << "Digite o Ip   Ex.: 195.132.0.115" << endl;
        cin >> ipString;

        cout << "Digite a Porta Ex.: 2000" << endl;
        cin >> port;
        ip = sf::IpAddress(ipString);
    }


    int timesTried = 0;
    bool loop = true;

    while(loop && timesTried < 5){
        //system("pause");
        timesTried++;
        sf::Socket::Status status = socket.connect(ip,port);
        //sf::Socket::Status status = socket.connect(sf::IpAddress::getLocalAddress(),2000);
        if(status == sf::Socket::Done){
            cout << "Connected to Server" << endl;
            loop = false;
        } else{
            cout << "Connection Timeout :";
            if(status == sf::Socket::Error){
                cout << "error";
            } else if(status == sf::Socket::NotReady){
                cout << "notReady";
            }
            cout << endl;
        }
    }

    loop = true;




    /// First Packet with client ID
    int clientId = -1;

    sf::Packet firstPacket;

    sf::Socket::Status statusReceive = socket.receive(firstPacket);
    while(statusReceive == sf::Socket::Partial){
        cout << "Partial Packet received trying again" << endl;
        statusReceive = socket.receive(firstPacket);
    }

    firstPacket >> clientId;

    cout << "Client ID " << clientId << endl;


    socket.setBlocking(false);






    // All objects
    vector<GameObject*> allGameObjects;
    int gameObjectNumber = 0;

    // All particles
    vector<ParticleObject*> allParticles;
    int particleNumber = 0;

    struct RoomInfo roomInfo;












    /// Setting Up The Resources
    ResourceHandler resources;


    /// Textures and Sprites

    sf::Texture basicSpritesTexture;
    if (!basicSpritesTexture.loadFromFile("Sprites.png"))
    {
        cout << "Arquivo Sprite.png nao foi encontrado na pasta do programa!" << endl;
    }

    for(int i = 0; i < SPRTOTAL; i++){
        sf::Sprite* sprite = new sf::Sprite();
        sprite->setTexture(basicSpritesTexture);
        sprite->setTextureRect(sf::IntRect(i*32, 0, 32, 32));
        resources.sprites.push_back(sprite);
    }


    sf::Texture backgroundsTexture;
    if (!backgroundsTexture.loadFromFile("Backgrounds.png"))
    {
        cout << "Arquivo Backgrounds.png nao foi encontrado na pasta do programa!" << endl;
    }

    for(int i = 0; i < 2; i++){
        sf::Sprite* sprite = new sf::Sprite();
        sprite->setTexture(backgroundsTexture);
        sprite->setTextureRect(sf::IntRect(i*120, 0, 120, 60));
        resources.backgrounds.push_back(sprite);
    }



    /// Sounds
    sf::SoundBuffer soundExplosion;
    if (!soundExplosion.loadFromFile("Explosion.wav"))
    {
        cout << "Could Not load Sound Explosion.wav" << endl;
    }

    resources.sounds.push_back(&soundExplosion);





    /// Fonts
    sf::Font fontGame;
    if (!fontGame.loadFromFile("FSEX300.ttf"))
    {
        cout << "Could Not load Font FSEX300.ttf" << endl;
    }



    resources.fonts.push_back(&fontGame);




    ClientEventHandler eventHandler(allGameObjects, allParticles, roomInfo, resources);







    sf::RenderWindow window(sf::VideoMode(initWindowWidth, initWindowHeight), "THE MYTH");




    float frames = 0;



    /// Preparation Loop
    bool prepFinish = false;

    float arrowWid = 200;

    Button leftButton(0, 0, arrowWid, initWindowHeight, SPRARROW);
    leftButton.stretchSprite = true;
    leftButton.facing = 1;

    Button rightButton(initWindowWidth-arrowWid, 0, arrowWid, initWindowHeight, SPRARROW);
    rightButton.stretchSprite = true;


    int spriteIndexSelector = 1;



    bool mouseState = false;


    bool keyboardState[sf::Keyboard::KeyCount][3];

    for(int i = 0; i < sf::Keyboard::KeyCount; i++){
        keyboardState[i][0] = false;
        keyboardState[i][1] = false;
        keyboardState[i][2] = false;
    }

    bool windowFocus = true;

    bool jumpRequest = false;
    bool placeBlockRequested = false;

    bool actionStartRequest = false;
    bool actionStopRequest = false;


    int placeObjectType = 0;

    bool readingText = false;
    string readingTextInput = "";

    vector<TextBox*> textBoxes;




    TextBox textBox((initWindowWidth/2)-100, 20, 200, 20);
    textBoxes.push_back(&textBox);

    View activeView = View();

    sf::Packet serverPacket;


    /// Window Loop
    while (window.isOpen())
    {




        bool mousePressed = false;
        readingText = false;


        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed){
                window.close();
            }  else if(event.type == sf::Event::GainedFocus){
                std::cout << "Gained" << std::endl;
                windowFocus = true;
            }  else if(event.type == sf::Event::LostFocus){
                std::cout << "Lost" << std::endl;
                windowFocus = false;
            } else if(event.type == sf::Event::MouseButtonPressed){
                if(!mouseState){
                    mousePressed = true;
                }
                mouseState = true;
            } else if(event.type == sf::Event::MouseButtonReleased){
                mouseState = false;
                mousePressed = false;
            } else if (event.type == sf::Event::TextEntered) {
                    if (event.text.unicode < 128) {
                        //cout << "Arcor" << endl;

                        int textBoxNumber = (int)textBoxes.size();
                        for(int i = 0; i < textBoxNumber; i++){
                            TextBox* txtBox = textBoxes[i];
                            if(txtBox->readingText){
                                //cout << "A" << endl;
                                if(event.text.unicode > 31){
                                    txtBox->addText(static_cast<char>(event.text.unicode));
                                    //cout << "Letter: " << static_cast<char>(event.text.unicode) << endl;
                                } else if (event.text.unicode == 3 || event.text.unicode == 8){
                                    txtBox->subText();
                                }
                            }
                        }
                    }

            }
        }





        frames += 0.2;


        /// Background
        window.clear(sf::Color(255, 255, 255));

        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        float mouseX = mousePos.x;
        float mouseY = mousePos.y;



        if(windowFocus){
            /// KeyBoard
            for(int i = 0; i < sf::Keyboard::KeyCount; i++){
                bool keyState = sf::Keyboard::isKeyPressed((sf::Keyboard::Key)i);
                if(!keyboardState[i][0] && keyState){
                    keyboardState[i][1] = true;
                } else {
                    keyboardState[i][1] = false;
                }

                if(keyboardState[i][0] && !keyState){
                    keyboardState[i][2] = true;
                } else {
                    keyboardState[i][2] = false;
                }

                keyboardState[i][0] = keyState;
            }

        } else {
            /// KeyBoard Not active
            for(int i = 0; i < sf::Keyboard::KeyCount; i++){
                keyboardState[i][0] = false;
                keyboardState[i][1] = false;
                keyboardState[i][2] = false;
            }
        }







         if(roomInfo.id != -1){


            GameObject* myPlayer = findGameObjectByClientId(allGameObjects, clientId);
            if(myPlayer != nullptr){
                activeView.updateTargetPoint(myPlayer->x, myPlayer->y);
            }

            activeView.update(roomInfo);


            if(roomInfo.backgroundIndex != -1){
                sf::Sprite* spr = resources.backgrounds[roomInfo.backgroundIndex];
                spr->setScale(12, 12);
                sf::FloatRect bounds = spr->getLocalBounds();
                spr->setOrigin(sf::Vector2f(bounds.width/2, bounds.height/2));

                float xParalax = activeView.x/10;
                float yParalax = activeView.y/10;

                spr->setPosition(roomInfo.x + (initWindowWidth/2) - xParalax, roomInfo.y + (initWindowHeight/2) - yParalax);
                window.draw(*(resources.backgrounds[roomInfo.backgroundIndex]));
            }




            if(windowFocus){
                if(keyboardState[sf::Keyboard::Space][1]){
                    jumpRequest = true;
                }
            }






            /// Object Handling

            /// Updating Objects
            gameObjectNumber = (int)allGameObjects.size();
            for(int i = 0; i < gameObjectNumber; i++){
                GameObject* obj = allGameObjects[i];
                if(obj->active){
                    obj->draw(window, resources, roomInfo, activeView);
                }
            }


            /// Removing Inactive Objects
            for(int i = gameObjectNumber-1; i > -1; i--){
                GameObject* obj = allGameObjects[i];
                if(!obj->active){
                    delete obj;
                    allGameObjects.erase(allGameObjects.begin()+i);
                    gameObjectNumber--;
                }
            }



            /// Particle Handling

            /// Updating Particles
            particleNumber = (int)allParticles.size();
            for(int i = 0; i < particleNumber; i++){
                ParticleObject* obj = allParticles[i];
                if(obj->active){
                    obj->update();
                    obj->draw(window, resources, roomInfo, activeView);
                }
            }


            /// Removing Inactive Particles
            for(int i = particleNumber-1; i > -1; i--){
                ParticleObject* obj = allParticles[i];
                if(!obj->active){
                    delete obj;
                    allParticles.erase(allParticles.begin()+i);
                    particleNumber--;
                }
            }



             if(keyboardState[sf::Keyboard::C][1]){
                actionStartRequest = true;
             } else if(keyboardState[sf::Keyboard::C][2]){
                actionStopRequest = true;
             }




            if(keyboardState[sf::Keyboard::X][1]){
                placeObjectType++;
                if(placeObjectType > 2){
                    placeObjectType = 0;
                }
            }

            int xPlace =  floor((mouseX + activeView.x)/64)*64;
            int yPlace =  floor((mouseY + activeView.y)/64)*64;

            int objectPlaceSprite = 0;
            switch(placeObjectType){
                case 0:
                    objectPlaceSprite = SPRNULL;
                    break;

                case 1:
                    objectPlaceSprite = SPRBOMB;
                    break;

                case 2:
                    objectPlaceSprite = SPRBOAT;
                    break;

                default:
                    objectPlaceSprite = SPRNULL;
                    break;
            }


            drawSprite(window, resources, objectPlaceSprite, xPlace - activeView.x, yPlace - activeView.y, 2, 2, 0, 0.1);

            if(mousePressed && !placeBlockRequested){

                int objType = 0;
                switch(placeObjectType){
                    case 0:
                        objType = WALL;
                        break;

                    case 1:
                        objType = BOMB;
                        break;

                    case 2:
                        objType = BOAT;
                        break;

                    default:
                        objType = WALL;
                        break;
                }

                GameObject obj = GameObject(objType, xPlace, yPlace, -1);
                sendCreateObjectRequest(serverPacket, &obj, clientId);
                placeBlockRequested = true;
            }



        } else {




            leftButton.draw(window, resources);

            rightButton.draw(window, resources);

            int textBoxNumber = (int)textBoxes.size();
            for(int i = 0; i < textBoxNumber; i++){
                TextBox* txtBox = textBoxes[i];
                txtBox->update(mouseX, mouseY, mousePressed);
                txtBox->draw(window, resources);
                if(txtBox->readingText){
                    readingText = true;
                }
            }



            sf::Sprite* spr = resources.sprites[spriteIndexSelector];

            float xScl = 4;
            float yScl = 4;

            sf::FloatRect bounds = spr->getLocalBounds();
            float xx = (initWindowWidth/2) -(bounds.width*xScl/2);
            float yy = (initWindowHeight/2) - (bounds.height*yScl/2);
            spr->setPosition(xx, yy);
            spr->setScale(xScl, yScl);

            window.draw(*spr);

            if(mousePressed){
                if(leftButton.contain(mouseX, mouseY)){
                    spriteIndexSelector--;
                    if(spriteIndexSelector < 0){
                        spriteIndexSelector = SPRTOTAL-1;
                    }

                } else if(rightButton.contain(mouseX, mouseY)){
                    spriteIndexSelector++;
                    if(spriteIndexSelector > SPRTOTAL-1){
                        spriteIndexSelector = 0;
                    }
                }
            }

            if(!readingText){
                if(keyboardState[sf::Keyboard::A][1] || keyboardState[sf::Keyboard::Left][1]){
                    spriteIndexSelector--;
                    if(spriteIndexSelector < 0){
                        spriteIndexSelector = SPRTOTAL-1;
                    }
                }

                if(keyboardState[sf::Keyboard::D][1] || keyboardState[sf::Keyboard::Right][1]){
                    spriteIndexSelector++;
                    if(spriteIndexSelector > SPRTOTAL-1){
                        spriteIndexSelector = 0;
                    }
                }
            }




            if(keyboardState[sf::Keyboard::Enter][1] && !readingText && !prepFinish){

                prepFinish = true;

                // Sending Answer
                sf::Packet sendPacket;

                sendPacket << CLIENTEVENTENTER;
                sendPacket << spriteIndexSelector;
                sendPacket << textBoxes[0]->textStr;

                sf::Socket::Status sendStatus = socket.send(sendPacket);

                while(sendStatus == sf::Socket::Partial){
                    cout << "Failed: Sent to server" << endl;
                }

            }
        }




        /// Networking

        /// Receiving and Sending Packets
        sf::Packet receivePacket;

        sf::Socket::Status statusReceive = socket.receive(receivePacket);
        while(statusReceive == sf::Socket::Partial){
            cout << "Partial Packet received trying again" << endl;
            statusReceive = socket.receive(receivePacket);
        }

        if(statusReceive == sf::Socket::Done){

            while(receivePacket && !receivePacket.endOfPacket()){
                eventHandler.executeEvent(receivePacket);
            }

            gameObjectNumber = (int)allGameObjects.size();








            // Sending Answer
            sf::Packet sendPacket;

             if(jumpRequest){
                sendPacket << CLIENTEVENTJUMP;
                cout << "sentJump" << endl;
             }


             if(keyboardState[sf::Keyboard::Right][0] || keyboardState[sf::Keyboard::D][0]){
                sendPacket << CLIENTEVENTRIGHTPRESS;
             } else if(keyboardState[sf::Keyboard::Left][0] || keyboardState[sf::Keyboard::A][0]){
                sendPacket << CLIENTEVENTLEFTPRESS;
             }  else{
                sendPacket << CLIENTEVENTSTOPMOVEMENT;
             }

             if(actionStartRequest){
                sendPacket << CLIENTEVENTACTSTART;
             }

             if(actionStopRequest){
                sendPacket << CLIENTEVENTACTSTOP;
             }







            actionStartRequest = false;
            actionStopRequest = false;
            placeBlockRequested = false;
            jumpRequest = false;


            serverPacket.append(sendPacket.getData(), sendPacket.getDataSize());

            sf::Socket::Status sendStatus = socket.send(serverPacket);

            while(sendStatus == sf::Socket::Partial){
                cout << "Failed: Sent to server" << endl;
            }

            serverPacket.clear();
            sendPacket.clear();

        }




        /// Drawing GUI
        sf::Text text;
        string str = "RoomId ";

        str+= to_string(roomInfo.id);

        text.setFont(*resources.fonts[0]);
       // text.setColor(sf::Color::Black);
        text.setString(str);
        text.setPosition(+10, +10);
        text.setFillColor(sf::Color::White);
        text.setOutlineThickness(1);
        text.setOutlineColor(sf::Color::Black);
        text.setCharacterSize(20);

        window.draw(text);

        sf::Text text2;
        string str2 = "ClientId ";

        str2+= to_string(clientId);

        text2.setFont(*resources.fonts[0]);
        //text2.setColor(sf::Color::Black);
        text2.setString(str2);
        text2.setCharacterSize(20);
        text2.setFillColor(sf::Color::White);
        text2.setOutlineThickness(1);
        text2.setOutlineColor(sf::Color::Black);
        float text2Wid = text2.getGlobalBounds().width;
        text2.setPosition(initWindowWidth - text2Wid - 10, +10);






        window.draw(text2);






        window.display();


    }

}



#endif // CLIENTMAIN_H_INCLUDED

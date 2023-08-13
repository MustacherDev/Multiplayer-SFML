#ifndef CLIENTMAIN_H_INCLUDED
#define CLIENTMAIN_H_INCLUDED




void drawSprite(sf::RenderWindow& windowDraw, ResourceHandler& resources, int spriteIndex, float x, float y, float xScl, float yScl, int facing, float ang) {


	sf::FloatRect _rect = resources.sprites[spriteIndex]->getLocalBounds();
	float sprWidth = _rect.width;
	float sprHeight = _rect.height;

	if (facing == 1) {

		resources.sprites[spriteIndex]->setOrigin(sprWidth / 2, sprHeight / 2);
		resources.sprites[spriteIndex]->setRotation(ang);

		resources.sprites[spriteIndex]->setPosition(x + ((sprWidth / 2)) * xScl, y + ((sprHeight / 2)) * yScl);
		resources.sprites[spriteIndex]->setScale(-xScl, yScl);

		windowDraw.draw(*(resources.sprites[spriteIndex]));
	}
	else {

		resources.sprites[spriteIndex]->setOrigin(sprWidth / 2, sprHeight / 2);
		resources.sprites[spriteIndex]->setRotation(ang);


		resources.sprites[spriteIndex]->setScale(xScl, yScl);
		resources.sprites[spriteIndex]->setPosition(x + ((sprWidth / 2)) * xScl, y + ((sprHeight / 2)) * yScl);
		windowDraw.draw(*(resources.sprites[spriteIndex]));
	}



}

/*
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

	void setup(float x, float y, float width, float height) {
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
*/


class ConnectScreen {
public:
	Button localButton;
	Button confirmButton;
	ValBox ipBox;
	ValBox portBox;

	ConnectScreen() {

		float xCenter = initWindowWidth / 2;
		float yCenter = initWindowHeight / 2;

		float buttonWid = initWindowWidth / 3;
		float buttonHei = 50;


		localButton.init(xCenter - buttonWid / 2 - buttonWid / 2, yCenter - buttonHei / 2 + buttonHei * 3, buttonWid, buttonHei);
		localButton.setFont(basicFont);
		localButton.label = "Local server";
		// localButton.subLabel = "Connect to the local server";
		localButton.boxColor = sf::Color::Cyan;

		confirmButton.init(xCenter - buttonWid / 2 + buttonWid / 2, yCenter - buttonHei / 2 + buttonHei * 3, buttonWid, buttonHei);
		confirmButton.setFont(basicFont);
		confirmButton.label = "Connect";
		confirmButton.boxColor = sf::Color::Green;


		ipBox.init(2, xCenter - buttonWid / 2 + buttonWid / 2, yCenter - buttonHei / 2 - buttonHei, buttonWid, buttonHei, "");
		ipBox.label = "IP";
		ipBox.subLabel = "Ip";
		ipBox.subLabelText.setFillColor(sf::Color::Black);
		ipBox.setFont(basicFont);
		ipBox.confirmOutOfFocus = true;


		portBox.init(1, xCenter - buttonWid / 2 + buttonWid / 2, yCenter - buttonHei / 2 + buttonHei, buttonWid, buttonHei, 0);
		portBox.label = "Port";
		portBox.subLabel = "Port";
		portBox.subLabelText.setFillColor(sf::Color::Black);
		portBox.setFont(basicFont);
		portBox.confirmOutOfFocus = true;
	}

	void update(Input input, TextInput textInput) {
		localButton.update(input);
		confirmButton.update(input);

		ipBox.update(input, textInput.inputType, textInput.lastChar);
		portBox.update(input, textInput.inputType, textInput.lastChar);
	}

	void draw(sf::RenderWindow& window) {
		localButton.draw(window);
		confirmButton.draw(window);

		ipBox.draw(window);
		portBox.draw(window);
	}
};


class ClientGame {
public:
	// All objects
	vector<GameObject*> allGameObjects;
	int gameObjectNumber = 0;

	// All particles
	vector<ParticleObject*> allParticles;
	int particleNumber = 0;

	struct RoomInfo roomInfo;

	/// Setting Up The Resources
	ResourceHandler resources;


	// Textures
	sf::Texture basicSpritesTexture;
	sf::Texture backgroundsTexture;


	/// Sounds
	sf::SoundBuffer soundExplosion;
	sf::SoundBuffer soundWarp;

	sf::Font fontGame;

	ClientEventHandler* eventHandler;

	float frames = 0;



	/// Preparation Loop
	bool prepFinish = false;

	float arrowWid = 200;

	int spriteIndexSelector = 1;


	bool jumpRequest = false;
	bool placeBlockRequested = false;

	bool actionStartRequest = false;
	bool actionStopRequest = false;


	int placeObjectType = 0;
	float objectPlacingX = 0;
	float objectPlacingY = 0;

	bool readingText = false;
	string readingTextInput = "";

	ValBox textBox;

	Button leftButton;
	Button rightButton;

	View activeView;

	sf::Packet serverPacket;

	sf::TcpSocket socket;

	sf::IpAddress ip;
	int port = 2000;

	int clientId = -1;

	bool connected = false;
	bool tryingConnection = false;

	ConnectScreen connectScreen;

	ClientGame() {

	}

	void loadGame() {

		if (!basicSpritesTexture.loadFromFile("sprites/Sprites.png"))
		{
			cout << "Arquivo Sprite.png nao foi encontrado na pasta do programa!" << endl;
		}

		for (int i = 0; i < SPRTOTAL; i++) {
			sf::Sprite* sprite = new sf::Sprite();
			sprite->setTexture(basicSpritesTexture);
			sprite->setTextureRect(sf::IntRect(i * 32, 0, 32, 32));
			resources.sprites.push_back(sprite);
		}

		if (!backgroundsTexture.loadFromFile("sprites/Backgrounds.png"))
		{
			println("Arquivo Backgrounds.png nao foi encontrado na pasta do programa!");
		}

		for (int i = 0; i < 2; i++) {
			sf::Sprite* sprite = new sf::Sprite();
			sprite->setTexture(backgroundsTexture);
			sprite->setTextureRect(sf::IntRect(i * 120, 0, 120, 60));
			resources.backgrounds.push_back(sprite);
		}


		if (!soundExplosion.loadFromFile("sounds/Explosion.wav"))
		{
			println("Could Not load Sound Explosion.wav");
		}
		resources.sounds.push_back(&soundExplosion);

		if (!soundWarp.loadFromFile("sounds/Warp.wav"))
		{
			println("Could Not load Sound Warp.wav");
		}
		resources.sounds.push_back(&soundWarp);



		/// Fonts

		if (!fontGame.loadFromFile("FSEX300.ttf"))
		{
			println("Could Not load Font FSEX300.ttf");
		}
		resources.fonts.push_back(&fontGame);


		eventHandler = new ClientEventHandler(allGameObjects, allParticles, roomInfo, resources);



		leftButton.init(0, 0, arrowWid, initWindowHeight);


		rightButton.init(initWindowWidth - arrowWid, 0, arrowWid, initWindowHeight);

		textBox.init(2, (initWindowWidth / 2) - 100, 20, 200, 20, "");

		activeView = View();


	}

	void connect(Input& input, TextInput& textInput) {
		if (tryingConnection) {
			socket.setBlocking(true);
			sf::Socket::Status status = socket.connect(ip, port);

			if (status == sf::Socket::Done) {

				/// First Packet with client ID
				println("Connected to Server");

				sf::Packet firstPacket;

				sf::Socket::Status statusReceive = socket.receive(firstPacket);
				while (statusReceive == sf::Socket::Partial) {
					println("Partial Packet received trying again");
					statusReceive = socket.receive(firstPacket);
				}

				firstPacket >> clientId;

				println("Client ID " << clientId);

				connected = true;
				socket.setBlocking(false);
			}
			else {
				cout << "Connection Timeout :";
				if (status == sf::Socket::Error) {
					cout << "error";
				}
				else if (status == sf::Socket::NotReady) {
					cout << "notReady";
				}
				cout << endl;
			}
			tryingConnection = false;
		}
		else {

			connectScreen.update(input, textInput);

			if (connectScreen.localButton.clicked) {
				ip = sf::IpAddress::getLocalAddress();
				port = 2000;
				tryingConnection = true;
			}
			else if (connectScreen.confirmButton.clicked) {
				ip = connectScreen.ipBox.sVal;
				port = connectScreen.portBox.iVal;
				tryingConnection = true;
			}
		}
	}

	void run(sf::RenderWindow& window, Input& input, TextInput& textInput) {
		if (connected) {
			runGame(input, textInput);
		}
		else {
			connect(input, textInput);
		}
	}


	void runGame(Input& input, TextInput& textInput);

	void characterSelect(Input& input, TextInput& textInput) {
		// Controls for Selecting Character Sprite
		bool switchLeft = false;
		bool switchRight = false;

		switchLeft = leftButton.clicked || (!textBox.selected && ((input.keyboardState[sf::Keyboard::A][1] || input.keyboardState[sf::Keyboard::Left][1])));
		switchRight = rightButton.clicked || (!textBox.selected && ((input.keyboardState[sf::Keyboard::D][1] || input.keyboardState[sf::Keyboard::Right][1])));

		if (switchLeft) {
			spriteIndexSelector--;
			if (spriteIndexSelector < 0) {
				spriteIndexSelector = SPRTOTAL - 1;
			}

		}
		else if (switchRight) {
			spriteIndexSelector++;
			if (spriteIndexSelector > SPRTOTAL - 1) {
				spriteIndexSelector = 0;
			}
		}


		// Finishing Player Creation
		if (input.keyboardState[sf::Keyboard::Enter][1] && !textBox.selected && !prepFinish) {

			prepFinish = true;

			// Sending Answer
			sf::Packet sendPacket;

			sendPacket << CLIENTEVENTENTER;
			sendPacket << spriteIndexSelector;
			sendPacket << textBox.sVal;

			sf::Socket::Status sendStatus = socket.send(sendPacket);

			while (sendStatus == sf::Socket::Partial) {
				println("Failed: Sent to server");
				sendStatus = socket.send(sendPacket);
			}

		}


		textBox.update(input, textInput.inputType, textInput.lastChar);
	}


	void receiveAndSend(Input& input) {

		// Receiving
		sf::Packet receivePacket;

		sf::Socket::Status statusReceive = socket.receive(receivePacket);

		while (statusReceive == sf::Socket::Partial) {
			println("Partial Packet received trying again");
			statusReceive = socket.receive(receivePacket);
		}


		if (statusReceive == sf::Socket::Done) {

			// Processing Received Data
			while (receivePacket && !receivePacket.endOfPacket()) {
				eventHandler->executeEvent(receivePacket);
			}

			//gameObjectNumber = (int)allGameObjects.size();



			// Sending Answer
			sf::Packet sendPacket;

			if (jumpRequest) {
				sendPacket << CLIENTEVENTJUMP;
			}

			if (input.keyboardState[sf::Keyboard::Right][0] || input.keyboardState[sf::Keyboard::D][0]) {
				sendPacket << CLIENTEVENTRIGHTPRESS;
			}
			else if (input.keyboardState[sf::Keyboard::Left][0] || input.keyboardState[sf::Keyboard::A][0]) {
				sendPacket << CLIENTEVENTLEFTPRESS;
			}
			else {
				sendPacket << CLIENTEVENTSTOPMOVEMENT;
			}

			if (actionStartRequest) {
				sendPacket << CLIENTEVENTACTSTART;
			}

			if (actionStopRequest) {
				sendPacket << CLIENTEVENTACTSTOP;
			}


			actionStartRequest = false;
			actionStopRequest = false;
			placeBlockRequested = false;
			jumpRequest = false;


			serverPacket.append(sendPacket.getData(), sendPacket.getDataSize());

			sf::Socket::Status sendStatus = socket.send(serverPacket);

			while (sendStatus == sf::Socket::Partial) {
				println("Failed: Sent to server");
				sendStatus = socket.send(serverPacket);
			}

			serverPacket.clear();
			sendPacket.clear();

		}
	}

	void drawGUI(sf::RenderWindow& window) {
		sf::Text text;
		string str = "RoomId ";

		str += to_string(roomInfo.id);

		if (roomInfo.id == -1) {
			str = "Player Creation";
		}

		text.setFont(*resources.fonts[0]);
		text.setString(str);
		text.setPosition(+10, +10);
		text.setFillColor(sf::Color::White);
		text.setOutlineThickness(1);
		text.setOutlineColor(sf::Color::Black);
		text.setCharacterSize(20);

		window.draw(text);

		sf::Text text2;
		string str2 = "ClientId ";

		str2 += to_string(clientId);

		text2.setFont(*resources.fonts[0]);
		text2.setString(str2);
		text2.setCharacterSize(20);
		text2.setFillColor(sf::Color::White);
		text2.setOutlineThickness(1);
		text2.setOutlineColor(sf::Color::Black);
		float text2Wid = text2.getGlobalBounds().width;
		text2.setPosition(initWindowWidth - text2Wid - 10, +10);

		window.draw(text2);
	}

	void drawGame(sf::RenderWindow& window);

	void draw(sf::RenderWindow& window) {
		if (!connected) {
			connectScreen.draw(window);
		}
		else {
			drawGame(window);
		}
	}
};




void ClientGame::drawGame(sf::RenderWindow& window) {
	/// Background
	window.clear(sf::Color(255, 255, 255));

	if (roomInfo.id == -1) {
		leftButton.draw(window);
		rightButton.draw(window);
		textBox.draw(window);


		sf::Sprite* spr = resources.sprites[spriteIndexSelector];

		float xScl = 4;
		float yScl = 4;

		sf::FloatRect bounds = spr->getLocalBounds();
		float xx = (initWindowWidth / 2) - (bounds.width * xScl / 2);
		float yy = (initWindowHeight / 2) - (bounds.height * yScl / 2);
		spr->setPosition(xx, yy);
		spr->setScale(xScl, yScl);


		window.draw(*spr);
	}
	else {
		// Background
		if (roomInfo.backgroundIndex != -1) {
			sf::Sprite* spr = resources.backgrounds[roomInfo.backgroundIndex];
			spr->setScale(12, 12);
			sf::FloatRect bounds = spr->getLocalBounds();
			spr->setOrigin(sf::Vector2f(bounds.width / 2, bounds.height / 2));

			float xParalax = activeView.x / 10;
			float yParalax = activeView.y / 10;

			spr->setPosition(roomInfo.x + (initWindowWidth / 2) - xParalax, roomInfo.y + (initWindowHeight / 2) - yParalax);
			window.draw(*(resources.backgrounds[roomInfo.backgroundIndex]));
		}

		/// Drawing Objects
		for (GameObject* obj : allGameObjects) {
			if (obj->active) {
				obj->draw(window, resources, roomInfo, activeView);
			}
		}

		/// Drawing Particles
		for (ParticleObject* obj : allParticles) {
			if (obj->active) {
				obj->draw(window, resources, roomInfo, activeView);
			}
		}

		// Object Placement
		int objectPlaceSprite = 0;
		switch (placeObjectType) {
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

		drawSprite(window, resources, objectPlaceSprite, objectPlacingX - activeView.x, objectPlacingY - activeView.y, 2, 2, 0, 0.1);

		/// Drawing GUI
		drawGUI(window);
	}

}

void ClientGame::runGame(Input& input, TextInput& textInput) {

	frames += 0.2;

	

	if (roomInfo.id == -1) {
		characterSelect(input, textInput);
	} else {
		// Player
		GameObject* myPlayer = findGameObjectByClientId(allGameObjects, clientId);
		if (myPlayer != nullptr) {
			activeView.updateTargetPoint(myPlayer->x, myPlayer->y);
		}

		// View
		activeView.update(roomInfo);
		






		/// Object Handling
		// Removing Inactive Objects
		for (int i = allGameObjects.size() - 1; i > -1; i--) {
			GameObject* obj = allGameObjects[i];
			if (!obj->active) {
				delete obj;
				allGameObjects.erase(allGameObjects.begin() + i);
			}
		}

		/// Particle Handling
		// Updating Particles
		for (ParticleObject* obj : allParticles) {
			if (obj->active) {
				obj->update();
			}
		}
		// Removing Inactive Particles
		for (int i = allParticles.size() - 1; i > -1; i--) {
			ParticleObject* obj = allParticles[i];
			if (!obj->active) {
				delete obj;
				allParticles.erase(allParticles.begin() + i);
			}
		}


		/// Controls
		// Jumping
		if (input.keyboardState[sf::Keyboard::Space][1]) {
			jumpRequest = true;
		}

		// I dont know
		if (input.keyboardState[sf::Keyboard::C][1]) {
			actionStartRequest = true;
		}
		else if (input.keyboardState[sf::Keyboard::C][2]) {
			actionStopRequest = true;
		}

		// Object placing
		if (input.keyboardState[sf::Keyboard::X][1]) {
			placeObjectType++;
			if (placeObjectType > 2) {
				placeObjectType = 0;
			}
		}

		objectPlacingX = floor((input.mousePos.x + activeView.x) / 64) * 64;
		objectPlacingY = floor((input.mousePos.y + activeView.y) / 64) * 64;



		if (input.mouseState[0][1] && !placeBlockRequested) {

			int objType = 0;
			switch (placeObjectType) {
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

			GameObject obj = GameObject(objType, objectPlacingX, objectPlacingY, -1);
			sendCreateObjectRequest(serverPacket, &obj, clientId);
			placeBlockRequested = true;
		}



	}


	/// Networking
	// Receiving and Sending Packets
	receiveAndSend(input);

}



#endif // CLIENTMAIN_H_INCLUDED

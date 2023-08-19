#pragma once

class ResourceManager {
public:

	static sf::Texture& getTexture(const std::string& filename);
	static sf::SoundBuffer& getSound(const std::string& filename);
	static sf::Font& getFont(const std::string& filename);

	static void deleteTexture(const std::string& filename);
	static void deleteSound(const std::string& filename);
	static void deleteFont(const std::string& filename);



private:
	static std::map<std::string, sf::Texture> textures;
	static std::map<std::string, sf::SoundBuffer> sounds;
	static std::map<std::string, sf::Font> fonts;
};

std::map<std::string, sf::Texture> ResourceManager::textures;
std::map<std::string, sf::SoundBuffer> ResourceManager::sounds;
std::map<std::string, sf::Font> ResourceManager::fonts;

sf::Texture& ResourceManager::getTexture(const std::string& filename) {
	if (textures.find(filename) == textures.end()) {
		// Texture not found, load it from file
		sf::Texture texture;
		if (texture.loadFromFile(filename)) {
			textures[filename] = texture;
		}
		else {
			// Handle error if texture loading fails
			// You can throw an exception, log an error, etc.
			println("Error loading Texture " << filename);
		}
	}

	return textures[filename];
}


sf::SoundBuffer& ResourceManager::getSound(const std::string& filename) {
	if (sounds.find(filename) == sounds.end()) {
		// Sound not found, load it from file
		sf::SoundBuffer sound;
		if (sound.loadFromFile(filename)) {
			sounds[filename] = sound;
		}
		else {
			// Handle error if sound loading fails
			// You can throw an exception, log an error, etc.
			println("Error loading Sound " << filename);
		}
	}

	return sounds[filename];
}


sf::Font& ResourceManager::getFont(const std::string& filename) {
	if (fonts.find(filename) == fonts.end()) {
		// Font not found, load it from file
		sf::Font font;
		if (font.loadFromFile(filename)) {
			fonts[filename] = font;
		}
		else {
			// Handle error if font loading fails
			// You can throw an exception, log an error, etc.
			println("Error loading Font " << filename);
		}
	}

	return fonts[filename];
}

void ResourceManager::deleteTexture(const std::string& filename) {
	auto it = textures.find(filename);
	if (it != textures.end()) {
		textures.erase(it);
		// Optional: The texture will be automatically deleted when it goes out of scope
		// But, you can explicitly call 'delete' to free the memory immediately
		// delete &(it->second);
	}
}


void ResourceManager::deleteSound(const std::string& filename) {
	auto it = sounds.find(filename);
	if (it != sounds.end()) {
		sounds.erase(it);
		// Optional: The sound buffer will be automatically deleted when it goes out of scope
		// But, you can explicitly call 'delete' to free the memory immediately
		// delete &(it->second);
	}
}


void ResourceManager::deleteFont(const std::string& filename) {
	auto it = fonts.find(filename);
	if (it != fonts.end()) {
		fonts.erase(it);
		// Optional: The font will be automatically deleted when it goes out of scope
		// But, you can explicitly call 'delete' to free the memory immediately
		// delete &(it->second);
	}
}




class TextInput {
public:
	char lastChar;
	int inputType = -1;

	TextInput() {
		lastChar = ' ';
		inputType = -1;
	}

	void reset() {
		lastChar = ' ';
		inputType = -1;
	}

	void setWithUnicode(int code) {
		if (code < 128) {
			if (code > 31) {
				lastChar = (static_cast<char>(code));
				inputType = 0;
			}
			else if (code == 3 ||code == 8) {
				inputType = 2;
			}
		}
	}

	void setEnter() {
		inputType = 1;
	}

};

class Input {
public:
	bool keyboardState[sf::Keyboard::KeyCount][3];
	bool mouseState[sf::Mouse::ButtonCount][3];
	float mouseScroll = 0;

	sf::Vector2f mousePos;

	char lastChar;
	int inputType = -1;

	Input() {
		mousePos.x = 0;
		mousePos.y = 0;

		mouseScroll = 0;

		for (int i = 0; i < sf::Keyboard::KeyCount; i++) {
			keyboardState[i][0] = false;
			keyboardState[i][1] = false;
			keyboardState[i][2] = false;
		}

		for (int i = 0; i < sf::Mouse::ButtonCount; i++) {
			mouseState[i][0] = false;
			mouseState[i][1] = false;
			mouseState[i][2] = false;
		}
	}

	void setMousePos(sf::Vector2f pos) {
		mousePos = pos;
	}

	sf::Vector2f getMousePos() {
		return mousePos;
	}

	void update() {
		mouseScroll = 0;

		for (int i = 0; i < sf::Keyboard::KeyCount; i++) {
			bool keyState = sf::Keyboard::isKeyPressed((sf::Keyboard::Key)i);

			if (!keyboardState[i][0] && keyState) {
				keyboardState[i][1] = true;
			}
			else {
				keyboardState[i][1] = false;
			}

			if (keyboardState[i][0] && !keyState) {
				keyboardState[i][2] = true;
			}
			else {
				keyboardState[i][2] = false;
			}

			keyboardState[i][0] = keyState;

		}

		for (int i = 0; i < sf::Mouse::ButtonCount; i++) {

			bool buttonState = sf::Mouse::isButtonPressed((sf::Mouse::Button)i);

			if (!mouseState[i][0] && buttonState) {
				mouseState[i][1] = true;
			}
			else {
				mouseState[i][1] = false;
			}

			if (mouseState[i][0] && !buttonState) {
				mouseState[i][2] = true;
			}
			else {
				mouseState[i][2] = false;
			}

			mouseState[i][0] = buttonState;

		}
	}

	void reset() {
		mouseScroll = 0;

		for (int i = 0; i < sf::Keyboard::KeyCount; i++) {
			keyboardState[i][0] = false;
			keyboardState[i][1] = false;
			keyboardState[i][2] = false;
		}

		for (int i = 0; i < sf::Mouse::ButtonCount; i++) {
			mouseState[i][0] = false;
			mouseState[i][1] = false;
			mouseState[i][2] = false;
		}
	}


};

#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <thread>
#include <cmath>

#define SFML_STATIC

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

using namespace std;

#define println(x) (std::cout << x << std::endl)

const int initWindowWidth = 800;
const int initWindowHeight = 600;

#include "Math2.h"
float clamp(float val, float min, float max) {
    return constrain(val, min, max);
}
float normalizeValue(float val, float min, float max) {
    return clamp((val - min) / (max - min), 0, 1);
}

#include "inputManager.h"
#include "GUI.h"
#include "gameHelper.h"

#include "serverMain.h"
#include "clientMain.h"





class ClientServerScreen {
public:
    Button clientButton;
    Button serverButton;
 

    ClientServerScreen() {

        float xCenter = initWindowWidth / 2;
        float yCenter = initWindowHeight / 2;

        float buttonWid = initWindowWidth / 3;
        float buttonHei = 50;


        clientButton.init(xCenter - buttonWid/2, yCenter - buttonHei/2 - buttonHei, buttonWid, buttonHei);
        clientButton.setFont(basicFont);
        clientButton.label = "Client";
        clientButton.boxColor = sf::Color::Green;

        serverButton.init(xCenter - buttonWid / 2, yCenter - buttonHei/2 + buttonHei, buttonWid, buttonHei);
        serverButton.setFont(basicFont);
        serverButton.label = "Server";
        serverButton.boxColor = sf::Color::Red;
    }

    void update(Input input, TextInput textInput) {
        clientButton.update(input);
        serverButton.update(input);
    }

    void draw(sf::RenderWindow& window) {
        clientButton.draw(window);
        serverButton.draw(window);
    }
};





int main()
{

    basicFont.loadFromFile("FSEX300.ttf");

    sf::RenderWindow window(sf::VideoMode(initWindowWidth, initWindowHeight), "The Lobby");
    Input input;
    TextInput textInput;

    ClientGame clientGame;

    int connectionType = -1;


    ClientServerScreen clientServerScreen;

    /// Window Loop
    while (window.isOpen())
    {

        bool windowFocus = true;

        textInput.reset();

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::GainedFocus) {
                println("GainedFocus");
                windowFocus = true;
            }
            else if (event.type == sf::Event::LostFocus) {
                println("LostFocus");
                windowFocus = false;
            }
            else if (event.type == sf::Event::MouseMoved) {
                input.setMousePos(sf::Vector2f(event.mouseMove.x, event.mouseMove.y));
            }
            // Pegando o texto
            else if (event.type == sf::Event::TextEntered) {
                if (windowFocus) {
                    textInput.setWithUnicode(event.text.unicode);
                }
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Enter) {
                    if (windowFocus) {
                        textInput.setEnter();
                    }
                }
            }
            //
        }




        /// Background
        window.clear(sf::Color(255, 255, 255));




        if (windowFocus) {
            input.update();
        }
        else {
            input.reset();
        }







        if (connectionType == 0) {
            window.close();
        }
        else if (connectionType == 1) {
            clientGame.run(window, input, textInput);
            clientGame.draw(window);
        }
        else {
            clientServerScreen.update(input, textInput);
            clientServerScreen.draw(window);

            if (clientServerScreen.clientButton.clicked) {
                connectionType = 1;
                clientGame.loadGame();
                //println("Clientela " <<  connectionType);
            }
            else if (clientServerScreen.serverButton.clicked) {
                connectionType = 0;
               // println("Servente " << connectionType);
            }

        }

        window.display();
    }

    if (connectionType == 0) {
        serverMain();
    }

    return 0;
}

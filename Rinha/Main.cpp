#define SFML_STATIC
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <thread>
#include <cmath>

using namespace std;

#define PI 3.14159

#define println(x) (std::cout << x << std::endl)

int sign(float num){
    if(num > 0){
        return 1;
    } else if(num < 0){
        return -1;
    }
    return 0;
}

float clamp(float val, float min, float max){
    if(val > max){
        return max;
    } else if(val < min){
        return min;
    }

    return val;
}

float normalizeValue(float val, float min, float max){
    return clamp((val-min)/(max-min), 0, 1);
}


const int initWindowWidth = 800;
const int initWindowHeight = 600;


#include "gameHelper.h"

#include "serverMain.h"
#include "clientMain.h"








int main()
{



    char connectionType;
    println("DIGITE (s) PARA SERVER OU (c) PARA CLIENTE");
    cin >> connectionType;




    if(connectionType == 's'){

        serverMain();
    } else {

        clientMain();
    }

    return 0;
}

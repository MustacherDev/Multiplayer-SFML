#pragma once

#include <math.h>

#define Gravity 9.81
const long double PI  = 3.141592653589793;
#define EULER 2.71828



// Retorna o sinal de um numero
int sign(float val) {
	if (val > 0) {
		return 1;
	}
	else if (val < 0) {
		return -1;
	}
	return 0;
}


// Maximo Minimo e Constrain que e os dois juntos
float maximum(float val, float val2) {
	if (val > val2) {
		return val;
	}
	return val2;
}

float minimum(float val, float val2) {
	if (val < val2) {
		return val;
	}
	return val2;
}


float constrain(float val, float min, float max) {
	return maximum(minimum(val, max), min);
}


float normalize(float val, float valMin, float valMax, float outMin, float outMax) {
	float perc = (val - valMin) / (valMax - valMin);
	return outMin + (outMax - outMin) * perc;
}


// Obsolete
float ruleOfThree(float val, float valMin, float valMax, float outMin, float outMax) {
	return normalize(val, valMin, valMax, outMin, outMax);
}
float ruleOfThree(float val, float valMax, float val2Max) {
	return (val / valMax) * val2Max;
}

// Random
int randInt(int max) {
	if (max < 0) {
		return -(rand() % (-max));
	}

	if (max == 0) {
		return 0;
	}

	return rand() % max;
}

int randIntRange(int min, int max) {

	
	if (min > max) {
		return max + randInt(min - max);
	}
	return min + randInt(max-min);
}

float randFloat(float max) {	
	return max * rand() / (RAND_MAX + 1.);
}

float randFloatRange(float min, float max) {
	return min + randFloat(max - min);
}


// Calculates Hypotenuse
float pitagoras(float cateto1, float cateto2) {
	return sqrt(pow(cateto1, 2) + pow(cateto2, 2));
}

// Radians to Degrees
float rad2Deg(float ang) {
	return (ang * 180) / PI;
}

// Degrees to Radians
float deg2Rad(float ang) {
	return (ang * PI) / 180;
}

// Obsolete
float toRadiAnus(float angle) {
	return deg2Rad(angle);
}



// Text Utils
std::string fixedDigits(int number, int digits){
	std::string str = std::to_string(number);

	while (str.size() < digits) {
		str.insert(str.begin(), '0');
	}

	while (str.size() > digits) {
		str.erase(str.begin());
	}
	
	return str;

}





/*
void printIntVec(std::vector<int> vec) {
	std::cout << "[ ";
	for (int i = 0; i < vec.size(); i++) {
		std::cout << vec[i];
		if (i == vec.size() - 1) {
			std::cout << " ]";

		}
		else {
			std::cout << ", ";
		}
	}
}

std::string streamIntVec(std::vector<int> vec) {
	std::stringstream a;
	a << "[ ";
	for (int i = 0; i < vec.size(); i++) {
		a << vec[i];
		if (i == vec.size() - 1) {


		}
		else {
			a << ", ";
		}
	}
	a << " ]";

	return a.str();
}
*/

#include "Die.h"

#include <random>

Die::Die() {
	this->faces = "";
}

Die::Die(std::string faces) {
	this->faces = faces;
}

char Die::roll() {
	std::random_device rd;
	std::mt19937 gen(rd());

	std::shuffle(this->faces.begin(), this->faces.end(), gen);

	return this->getFace();
}

char Die::getFace() {
	return this->faces.size() > 0 ? this->faces[0] : '\0';
}
#pragma once

#include <string>

class Die {
public:
	Die();

	Die(std::string faces);

	char roll();

	char getFace();

private:
	std::string faces;
};


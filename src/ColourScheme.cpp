#include <algorithm>

#include "./headers/ColourScheme.h"
#include "./headers/RandomNumberGenerators.h"

static std::vector<ColourScheme> colourSchemes = std::vector<ColourScheme>();

std::vector<int> ColourScheme::generateColour(int s) {
	std::vector<int> newCol = std::vector<int>(4, 255);

	if (s == -1) return newCol;

	ColourScheme& CS = colourSchemes[s];

	newCol[0] = RandomNumberGenerators::rUnifInt(CS.rMin, CS.rMax);
	newCol[1] = RandomNumberGenerators::rUnifInt(CS.gMin, CS.gMax);
	newCol[2] = RandomNumberGenerators::rUnifInt(CS.bMin, CS.bMax);

	newCol[3] = 255;

	return newCol;
}

ColourScheme::ColourScheme(int id) {
	this->id = id;
}

void ColourScheme::addScheme(ColourScheme T) {

	colourSchemes.push_back(T);

	std::sort(colourSchemes.begin(), colourSchemes.end(), [](const ColourScheme& lhs, const ColourScheme& rhs) {
		return lhs.id < rhs.id;
		});

}
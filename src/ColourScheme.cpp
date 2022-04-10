#include <algorithm>

#include "./headers/ColourScheme.h"
#include "./headers/RandomNumberGenerators.h"

static std::vector<ColourScheme> colourSchemes = std::vector<ColourScheme>();

/**
 * @brief Generate a new colour from the colour scheme with the provided ID
 * 
 * @param s ID of colour scheme to generate from
 * @return std::vector<int> Vector of integer colour values, [R,G,B,A]
 */
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

/**
 * @brief Construct a new ColourScheme object
 * 
 * @param id ID to assign new colour scheme
 */
ColourScheme::ColourScheme(int id) {
	this->id = id;
}

/**
 * @brief Add the given colour scheme to the list of schemes
 * 
 * @param T Colour scheme to add
 */
void ColourScheme::addScheme(ColourScheme T) {

	colourSchemes.push_back(T);

	std::sort(colourSchemes.begin(), colourSchemes.end(), [](const ColourScheme& lhs, const ColourScheme& rhs) {
		return lhs.id < rhs.id;
		});

}
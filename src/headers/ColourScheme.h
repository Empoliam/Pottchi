#pragma once

#include <vector>

class ColourScheme {

public:

	int id;

	int rMin = 255;
	int rMax = 255;

	int gMin = 255;
	int gMax = 255;

	int bMin = 255;
	int bMax = 255;

	static std::vector<int> generateColour(int s);

	ColourScheme(int id);
	static void addScheme(ColourScheme T);

};

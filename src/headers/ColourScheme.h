#pragma once

#include <vector>

class ColourScheme {

public:

	int id;

	int rMin = 0;
	int rMax = 255;

	int gMin = 0;
	int gMax = 255;

	int bMin = 0;
	int bMax = 255;

	static std::vector<int> generateColour(int s);

	ColourScheme(int id);
	static void addScheme(ColourScheme T);

};

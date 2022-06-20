#pragma once

#include <vector>

class CellType {

public:

	CellType(int id);
		
	int id;

	std::vector<double> J;

	bool doesDivide = false;
	bool isStatic = false;
	bool ignoreVolume = false;
	bool countable = true;	

	double divideMean = 0;
	double divideSD = 0;
	int divideType = 0;
	int divMinVolume = 0;
	double divMinRatio = 0.0;


	int colourScheme = -1;
		
	static void addType(CellType T);
	static CellType& getType(int t);

};
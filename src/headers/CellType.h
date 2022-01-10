#pragma once

#include <vector>

class CellType {

public:

	CellType(int id);
		
	int id = -1;

	std::vector<double> J;

	bool doesDivide = false;
	bool isStatic = false;
	bool ignoreVolume = false;
	bool ignoreSurface = false;

	double divideMean = 0;
	double divideSD = 0;
	int divideType = 0;
	int divMinVolume = 0;
		
	static void addType(CellType T);
	static CellType& getType(int t);

};
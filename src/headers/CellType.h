#pragma once

#include <vector>

class CellType {

public:

	CellType(int id) {
		this->id = id;
	};

	std::vector<double> J;
	int id = -1;

	bool doesDivide = false;
	bool isStatic = false;
	bool ignoreVolume = false;
	bool ignoreSurface = false;

	double divideMean = 0;
	double divideSD = 0;
	int divideType = 0;

	static void addType(CellType T);

};
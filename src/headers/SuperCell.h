#pragma once

#include "CellTypes.h"
#include <vector>

class SuperCell {

public:
	
	static int makeNewSuperCell(CELL_TYPE type, int gen, int targetV);

	static int getID(int i);

	static int getGeneration(int i);
	static void increaseGeneration(int i);
	static void setGeneration(int i, int gen);

	static int getTargetVolume(int i);
	static void changeVolume(int i, int delta);
	static int getVolume(int i);
	
	static CELL_TYPE getCellType(int i);

	static int getCounter();

	static void setColour(int i, int r, int g, int b, int a);
	static std::vector<int> getColour(int i);

	

private:

	int ID;
	int generation;
	int targetVolume;
	int volume;
	
	CELL_TYPE type;

	SuperCell(CELL_TYPE type, int generation, int targetVolume);

	std::vector<int> colour = std::vector<int>(4,0);

};


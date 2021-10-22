#pragma once

#include "CellTypes.h"

class SuperCell {

public:
	
	static int makeNewSuperCell(CELL_TYPE type, int gen, int targetV);

	static int getID(int i);

	static int getGeneration(int i);
	static void increaseGeneration(int i);
	static void setGeneration(int i, int gen);

	static int getTargetVolume(int i);
	static CELL_TYPE getCellType(int i);

	static int getCounter();

private:

	int ID;
	int generation;
	int targetVolume;
	
	CELL_TYPE type;

	SuperCell(CELL_TYPE type, int generation, int targetVolume);

};


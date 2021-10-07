#pragma once

#include "Cell.h"
#include <vector>

class CellGrid {

public:

	virtual Cell& getCell(int row, int col) = 0;
	virtual std::vector<Cell*> getNeighbours(int row, int col) = 0;

};
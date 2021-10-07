#pragma once

#include <string>
#include "CellTypes.h"

class Cell {

public:

	Cell(CELL_TYPE type);
	Cell();

	CELL_TYPE getType() const;
	void setType(CELL_TYPE type);
	std::string toString() const;

private:

	CELL_TYPE type;

};


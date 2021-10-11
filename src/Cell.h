#pragma once

#include <string>
#include "CellTypes.h"
#include "Vector2D.h"

class Cell {

public:

	Cell(CELL_TYPE type, int generation);
	Cell(CELL_TYPE type);
	Cell();

	Cell(const Cell& c1) {
		generation = c1.generation;
		type = c1.type;
	}

	CELL_TYPE getType() const;
	void setType(CELL_TYPE type);

	int getGeneration()const;
	void setGeneration(int g);
	void increaseGeneration();

	std::string toString() const;

private:

	CELL_TYPE type;
	int generation;

};


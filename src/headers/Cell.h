#pragma once

#include <string>
#include "../headers/CellTypes.h"
#include "../headers/Vector2D.h"
#include "SuperCell.h"

class Cell {

public:

	Cell();
	Cell(int superCell);
	Cell(CELL_TYPE t, int targetVolume);

	Cell(const Cell& c1) {
		this->superCell = c1.superCell;
	};

	int getSuperCell() const;
	void setSuperCell(int i);

	CELL_TYPE getType() const;

	int getGeneration()const;
	void increaseGeneration();
	void setGeneration(int gen);

	int getTargetVolume() const;

	char toChar() const;

private:

	int superCell;

};


#pragma once

#include <string>
#include "../headers/CellTypes.h"
#include "../headers/Vector2D.h"
#include "SuperCell.h"

class Cell {

public:

	Cell();
	Cell(int superCell);

	Cell(const Cell& c1) {
		this->superCell = c1.superCell;
	};

	int getSuperCell() const;
	void setSuperCell(int i);

	CELL_TYPE getType() const;
	void setType(CELL_TYPE t);

	int getGeneration()const;
	void increaseGeneration();
	void setGeneration(int gen);

	void setTargetVolume(int target);
	int getTargetVolume() const;
	int getVolume() const;

	void setTargetSurface(int target);
	int getTargeSurface() const;
	int getSurface() const;
	void increaseSurface(int delta);

	std::vector<int> getColour() const;
	void generateNewColour();

	int getMCS();
	void setMCS(int i);
	int getNextDiv();
	void setNextDiv(int i);

private:

	int superCell;

};


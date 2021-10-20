#pragma once

#include "Cell.h"
#include <vector>

class CellGrid {

public:


	virtual Cell& getCell(int row, int col) = 0;

	virtual std::vector<Cell*> getNeighbours(int row, int col) = 0;
	virtual std::vector<Cell*> getNeighbours(int row, int col, CELL_TYPE t) = 0;
	virtual std::vector<Cell*> getNeighbours(int row, int col, std::vector<std::vector<Cell>>& grid) = 0;
	virtual std::vector<Cell*> getNeighbours(int row, int col, std::vector<std::vector<Cell>>& grid, CELL_TYPE t) = 0;

	virtual std::vector<Vector2D<int>> getNeighboursCoords(int row, int col) = 0;
	virtual std::vector<Vector2D<int>> getNeighboursCoords(int row, int col, CELL_TYPE t) = 0;
	virtual std::vector<Vector2D<int>> getNeighboursCoords(int row, int col, std::vector<std::vector<Cell>>& grid, CELL_TYPE t) = 0;

	virtual int divideCell(int x, int y) = 0;
	virtual int moveCell(int x, int y) = 0;

	virtual void clearCell(int x, int y) = 0;

	virtual int printGrid() = 0;

	virtual float getHamiltonian(std::vector<std::vector<Cell>>& grid) = 0;

};
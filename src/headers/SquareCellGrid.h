#pragma once

#include "CellGrid.h"


class SquareCellGrid : public CellGrid {

public:
	int boundaryWidth;
	int boundaryHeight;
	int interiorWidth;
	int interiorHeight;

	SquareCellGrid(int w, int h);

	Cell& getCell(int row, int col);
	std::vector<Cell*> getNeighbours(int row, int col);
	std::vector<Cell*> getNeighbours(int row, int col, CELL_TYPE t);


	std::vector<Vector2D<int>> getNeighboursCoords(int row, int col);
	std::vector<Vector2D<int>> getNeighboursCoords(int row, int col, CELL_TYPE t);

	int divideCell(int x, int y);
	int moveCell(int x, int y);

	void clearCell(int x, int y);

	int printGrid();

protected:
	std::vector<std::vector<Cell>> internalGrid;
};
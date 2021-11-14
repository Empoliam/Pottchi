#pragma once

#include "Cell.h"
#include "SDL2/SDL.h"

class SquareCellGrid {

public:
	int boundaryWidth;
	int boundaryHeight;
	int interiorWidth;
	int interiorHeight;

	SquareCellGrid(int w, int h);

	Cell& getCell(int row, int col);

	void setCell(int row, int col, int superCell);

	std::vector<Cell*> getNeighbours(int row, int col);
	std::vector<Cell*> getNeighbours(int row, int col, CELL_TYPE t);
	
	std::vector<Cell*> getNeighbours(int row, int col, std::vector<std::vector<Cell>>& grid);
	std::vector<Cell*> getNeighbours(int row, int col, std::vector<std::vector<Cell>>& grid, CELL_TYPE t);


	std::vector<Vector2D<int>> getNeighboursCoords(int row, int col);
	std::vector<Vector2D<int>> getNeighboursCoords(int row, int col, CELL_TYPE t);	
	
	std::vector<Vector2D<int>> getNeighboursCoords(int row, int col, std::vector<std::vector<Cell>>& grid, CELL_TYPE t);

	int divideCell(int c);
	int divideCellRandomAxis(int c);
	int cleaveCell(int c);

	int moveCell(int x, int y);

	float getAdhesionDelta(int sourceX, int sourceY, int destX, int destY);
	float getVolumeDelta(int sourceX, int sourceY, int destX, int destY);

	void fullTextureRefresh();
	std::vector<unsigned char> getPixels();

protected:
	std::vector<std::vector<Cell>> internalGrid;
	std::vector<unsigned char> pixels;

};
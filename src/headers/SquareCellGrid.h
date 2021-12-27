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
	
	std::vector<Vector2D<int>> getNeighboursCoords(int row, int col);
	std::vector<Vector2D<int>> getNeighboursCoords(int row, int col, CELL_TYPE t);

	int divideCell(int c);
	int divideCellRandomAxis(int c);
	int divideCellShortAxis(int c);
	int cleaveCell(int c);

	int moveCell(int x, int y);

	int calcSubCellPerimeter(int x, int y);

	void fullPerimeterRefresh();

	float getAdhesionDelta(int sourceX, int sourceY, int destX, int destY);
	float getVolumeDelta(int sourceX, int sourceY, int destX, int destY);

	void fullTextureRefresh();
	void localTextureRefresh(int x, int y);
	std::vector<unsigned char> getPixels();

protected:
	std::vector<std::vector<Cell>> internalGrid;
	std::vector<unsigned char> pixels;

	float calculateRawImageMoment(std::vector<Vector2D<int>> data, int iO, int jO);

};
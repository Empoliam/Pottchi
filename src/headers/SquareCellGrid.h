#pragma once

#include "Vector2D.h"
#include <SDL2/SDL.h>

#include <vector>

class SquareCellGrid {

public:
	int boundaryWidth;
	int boundaryHeight;
	int interiorWidth;
	int interiorHeight;

	double BOLTZ_TEMP;
	double OMEGA;
	double LAMBDA;

	SquareCellGrid(int w, int h, int boundarySC, int spaceSC);

	int getCell(int row, int col);

	void setCell(int row, int col, int superCell);
	
	std::vector<int> getNeighboursSuperCells(int row, int col);
	std::vector<int> getNeighboursTypes(int row, int col);
	std::vector<Vector2D<int>> getNeighboursCoords(int row, int col);
	std::vector<Vector2D<int>> getNeighboursCoords(int row, int col, int type);

	int divideCell(int c);
	int divideCellRandomAxis(int c);
	int divideCellShortAxis(int c);
	int cleaveCell(int c);

	int moveCell(int x, int y);

	double getAdhesionDelta(int sourceX, int sourceY, int destX, int destY);
	double getVolumeDelta(int sourceX, int sourceY, int destX, int destY);

	void fullTextureRefresh();
	void localTextureRefresh(int x, int y);
	std::vector<Uint8> getPixels();

protected:

	double calculateRawImageMoment(std::vector<Vector2D<int>> data, int iO, int jO);

};
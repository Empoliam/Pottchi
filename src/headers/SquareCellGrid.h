#pragma once

#include "Vector2D.h"
#include "SDL2/SDL.h"

#include <vector>

class SquareCellGrid {

public:
	int boundaryWidth;
	int boundaryHeight;
	int interiorWidth;
	int interiorHeight;

	//Always overriden, do not change
	double BOLTZ_TEMP;
	double OMEGA;
	double LAMBDA;
	double SIGMA;

	SquareCellGrid(int w, int h);

	int getCell(int row, int col);

	void setCell(int row, int col, int superCell);
	
	std::vector<Vector2D<int>> getNeighboursCoords(int row, int col);
	std::vector<Vector2D<int>> getNeighboursCoords(int row, int col, int type);

	int divideCell(int c);
	int divideCellRandomAxis(int c);
	int divideCellShortAxis(int c);
	int cleaveCell(int c);

	int moveCell(int x, int y);

	int calcSubCellPerimeter(int x, int y);
	int calcSubCellPerimeter(int x, int y, int activeSuper);

	void fullPerimeterRefresh();

	double getAdhesionDelta(int sourceX, int sourceY, int destX, int destY);
	double getVolumeDelta(int sourceX, int sourceY, int destX, int destY);
	double getSurfaceDelta(int sourceX, int sourceY, int destX, int destY);

	void fullTextureRefresh();
	void localTextureRefresh(int x, int y);
	std::vector<unsigned char> getPixels();

protected:

	std::vector<std::vector<int>> internalGrid;
	std::vector<unsigned char> pixels;

	double calculateRawImageMoment(std::vector<Vector2D<int>> data, int iO, int jO);

};
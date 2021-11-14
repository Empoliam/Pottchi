#include "./headers/SquareCellGrid.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <random>
#include <math.h>

#include "./headers/RandomNumberGenerators.h"

using namespace std;

const float  PI_F = 3.14159265358979f;

const float BOLTZ_TEMP = 10.0f;
const float LAMBDA = 5.0f;
const float J[4][4] = {
	{1000000.0f, 1000000.0f,	1000000.0f, 1000000.0f},
	{1000000.0f, 0.0f,			0.0f,		0.0f},
	{1000000.0f, 0.0f,			0.0f,		50.0f},
	{1000000.0f, 50.0f,			50.0f,		30.0f}
};

SquareCellGrid::SquareCellGrid(int w, int h) : internalGrid(w + 2, std::vector<Cell>(h + 2)), pixels((w+2) * (h+2) * 4, 0) {

	interiorWidth = w;
	interiorHeight = h;
	boundaryWidth = w + 2;
	boundaryHeight = h + 2;

	for (int x = 0; x < boundaryWidth; x++) {
		internalGrid[x][0] = Cell((int)CELL_TYPE::BOUNDARY);
		internalGrid[x][boundaryHeight - 1] = Cell((int)CELL_TYPE::BOUNDARY);
	};

	for (int y = 0; y < boundaryHeight; y++) {
		internalGrid[0][y] = Cell((int)CELL_TYPE::BOUNDARY);
		internalGrid[boundaryWidth - 1][y] = Cell((int)CELL_TYPE::BOUNDARY);
	};
	
	SuperCell::setVolume(1, interiorWidth * interiorHeight);
	SuperCell::setVolume(0, (boundaryWidth*boundaryHeight) - (interiorWidth * interiorHeight));

}

vector<Cell*> SquareCellGrid::getNeighbours(int row, int col, std::vector<std::vector<Cell>>& grid)
{

	vector<Cell*> neighbours;

	neighbours.push_back(&(grid[row - 1][col - 1]));
	neighbours.push_back(&(grid[row][col - 1]));
	neighbours.push_back(&(grid[row + 1][col - 1]));
	neighbours.push_back(&(grid[row - 1][col]));
	neighbours.push_back(&(grid[row + 1][col]));
	neighbours.push_back(&(grid[row - 1][col + 1]));
	neighbours.push_back(&(grid[row][col + 1]));
	neighbours.push_back(&(grid[row + 1][col + 1]));

	return neighbours;

}

vector<Cell*> SquareCellGrid::getNeighbours(int row, int col) {
	return getNeighbours(row, col, internalGrid);
}

std::vector<Cell*> SquareCellGrid::getNeighbours(int row, int col, CELL_TYPE t) {
	return getNeighbours(row, col, internalGrid, t);
}

vector<Cell*> SquareCellGrid::getNeighbours(int row, int col, std::vector<std::vector<Cell>>& grid, CELL_TYPE t) {

	vector<Cell*> neighbours = getNeighbours(row, col, grid);

	neighbours.erase(std::remove_if(
		neighbours.begin(),
		neighbours.end(),
		[t](const Cell* c) { return c->getType() != t; })
		, neighbours.end()
	);

	return neighbours;

}

vector<Vector2D<int>> SquareCellGrid::getNeighboursCoords(int row, int col)
{
	vector<Vector2D<int>> neighbours;

	neighbours.reserve(8);
	neighbours.push_back(Vector2D<int>(row - 1, col - 1));
	neighbours.push_back(Vector2D<int>(row, col - 1));
	neighbours.push_back(Vector2D<int>(row + 1, col - 1));
	neighbours.push_back(Vector2D<int>(row - 1, col));
	neighbours.push_back(Vector2D<int>(row + 1, col));
	neighbours.push_back(Vector2D<int>(row - 1, col + 1));
	neighbours.push_back(Vector2D<int>(row, col + 1));
	neighbours.push_back(Vector2D<int>(row + 1, col + 1));

	return neighbours;
}

std::vector<Vector2D<int>> SquareCellGrid::getNeighboursCoords(int row, int col, CELL_TYPE t) {
	return getNeighboursCoords(row, col, internalGrid, t);
}

vector<Vector2D<int>> SquareCellGrid::getNeighboursCoords(int row, int col, std::vector<std::vector<Cell>>& grid, CELL_TYPE t)
{
	vector<Vector2D<int>> neighbours;

	neighbours.reserve(8);
	if (grid[row - 1][col - 1].getType() == t) neighbours.push_back(Vector2D<int>(row - 1, col - 1));
	if (grid[row][col - 1].getType() == t) neighbours.push_back(Vector2D<int>(row, col - 1));
	if (grid[row + 1][col - 1].getType() == t) neighbours.push_back(Vector2D<int>(row + 1, col - 1));
	if (grid[row - 1][col].getType() == t) neighbours.push_back(Vector2D<int>(row - 1, col));
	if (grid[row + 1][col].getType() == t) neighbours.push_back(Vector2D<int>(row + 1, col));
	if (grid[row - 1][col + 1].getType() == t) neighbours.push_back(Vector2D<int>(row - 1, col + 1));
	if (grid[row][col + 1].getType() == t) neighbours.push_back(Vector2D<int>(row, col + 1));
	if (grid[row + 1][col + 1].getType() == t) neighbours.push_back(Vector2D<int>(row + 1, col + 1));

	return neighbours;
}

int SquareCellGrid::divideCell(int c) {

	int minX = interiorWidth;
	int minY = interiorHeight;
	int maxX = 1;
	int maxY = 1;

	vector<Vector2D<int>> cellList;
	vector<Vector2D<int>> newList;

	for (int X = 1; X <= interiorWidth; X++) {
		for (int Y = 1; Y <= interiorHeight; Y++) {

			if (internalGrid[X][Y].getSuperCell() == c) {

				cellList.push_back(Vector2D<int>(X, Y));

				if (X < minX) minX = X;
				if (X > maxX) maxX = X;
				if (Y < minY) minY = Y;
				if (Y > maxY) maxY = Y;

			}

		}
	}

	if (cellList.size() <= 1) {
		return 1;
	}

	int splitAxis;
	int midPoint;

	if ((maxX - minX) > (maxY - minY)) {
		splitAxis = 0;
		midPoint = (maxX + minX) / 2;
	}
	else {
		splitAxis = 1;
		midPoint = (maxY + minY) / 2;
	}

	for (unsigned int k = 0; k < cellList.size(); k++) {
		if (cellList[k][splitAxis] < midPoint) {
			newList.push_back(cellList[k]);
		}
	}

	SuperCell::increaseGeneration(c);
	int newSuperCell = SuperCell::makeNewSuperCell(SuperCell::getCellType(c), SuperCell::getGeneration(c), SuperCell::getTargetVolume(c));

	SuperCell::setMCS(c, 0);
	SuperCell::setMCS(newSuperCell, 0);

	for (unsigned int c = 0; c < newList.size(); c++) {
		Vector2D<int>& V = newList[c];
		setCell(V[0], V[1], newSuperCell);
	}

	return newSuperCell;

}

int SquareCellGrid::divideCellRandomAxis(int c) {

	int minX = interiorWidth;
	int minY = interiorHeight;
	int maxX = 1;
	int maxY = 1;

	vector<Vector2D<int>> cellList;
	vector<Vector2D<int>> newList;

	for (int X = 1; X <= interiorWidth; X++) {
		for (int Y = 1; Y <= interiorHeight; Y++) {

			if (internalGrid[X][Y].getSuperCell() == c) {

				cellList.push_back(Vector2D<int>(X, Y));

				if (X < minX) minX = X;
				if (X > maxX) maxX = X;
				if (Y < minY) minY = Y;
				if (Y > maxY) maxY = Y;

			}

		}
	}

	if (cellList.size() <= 1) {
		return -1;
	}

	int midX = (int)(0.5 * (minX + maxX));
	int midY = (int)(0.5 * (minY + maxY));

	int gradM = RandomNumberGenerators::rUnifInt(-89, 89);
	float grad = tan(gradM*PI_F/180.f);

	cout << gradM << endl;

	for (unsigned int k = 0; k < cellList.size(); k++) {
		if (cellList[k][1] > grad*(cellList[k][0]-midX)+midY) {
			newList.push_back(cellList[k]);
		}
	}

	SuperCell::increaseGeneration(c);
	int newSuperCell = SuperCell::makeNewSuperCell(SuperCell::getCellType(c), SuperCell::getGeneration(c), SuperCell::getTargetVolume(c));

	SuperCell::setMCS(c, 0);
	SuperCell::setMCS(newSuperCell, 0);

	for (unsigned int c = 0; c < newList.size(); c++) {
		Vector2D<int>& V = newList[c];
		setCell(V[0], V[1], newSuperCell);
	}

	return newSuperCell;

}

int SquareCellGrid::cleaveCell(int c) {

	int superCellA = c;
	int superCellB = divideCellRandomAxis(c);

	if (superCellB == -1) return -1;

	int newTargetVolume = SuperCell::getTargetVolume(c) / 2;

	SuperCell::setTargetVolume(superCellA, newTargetVolume);
	SuperCell::setTargetVolume(superCellB, newTargetVolume);

	return superCellB;
}

int SquareCellGrid::moveCell(int x, int y) {

	vector<Vector2D<int>> neighbours = getNeighboursCoords(x, y);

	int r = RandomNumberGenerators::rUnifInt(0, (int)neighbours.size() - 1);

	int targetX = neighbours[r][0];
	int targetY = neighbours[r][1];

	Cell& origin = internalGrid[x][y];
	Cell& swap = internalGrid[neighbours[r][0]][neighbours[r][1]];

	if (swap.getType() != CELL_TYPE::BOUNDARY &&
		swap.getSuperCell() != internalGrid[x][y].getSuperCell()) {

		float deltaH = getAdhesionDelta(x, y, targetX, targetY) + getVolumeDelta(x, y, targetX, targetY);

		if (deltaH <= 0 || (RandomNumberGenerators::rUnifProb() < exp(-deltaH / BOLTZ_TEMP))) {
			setCell(targetX, targetY, internalGrid[x][y].getSuperCell());
			return 1;
		}

	}

	return 0;

}

Cell& SquareCellGrid::getCell(int row, int col) {
	return internalGrid[row][col];
}

void SquareCellGrid::setCell(int x, int y, int superCell) {
	SuperCell::changeVolume(internalGrid[x][y].getSuperCell(), -1);
	internalGrid[x][y].setSuperCell(superCell);
	SuperCell::changeVolume(superCell, 1);
}

float SquareCellGrid::getAdhesionDelta(int sourceX, int sourceY, int destX, int destY) {

	Cell& source = internalGrid[sourceX][sourceY];
	Cell& dest = internalGrid[destX][destY];

	int sourceSuper = source.getSuperCell();
	int destSuper = dest.getSuperCell();

	float initH = 0.0f;
	float postH = 0.0f;

	vector<Cell*> neighbours = getNeighbours(destX, destY);
	for (int i = 0; i < 8; i++) {

		int nSuper = neighbours[i]->getSuperCell();

		initH += J[(int)neighbours[i]->getType()][(int)dest.getType()] * (nSuper != destSuper);
		postH += J[(int)neighbours[i]->getType()][(int)source.getType()] * (nSuper != sourceSuper);

	}

	return (postH - initH);
}

float SquareCellGrid::getVolumeDelta(int sourceX, int sourceY, int destX, int destY) {

	int destSuper = internalGrid[destX][destY].getSuperCell();

	//Prevent destruction of cells
	if (SuperCell::getVolume(destSuper) - 1 == 0) return 100000.0f;

	int sourceSuper = internalGrid[sourceX][sourceY].getSuperCell();

	int sourceVol = SuperCell::getVolume(sourceSuper);
	int destVol = SuperCell::getVolume(destSuper);

	int sourceTarget = SuperCell::getTargetVolume(sourceSuper);
	int destTarget = SuperCell::getTargetVolume(destSuper);

	float deltaH = 0.0f;

	if (sourceSuper == (int)CELL_TYPE::EMPTYSPACE) {

		deltaH = (float)pow(destVol - 1 - destTarget, 2)- (float)pow(destVol - destTarget, 2);

	}	else if (destSuper == (int)CELL_TYPE::EMPTYSPACE) {

		deltaH = (float)pow(sourceVol + 1 - sourceTarget, 2) - (float)pow(sourceVol - sourceTarget, 2);

	} else {

		deltaH = (float)pow(sourceVol + 1 - sourceTarget, 2)
			+ (float)pow(destVol - 1 - destTarget, 2)
			- (float)pow(sourceVol - sourceTarget, 2)
			- (float)pow(destVol - destTarget, 2);

	}



	deltaH *= LAMBDA;

	return deltaH;
}

void SquareCellGrid::fullTextureRefresh() {

	for (int x = 0; x < boundaryWidth; x++) {

		for (int y = 0; y < boundaryHeight; y++) {

			const unsigned pixOffset = (boundaryWidth * 4 * y) + x * 4;
			vector<int> colour = internalGrid[x][y].getColour();

			if (colour.size() == 0) {
				colour = { 0,0,0,0 };
			}

			pixels[pixOffset + 0] = (char) colour[0];
			pixels[pixOffset + 1] = (char) colour[1];
			pixels[pixOffset + 2] = (char) colour[2];
			pixels[pixOffset + 3] = SDL_ALPHA_OPAQUE;

		}

	}

}

std::vector<unsigned char> SquareCellGrid::getPixels() {

	return pixels;

}

#include "./headers/SquareCellGrid.h"

#include <iostream>
#include <vector>
#include <random>
#include <math.h>

#include "./headers/RandomNumberGenerators.h"

using namespace std;

const float BOLTZ_TEMP = 10.0f;
const float LAMBDA = 1.0f;
const float J[3][3] = {
	{10.0f, 10.0f, 10.0f},
	{10.0f, 10.0f, 10.0f},
	{10.0f, 10.0f, 10.0f}
};

SquareCellGrid::SquareCellGrid(int w, int h) : internalGrid(w + 2, std::vector<Cell>(h + 2)) {

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

int SquareCellGrid::divideCell(int x, int y) {

	vector<Vector2D<int>> neighbours = getNeighboursCoords(x, y);
	Cell& active = internalGrid[x][y];

	if (neighbours.size() != 0) {

		active.increaseGeneration();

		int r = RandomNumberGenerators::rUnifInt(0, neighbours.size() - 1);
		Vector2D<int>& target = neighbours[r];

		int s = SuperCell::makeNewSuperCell(active.getType(), active.getGeneration(), active.getTargetVolume());

		setCell(target[0], target[1], s);

	}

	return 0;

}


int SquareCellGrid::moveCell(int x, int y) {

	vector<Vector2D<int>> neighbours = getNeighboursCoords(x, y);

	int r = RandomNumberGenerators::rUnifInt(0, neighbours.size() - 1);

	int targetX = neighbours[r][0];
	int targetY = neighbours[r][1];

	Cell& origin = internalGrid[x][y];
	Cell& swap = internalGrid[neighbours[r][0]][neighbours[r][1]];

	if (swap.getType() != CELL_TYPE::BOUNDARY() &&
		swap.getSuperCell() != internalGrid[x][y].getSuperCell()) {

		float deltaH = getAdhesionDelta(x, y, targetX, targetY) + getVolumeDelta(x, y, targetX, targetY);

		if (deltaH <= 0 || (RandomNumberGenerators::rUnifProb() < exp(-deltaH / BOLTZ_TEMP))) {
			setCell(targetX, targetY, internalGrid[x][y].getSuperCell());
			return 1;
		}

	}

	return 0;

}


int SquareCellGrid::printGrid(SDL_Renderer* renderer, int pixelSize) {

	for (int y = 0; y < boundaryHeight; y++) {
		for (int x = 0; x < boundaryWidth; x++) {

			vector<int> colour = internalGrid[x][y].getColour();

			SDL_Rect rect = { x * pixelSize, y * pixelSize, pixelSize, pixelSize };

			SDL_SetRenderDrawColor(renderer, colour[0], colour[1], colour[2], colour[3]);
			SDL_RenderFillRect(renderer, &rect);

		}

	}

	SDL_RenderPresent(renderer);

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

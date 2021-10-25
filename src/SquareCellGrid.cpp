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
	{10000.0f, 10000.0f, 10000.0f},
	{10000.0f, 10000.0f, 10.0f},
	{10000.0f, 10.0f, 10.0f} 
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

	Cell& origin = internalGrid[x][y];
	Cell& swap = internalGrid[neighbours[r][0]][neighbours[r][1]];

	if (swap.getType() != CELL_TYPE::BOUNDARY() &&
		swap.getSuperCell() != internalGrid[x][y].getSuperCell()) {

		vector<vector<Cell>> newConfig = internalGrid;
		newConfig[neighbours[r][0]][neighbours[r][1]] = internalGrid[x][y];

		float currentEnergy = getHamiltonian(internalGrid);
		float newEnergy = getHamiltonian(newConfig);

		float dEnergy = newEnergy - currentEnergy;
		float probChange = exp(-dEnergy / BOLTZ_TEMP);

		if ((newEnergy <= currentEnergy) || (RandomNumberGenerators::rUnifProb() < exp(-dEnergy / BOLTZ_TEMP))) {
			
			SuperCell::changeVolume(origin.getSuperCell(), 1);
			SuperCell::changeVolume(swap.getSuperCell(), -1);

			internalGrid = newConfig;
						
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
	SuperCell::changeVolume(internalGrid[x][y].getSuperCell(),-1);
	internalGrid[x][y].setSuperCell(superCell);
	SuperCell::changeVolume(superCell, 1);
}

float SquareCellGrid::getHamiltonian(std::vector<std::vector<Cell>>& grid) {

	float energy = 0.0f;

	int xBounds = grid.size() - 2;
	int yBounds = grid[0].size() - 2;

	vector<int> volumes(SuperCell::getCounter());

	for (int x = 1; x <= xBounds; x++) {

		for (int y = 1; y <= yBounds; y++) {

			Cell& active = grid[x][y];
			vector<Cell*> neighbours = getNeighbours(x, y, grid);

			volumes[active.getSuperCell()]++;

			for (int i = 0; i < 8; i++) {

				if (neighbours[i]->getSuperCell() != active.getSuperCell()) {

					energy += J;


				}

			}

		}

	}

	for (unsigned int i = 2; i < volumes.size(); i++) {
		if (volumes[i] == 0) energy += 10000000;
		energy += LAMBDA * (int)pow(volumes[i] - SuperCell::getTargetVolume(i), 2);
	}

	return energy;

}

float SquareCellGrid::getAdhesionDelta(int sourceX, int sourceY, int destX, int destY) {

	Cell& source = internalGrid[sourceX][sourceY];
	Cell& dest = internalGrid[destX][destY];

	int sourceSuper = source.getSuperCell();
	int destSuper = source.getSuperCell();

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

	int sourceSuper = internalGrid[sourceX][sourceY].getSuperCell();
	int destSuper = internalGrid[destX][destY].getSuperCell();

	int sourceVol = SuperCell::getVolume(sourceSuper);
	int destVol = SuperCell::getVolume(destSuper);

	int sourceTarget = SuperCell::getTargetVolume(sourceSuper);
	int destTarget = SuperCell::getTargetVolume(destSuper);

	float deltaH = (float) pow(sourceVol + 1 - sourceTarget, 2)
		+ (float) pow(destVol - 1 - destTarget, 2)
		- (float) pow(sourceVol-sourceTarget, 2)
		- (float) pow(destVol - destTarget, 2);

	deltaH*=LAMBDA;

	return deltaH;
}

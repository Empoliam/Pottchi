#include "./headers/SquareCellGrid.h"

#include <iostream>
#include <vector>
#include <random>
#include <curses.h>

#include "./headers/RandomNumberGenerators.h"

using namespace std;

const float BOLTZ_TEMP = 1;
const float LAMBDA = 1;

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

	vector<Cell*> neighbours = getNeighbours(x, y, CELL_TYPE::EMPTYSPACE);
	Cell& active = internalGrid[x][y];

	if (neighbours.size() != 0) {

		active.increaseGeneration();

		int r = RandomNumberGenerators::rUnifInt(0, neighbours.size() - 1);

		*neighbours[r] = Cell(active.getType(), active.getTargetVolume());
		neighbours[r]->setGeneration(active.getGeneration());

	}

	return 0;

}


int SquareCellGrid::moveCell(int x, int y) {

	float currentEnergy = getHamiltonian(internalGrid);

	vector<Vector2D<int>> neighbours = getNeighboursCoords(x, y, CELL_TYPE::EMPTYSPACE);

	if (!neighbours.empty()) {

		int r = RandomNumberGenerators::rUnifInt(0, neighbours.size() - 1);

		vector<vector<Cell>> newConfig = internalGrid;
		newConfig[neighbours[r][0]][neighbours[r][1]] = internalGrid[x][y];
		newConfig[x][y].setSuperCell((int)CELL_TYPE::EMPTYSPACE);

		float newEnergy = getHamiltonian(newConfig);
		float dEnergy = newEnergy - currentEnergy;
		float probChange = exp(-dEnergy / BOLTZ_TEMP);

		if (newEnergy <= currentEnergy) {
			internalGrid = newConfig;
		}
		else if (RandomNumberGenerators::rUnifProb() < exp(-dEnergy / BOLTZ_TEMP)) {
			internalGrid = newConfig;
		}

		return 0;

	}

	return 1;

}


int SquareCellGrid::printGrid() {

	for (int y = 0; y < boundaryHeight; y++) {
		for (int x = 0; x < boundaryWidth; x++) {

			Cell c = internalGrid[x][y];

			move(y, x);
			attron(COLOR_PAIR(c.getGeneration()));
			addch(c.toChar());
			attroff(COLOR_PAIR(c.getGeneration()));

		}

	}

	refresh();

	return 0;
}


Cell& SquareCellGrid::getCell(int row, int col) {
	return internalGrid[row][col];
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

					if (neighbours[i]->getType() != active.getType()) {

						energy += 1.0f;

					}

				}

			}

		}

	}

	for (unsigned int i = 2; i < volumes.size(); i++) {
		energy += ((volumes[i] - SuperCell::getTargetVolume(i)) ^ 2);
	}

	return energy;

}
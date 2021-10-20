#include "./headers/SquareCellGrid.h"

#include <iostream>
#include <vector>
#include <random>
#include <curses.h>

#include "./headers/RandomNumberGenerators.h"

using namespace std;

SquareCellGrid::SquareCellGrid(int w, int h) : internalGrid(w + 2, std::vector<Cell>(h + 2)) {

	interiorWidth = w;
	interiorHeight = h;
	boundaryWidth = w + 2;
	boundaryHeight = h + 2;

	for (int x = 0; x < boundaryWidth; x++) {
		internalGrid[x][0].setType(CELL_TYPE::BOUNDARY);
		internalGrid[x][boundaryHeight - 1].setType(CELL_TYPE::BOUNDARY);
	};

	for (int y = 0; y < boundaryWidth; y++) {
		internalGrid[0][y].setType(CELL_TYPE::BOUNDARY);
		internalGrid[boundaryWidth - 1][y].setType(CELL_TYPE::BOUNDARY);
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
	return getNeighbours(row,col,internalGrid);
}

std::vector<Cell*> SquareCellGrid::getNeighbours(int row, int col, CELL_TYPE t) {
	return getNeighbours(row, col, internalGrid, t);
}

vector<Cell*> SquareCellGrid::getNeighbours(int row, int col, std::vector<std::vector<Cell>>& grid, CELL_TYPE t) {

	vector<Cell*> neighbours = getNeighbours(row, col, grid);

	neighbours.erase(std::remove_if(
		neighbours.begin(), 
		neighbours.end(), 
		[t](const Cell* c){ return c->getType() != t; })
		,neighbours.end()
	);

	return neighbours;

}

vector<Vector2D<int>> SquareCellGrid::getNeighboursCoords(int row, int col)
{
	vector<Vector2D<int>> neighbours;

	neighbours.reserve(8);
	neighbours.push_back(Vector2D<int>(row - 1,col - 1));
	neighbours.push_back(Vector2D<int>(row,col - 1));
	neighbours.push_back(Vector2D<int>(row + 1,col - 1));
	neighbours.push_back(Vector2D<int>(row - 1,col));
	neighbours.push_back(Vector2D<int>(row + 1,col));
	neighbours.push_back(Vector2D<int>(row - 1,col + 1));
	neighbours.push_back(Vector2D<int>(row,col + 1));
	neighbours.push_back(Vector2D<int>(row + 1,col + 1));

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

	if (neighbours.size() != 0) {

		internalGrid[x][y].increaseGeneration();

		int r = RandomNumberGenerators::rUnifInt(0, neighbours.size() - 1);

		*neighbours[r] = internalGrid[x][y];

	}

	return 0;

}

int SquareCellGrid::moveCell(int x, int y) {
	
	vector<Vector2D<int>> emptyNeighbours = getNeighboursCoords(x, y, CELL_TYPE::EMPTYSPACE);
	
	if (!emptyNeighbours.empty()) {
		
		vector<Vector2D<int>> validSquares;
		Vector2D<int> excludeCell(x,y);
		CELL_TYPE adhesiveType = internalGrid[x][y].getType();

		for (int i = 0; i < (int)emptyNeighbours.size(); i++) {

			const Vector2D<int>& v = emptyNeighbours[i];
			
			vector<Vector2D<int>> N = getNeighboursCoords(v[0], v[1], adhesiveType);

			if (!N.empty()) {
				for (int j = 0; j < (int) N.size(); j++) {
					if (N[j] != excludeCell) {
						validSquares.push_back(emptyNeighbours[i]);
					}
				}
			}

		}

		if (!validSquares.empty()) {

			int r = RandomNumberGenerators::rUnifInt(0, validSquares.size()-1);
			const Vector2D<int>& V = validSquares[r];
			
			internalGrid[V[0]][V[1]] = internalGrid[x][y];
			
			clearCell(x, y);

		}

		return 0;

	}
	else return 1;

}


//int SquareCellGrid::moveCell(int x, int y) {
//
//	float currentEnergy = getHamiltonian();
//
//}

void SquareCellGrid::clearCell(int x, int y)
{
	internalGrid[x][y] = Cell(CELL_TYPE::EMPTYSPACE,0);
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



float SquareCellGrid::getHamiltonian() {

	float energy = 0.0f;

	for (int x = 1; x <= interiorWidth; x++) {

		for (int y = 1; y <= interiorHeight; y++) {

			Cell& active = internalGrid[x][y];
			vector<Cell*> neighbours = getNeighbours(x, y);
			
			for (int i = 0; i < 8; i++) {

				if (neighbours[i]->getType() != active.getType()) {
					
					energy += 1.0f;

				}

			}

		}

	}

	return 0.0f;

}
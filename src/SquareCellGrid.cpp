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

vector<Cell*> SquareCellGrid::getNeighbours(int row, int col)
{

	vector<Cell*> neighbours;

	neighbours.push_back(&internalGrid[row - 1][col - 1]);
	neighbours.push_back(&internalGrid[row][col - 1]);
	neighbours.push_back(&internalGrid[row + 1][col - 1]);
	neighbours.push_back(&internalGrid[row - 1][col]);
	neighbours.push_back(&internalGrid[row + 1][col]);
	neighbours.push_back(&internalGrid[row - 1][col + 1]);
	neighbours.push_back(&internalGrid[row][col + 1]);
	neighbours.push_back(&internalGrid[row + 1][col + 1]);

	return neighbours;

}

vector<Cell*> SquareCellGrid::getNeighbours(int row, int col, CELL_TYPE t) {

	vector<Cell*> neighbours = getNeighbours(row, col);

	neighbours.erase(std::remove_if(
		neighbours.begin(), 
		neighbours.end(), 
		[t](const Cell* c){ return c->getType() != t; })
		,neighbours.end()
	);

	return neighbours;

}

std::vector<Vector2D<int>> SquareCellGrid::getNeighboursCoords(int row, int col)
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

std::vector<Vector2D<int>> SquareCellGrid::getNeighboursCoords(int row, int col, CELL_TYPE t)
{
	vector<Vector2D<int>> neighbours = getNeighboursCoords(row, col);

	neighbours.erase(std::remove_if(
		neighbours.begin(),
		neighbours.end(),
		[t, &internalGrid = internalGrid](const Vector2D<int> v) {
			Cell c = internalGrid[v[0]][v[1]];
			return (c.getType() != t); 
		})
		, neighbours.end()
	);

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
	
	vector<Vector2D<int>> neighbours = getNeighboursCoords(x, y, CELL_TYPE::EMPTYSPACE);
	
	if (!neighbours.empty()) {
		
		vector<Vector2D<int>> validSquares;

		for (int i = 0; i < (int)neighbours.size(); i++) {
			Vector2D<int>& v = neighbours[i];
			Cell* excludeN = &internalGrid[x][y];
			vector<Cell*> N = getNeighbours(v[0], v[0], excludeN->getType());

			bool valid = false;

			if (!N.empty()) {
				for (int j = 0; j < N.size(); j++) {
					if (N[j] != excludeN) {
						valid = true;
						break;
					}
				}
			}

			if (valid) {
				validSquares.push_back(neighbours[i]);
			}

		}

		return 0;

	}
	else return 1;

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


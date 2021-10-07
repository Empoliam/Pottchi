#include <iostream>
#include "Cell.h"
#include "SquareCellGrid.h"

const int width = 100;
const int height = 100;

using namespace std;

int main() {

	SquareCellGrid grid(width, height);

	grid.getCell(width / 2, height / 2).setType(CELL_TYPE::GENERIC);

	vector<Cell*> cList = grid.getNeighbours(width / 2, height / 2);

	for (uint8_t i = 0; i < cList.size(); i++) {
		cList[i]->setType(CELL_TYPE::GENERIC);
	}

	for (int y = 0; y < grid.boundaryHeight; y++) {
		for (int x = 0; x < grid.boundaryWidth; x++) {

			Cell c = grid.getCell(x, y);
			cout << c.toString();

		}

		cout << "\n";
	}

	return 0;
}
#include <iostream>
#include <random>

#include "Cell.h"
#include "SquareCellGrid.h"
#include "Vector2D.h"

#include "RandomNumberGenerators.h"

const int width = 40;
const int height = 40;

const float iMax = 1000;
const float pDiv = 0.01f;

using namespace std;

int main() {

	SquareCellGrid grid(width, height);

	grid.getCell(width / 2, height / 2).setType(CELL_TYPE::GENERIC);

	for (int i = 0; i < iMax; i++) {

		for (int x = 1; x <= grid.interiorWidth; x++) {

			for (int y = 1; y < grid.interiorHeight; y++) {

				Cell c = grid.getCell(x, y);
				if (c.getType() == CELL_TYPE::GENERIC && c.getGeneration() < 4) {
					
					if (RandomNumberGenerators::rUnifProb() <= pDiv) {

						grid.divideCell(x, y);

					}

				}

			}

		}

	}
	
	grid.printGrid();

	return 0;
}
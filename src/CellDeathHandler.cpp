#include "headers/CellDeathHandler.h"

#include "headers/CellDeathEvent.h"
#include "headers/RandomNumberGenerators.h"
#include "headers/SuperCell.h"

#include <iostream>
#include <unordered_set>
#include <algorithm>

static std::shared_ptr<SquareCellGrid> grid;

void CellDeathHandler::initializeHandler(std::shared_ptr<SquareCellGrid> ptr) {
	grid = ptr;
}

void CellDeathHandler::runDeathLoop(int m) {

	for (int d = 0; d < CellDeathEvent::getNumEvents(); d++) {

		CellDeathEvent &D = CellDeathEvent::getEvent(d);

		if (m != 0 && m % D.fireOn == 0) {

			// Random probabilistic
			if (D.type == 0) {

				for (int c = 0; c < SuperCell::getNumSupers(); c++) {

					if (SuperCell::getCellType(c) == D.targetType && !SuperCell::isDead(c)) {

						if (RandomNumberGenerators::rUnifProb() < D.data[0]) {

							SuperCell::setDead(c, true);
						}
					}
				}
			}

			// Neighbour weighted
			if (D.type == 1) {

				for (int c = 0; c < SuperCell::getNumSupers(); c++) {

					if (SuperCell::getCellType(c) == (int)D.targetType) {

						std::unordered_set<int> targetNeighbours;

						for (int y = 1; y <= grid->interiorHeight; y++) {
							for (int x = 1; x <= grid->interiorWidth; x++) {

								if (grid->getCell(x, y) == c) {

									auto N = grid->getNeighboursSuperCells(x, y);

									for (int nsc : N) {
										if (SuperCell::getCellType(nsc) == (int)D.data[0])
											targetNeighbours.insert(nsc);
									}
								}
							}
						}

						double saturation = (double)(std::max((double)targetNeighbours.size(),D.data[1]))/D.data[1];
						double prob = saturation * D.data[2];

						if(RandomNumberGenerators::rUnifProb() < prob) SuperCell::setDead(c, true);

					}
				}
			}
		}
	}
}
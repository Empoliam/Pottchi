#include "headers/CellDeathHandler.h"

#include "headers/CellDeathEvent.h"
#include "headers/RandomNumberGenerators.h"
#include "headers/SuperCell.h"

#include <iostream>

static std::shared_ptr<SquareCellGrid> grid;

void CellDeathHandler::initializeHandler(std::shared_ptr<SquareCellGrid> ptr) {
	grid = ptr;
}

void CellDeathHandler::runDeathLoop(int m) {

	for(int d = 0; d < CellDeathEvent::getNumEvents(); d++) {

		CellDeathEvent& D = CellDeathEvent::getEvent(d);

		if(m!=0 && m%D.fireOn == 0) {

			if(D.type == 0) {

				for(int c = 0; c < SuperCell::getNumSupers(); c++) {

					if(SuperCell::getCellType(c) == D.targetType && !SuperCell::isDead(c)) {

						if(RandomNumberGenerators::rUnifProb() < D.data[0]) {
							
							SuperCell::setDead(c, true);

						}

					}

				}

			}

		}

	}

}
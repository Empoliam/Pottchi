#include "./headers/DivisionHandler.h"

#include "./headers/SuperCell.h"

static std::shared_ptr<SquareCellGrid> grid;

void DivisionHandler::initializeHandler(std::shared_ptr<SquareCellGrid> ptr) {
	grid = ptr;
}

void DivisionHandler::runDivisionLoop() {
	for (int c = 0; c < SuperCell::getNumSupers(); c++) {

		if(SuperCell::isDead(c)) continue;

		if (SuperCell::doDivide(c)) {

			if (SuperCell::getMCS(c) > SuperCell::getNextDiv(c) && SuperCell::getVolume(c) >= SuperCell::getDivMinVol(c)) {

				int dType = SuperCell::getDivType(c);
				int newSuper = -1;
				switch (dType) {
				case (0):
					newSuper = grid->divideCellRandomAxis(c);
					break;
				case (1):
					newSuper = grid->divideCellShortAxis(c);
					break;
				case (2):
					newSuper = grid->cleaveCell(c);
					break;
				default:
					break;
				}

				if (newSuper > -1) {

					// std::cout << "Division at " << m << std::endl;

					SuperCell::setNextDiv(newSuper, SuperCell::generateNewDivisionTime(c));

					SuperCell::generateNewColour(newSuper);
				}

				SuperCell::setNextDiv(c, SuperCell::generateNewDivisionTime(c));
			}
		}
	}
}
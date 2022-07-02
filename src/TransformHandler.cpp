#include "./headers/TransformHandler.h"

#include <iostream>

#include "./headers/SuperCell.h"
#include "./headers/RandomNumberGenerators.h"
#include "./headers/TransformEvent.h"

static std::shared_ptr<SquareCellGrid> grid;

void TransformHandler::initializeHandler(std::shared_ptr<SquareCellGrid> ptr) {
	grid = ptr;
}

void TransformHandler::runTransformLoop() {

	for (int e = 0; e < TransformEvent::getNumEvents(); e++) {

		TransformEvent &T = TransformEvent::getEvent(e);

		if (T.triggered)
			continue;

		if (T.waitForOther && !T.timerStart) {
			if (TransformEvent::getEvent(T.eventToWait).triggered) {
				T.startTimer();
			}
		}

		if (T.mcsTimer >= T.triggerMCS) {

			if (T.reportFire)
				std::cout << "Event " << T.id << " fired" << std::endl;

			// Global transform
			if (T.transformType == 0) {

				for (int c = 0; c < SuperCell::getNumSupers(); c++) {
					if (SuperCell::getCellType(c) == T.transformFrom) {

						SuperCell::setCellType(c, T.transformTo);
						if (T.updateColour)
							SuperCell::generateNewColour(c);
						if (T.updateDiv) {
							SuperCell::setNextDiv(c, SuperCell::generateNewDivisionTime(c));
							SuperCell::setMCS(c, 0);
						}
						if (T.volumeMult != 1.0) {
							SuperCell::setTargetVolume(c, SuperCell::getTargetVolume(c) * T.volumeMult);
						}
					}
				}

			}
			// Transform, conditional on neighbours
			else if (T.transformType == 1) {

				for (int y = 1; y <= grid->interiorHeight; y++) {
					for (int x = 1; x <= grid->interiorWidth; x++) {

						int c = grid->getCell(x, y);

						if (SuperCell::getCellType(c) == T.transformFrom) {

							auto N = grid->getNeighboursCoords(x, y, T.transformData);

							if (!N.empty()) {

								SuperCell::setCellType(c, T.transformTo);
								if (T.updateColour)
									SuperCell::generateNewColour(c);
								if (T.updateDiv) {
									SuperCell::setNextDiv(c, SuperCell::generateNewDivisionTime(c));
									SuperCell::setMCS(c, 0);
								}
								if (T.volumeMult != 1.0) {
									SuperCell::setTargetVolume(c, SuperCell::getTargetVolume(c) * T.volumeMult);
								}
							}
						}
					}
				}

			}

			// Probabilistic transform
			else if (T.transformType == 2) {

				double pTransform = ((double)T.transformData / 100.0);

				for (int c = 0; c < SuperCell::getNumSupers(); c++) {

					if (SuperCell::getCellType(c) == T.transformFrom) {

						if (RandomNumberGenerators::rUnifProb() < pTransform) {
							SuperCell::setCellType(c, T.transformTo);
							if (T.updateColour)
								SuperCell::generateNewColour(c);
							if (T.updateDiv) {
								SuperCell::setNextDiv(c, SuperCell::generateNewDivisionTime(c));
								SuperCell::setMCS(c, 0);
							}
							if (T.volumeMult != 1.0) {
								SuperCell::setTargetVolume(c, SuperCell::getTargetVolume(c) * T.volumeMult);
							}
						}
					}
				}

			}

			// Random Spawn on type
			else if (T.transformType == 3) {

				bool success = false;

				while (!success) {

					int x = RandomNumberGenerators::rUnifInt(1, grid->interiorWidth);
					int y = RandomNumberGenerators::rUnifInt(1, grid->interiorHeight);

					if (SuperCell::getCellType(grid->getCell(x, y)) == T.transformFrom) {

						int newSuper = SuperCell::makeNewSuperCell(SuperCellTemplate::getTemplate(T.transformTo));
						SuperCell::generateNewColour(newSuper);
						grid->setCell(x, y, newSuper);

						success = true;
					}
				}

			}

			// Random spawn, conditional on subcell neighbours
			else if (T.transformType == 4) {

				bool success = false;
				int attempts = 0;

				while (!success && (attempts < (grid->interiorWidth * grid->interiorHeight))) {

					attempts++;

					int x = RandomNumberGenerators::rUnifInt(1, grid->interiorWidth);
					int y = RandomNumberGenerators::rUnifInt(1, grid->interiorHeight);

					if (SuperCell::getCellType(grid->getCell(x, y)) == T.transformFrom) {

						auto N = grid->getNeighboursCoords(x, y, T.transformData);

						if (!N.empty()) {

							int newSuper = SuperCell::makeNewSuperCell(SuperCellTemplate::getTemplate(T.transformTo));
							SuperCell::generateNewColour(newSuper);
							grid->setCell(x, y, newSuper);

							success = true;
						}
					}
				}
			}

			// Kill event after kill condition met
			if (T.killRepeat) {
				if (TransformEvent::getEvent(T.killOnEvent).triggered == true) {
					T.triggered = true;
				}
			}

			if (T.doRepeat) {
				T.startTimer();
			} else {
				T.triggered = true;
			}
		}
	}
}
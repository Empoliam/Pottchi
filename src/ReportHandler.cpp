#include "headers/ReportHandler.h"

#include "headers/SuperCell.h"
#include "headers/ReportEvent.h"

#include <fstream>
#include <algorithm>


static std::shared_ptr<SquareCellGrid> grid;

void ReportHandler::initializeHandler(std::shared_ptr<SquareCellGrid> ptr) {
	grid = ptr;
}

void ReportHandler::runReportLoop(int m, std::ofstream& logFile) {

    for (int r = 0; r < ReportEvent::getNumEvents(); r++) {

			ReportEvent &R = ReportEvent::getEvent(r);

			bool isFired = R.fired;
			bool triggerMet = false;
			bool notZero = (m != 0);

			if (m != 0 && !(R.fired) && (m % R.triggerOn == 0)) {

				// Unconditional log entry
				if (R.type == 0) {

					logFile << R.reportText << "," << m << "," << R.data[0] << "\n";

					if (!R.doRepeat) {
						R.fired = true;
					}
				}

				// Count all countable cells
				if (R.type == 1) {

					int cellCount = 0;

					for (int s = 0; s < SuperCell::getNumSupers(); s++) {
						if(SuperCell::isDead(s)) continue;
						cellCount += SuperCell::isCountable(s);
					}

					logFile << R.reportText << "," << m << "," << cellCount << "\n";

					if (!R.doRepeat) {
						R.fired = true;
					}
				}

				// Check if a cell of type 0 is touching type 1
				if (R.type == 2) {

					int typeA = stoi(R.data[0]);
					int typeB = stoi(R.data[1]);

					for (int y = 1; y <= grid->interiorHeight; y++) {

						for (int x = 1; x <= grid->interiorWidth; x++) {

							if (SuperCell::getCellType(grid->getCell(x, y)) == typeA) {

								std::vector<int> neighbourTypes = grid->getNeighboursTypes(x, y);

								if (std::find(neighbourTypes.begin(), neighbourTypes.end(), typeB) != neighbourTypes.end()) {

									logFile << R.reportText << "," << m << "\n";

									if (!R.doRepeat) {
										R.fired = true;
									}

									goto endLoop;
								}
							}
						}
					}

				endLoop:;
				}

				// Count all cells of a specific type
				if (R.type == 3) {

					int cellCount = 0;

					for (int s = 0; s < SuperCell::getNumSupers(); s++) {
						if(SuperCell::isDead(s)) continue;
						cellCount += SuperCell::getCellType(s) == stoi(R.data[0]);
					}

					logFile << R.reportText << "," << m << "," << cellCount << "\n";
				}

				// Check for any cell of type 0 not touching type 1
				if (R.type == 4) {

					int typeA = stoi(R.data[0]);
					int typeB = stoi(R.data[1]);

					std::map<int, bool> listOfTypeASupers;

					for (int s = 0; s < SuperCell::getNumSupers(); s++) {
						if (SuperCell::getCellType(s) == typeA && !SuperCell::isDead(s)) {
							listOfTypeASupers[s] = false;
						}
					}

					if (listOfTypeASupers.empty())
						goto emptyMap;

					for (int y = 1; y <= grid->interiorHeight; y++) {

						for (int x = 1; x <= grid->interiorWidth; x++) {

							int sc = grid->getCell(x, y);

							if (SuperCell::getCellType(sc) == typeA) {

								if (listOfTypeASupers[sc] == true)
									continue;

								std::vector<int> neighbourTypes = grid->getNeighboursTypes(x, y);

								if (std::find(neighbourTypes.begin(), neighbourTypes.end(), typeB) != neighbourTypes.end()) {
									listOfTypeASupers[sc] = true;
								}
							}
						}
					}

					for (auto neighbourStatusPair : listOfTypeASupers) {
						if (neighbourStatusPair.second == false) {

							logFile << R.reportText << "," << m << "\n";

							if (!R.doRepeat) {
								R.fired = true;
							}

							break;
						}
					}

				emptyMap:;
				}

				// Count cells of type 0 is touching type 1
				if (R.type == 5) {

					int typeA = stoi(R.data[0]);
					int typeB = stoi(R.data[1]);

					std::vector<int> confirmedSupers;

					for (int y = 1; y <= grid->interiorHeight; y++) {

						for (int x = 1; x <= grid->interiorWidth; x++) {

							if (!(std::count(confirmedSupers.begin(), confirmedSupers.end(), grid->getCell(x, y))) && SuperCell::getCellType(grid->getCell(x, y)) == typeA) {

								std::vector<int> neighbourTypes = grid->getNeighboursTypes(x, y);

								if (std::find(neighbourTypes.begin(), neighbourTypes.end(), typeB) != neighbourTypes.end()) {

									confirmedSupers.push_back(grid->getCell(x, y));
								}
							}
						}
					}

					logFile << R.reportText << "," << m << "," << confirmedSupers.size() << "\n";

					if (!R.doRepeat) {
						R.fired = true;
					}
				}

				// Count dead cells of type 0. If -1, count all dead cells.
				if (R.type == 6) {

					int type = stoi(R.data[0]);

					int cellCount = 0;

					for (int s = 0; s < SuperCell::getNumSupers(); s++) {
						if(!SuperCell::isDead(s)) continue;

						if(type == -1) {
							cellCount++;
							continue;
						}

						cellCount += SuperCell::getCellType(s) == type;
					}

					logFile << R.reportText << "," << m << "," << cellCount << "\n";

				}
			}
		}


}
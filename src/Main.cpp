#include <atomic>
#include <bitset>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <random>
#include <stdlib.h>
#include <thread>

#define _USE_MATH_DEFINES
#include <cmath>

#include <SFML/Window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "./headers/CellType.h"
#include "./headers/ColourScheme.h"
#include "./headers/MathConstants.h"
#include "./headers/RandomNumberGenerators.h"
#include "./headers/ReportEvent.h"
#include "./headers/SquareCellGrid.h"
#include "./headers/SuperCell.h"
#include "./headers/SuperCellTemplate.h"
#include "./headers/TransformEvent.h"
#include "./headers/Vector2D.h"
#include "./headers/split.h"

using namespace std;

unsigned int PIXEL_SCALE = 4;
unsigned int MAX_MCS = 84000;
unsigned int SIM_DELAY = 0;
unsigned int RENDER_FPS = 60;

double BOLTZ_TEMP = 10.0;
double OMEGA = 1.0;
double LAMBDA = 5.0;

std::string IMAGE_NAME = "default";

// Number of MCS per real hour
int MCS_HOUR_EST = 500.0;

bool AUTO_QUIT = false;

string loadName;

ostringstream logStream;

int simLoop(shared_ptr<SquareCellGrid> grid, atomic<bool> &done);
//int writeGrid(SDL_Renderer *renderer, SDL_Texture *texture, const char *filename);
unsigned int readConfig(string cfg);
shared_ptr<SquareCellGrid> initializeGrid(string imgName);

map<int, int> templateColourMap;

int main(int argc, char *argv[]) {

	if (argc == 2) {
		loadName = argv[1];
	} else {
		loadName = "default";
	}

	cout << "Loading: " << loadName << endl;

	int configStatus = readConfig(loadName + ".cfg");

	cout << "Done loading" << endl;

	// TODO sort this out
	if (configStatus) {
		cout << "Missing configuration option: " << configStatus << endl;
		return 1;
	}

	shared_ptr<SquareCellGrid> grid = initializeGrid(IMAGE_NAME + ".pgm");

	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);

	// Initialize window
	sf::RenderWindow window(sf::VideoMode(PIXEL_SCALE*grid->boundaryWidth,PIXEL_SCALE*grid->boundaryHeight), "Pottchi");
	window.setFramerateLimit(60);

	// Texture to render simulation to
	sf::Texture gridTexture;
	gridTexture.create(grid->boundaryWidth,grid->boundaryHeight);
	sf::Sprite sprite(gridTexture);
	sprite.setScale(PIXEL_SCALE,PIXEL_SCALE);

	// Start simulation loop
	std::atomic<bool> done(false);
	std::thread simLoopThread(simLoop, grid, std::ref(done));

	bool quit = false;
	bool waitForEnd = false;

	//TODO Auto quit fix

	// Graphics loop
	while (window.isOpen()) {


		// Draw grid
		Uint8* pixels = grid->getPixels().data();
		gridTexture.update(pixels);
		window.draw(sprite);
		window.display();

		sf::Event event;
		while(window.pollEvent(event)) {

			if(event.type == sf::Event::Closed) {

				done = true;
				
			}

		}

		if(done) {

			cout << "Simulation finished\n";
			simLoopThread.join();
			window.close();

		}

	}

	string imgName;

	// Rendering to PNG and outputting log
	int attempt = 0;
	do {

		std::ostringstream oss;
		oss << std::put_time(&tm, "%Y-%m-%d %H-%M-%S") << "-" << attempt << ".png";
		imgName = oss.str();

		++attempt;

	} while (filesystem::exists(imgName));

	std::ostringstream oss;
	oss << std::put_time(&tm, "%Y-%m-%d %H-%M-%S") << "-" << (attempt - 1) << ".log";
	string logName = oss.str();

	ofstream logFile;
	logFile.open(logName);
	logFile << logStream.str();
	logFile.close();

	gridTexture.copyToImage().saveToFile(imgName);

	return 0;
}

int simLoop(shared_ptr<SquareCellGrid> grid, atomic<bool> &done) {

	// Target times for key events

	unsigned int diffStartMCS;

	grid->fullTextureRefresh();

	// Number of samples to take before increasing MCS count
	unsigned int iMCS = grid->interiorWidth * grid->interiorHeight;

	// Simulation loop
	for (unsigned int m = 0; m < MAX_MCS; m++) {

		if (done) {
			break;
		}

		// Monte Carlo Step
		for (unsigned int i = 0; i < iMCS; i++) {

			int x = RandomNumberGenerators::rUnifInt(1, grid->interiorWidth);
			int y = RandomNumberGenerators::rUnifInt(1, grid->interiorHeight);

			bool success = grid->moveCell(x, y);
			
		}

		// Cell division
		for (int c = 0; c < SuperCell::getNumSupers(); c++) {

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

						cout << "Division at " << m << endl;

						SuperCell::setNextDiv(newSuper, SuperCell::generateNewDivisionTime(c));

						SuperCell::generateNewColour(newSuper);
						grid->fullTextureRefresh();
					}

					SuperCell::setNextDiv(c, SuperCell::generateNewDivisionTime(c));
				}
			}
		}

		// Transform Events
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
					cout << "Event " << T.id << " fired" << endl;

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

		// Reporting
		for (int r = 0; r < ReportEvent::getNumEvents(); r++) {

			ReportEvent &R = ReportEvent::getEvent(r);

			bool isFired = R.fired;
			bool triggerMet = false;
			bool notZero = (m != 0);

			if (m != 0 && !(R.fired) && (m % R.triggerOn == 0)) {

				// Unconditional log entry
				if (R.type == 0) {

					logStream << R.reportText << "," << m << "," << R.data[0] << "\n";

					if (!R.doRepeat) {
						R.fired = true;
					}
				}

				// Count all countable cells
				if (R.type == 1) {

					int cellCount = 0;

					for (int s = 0; s < SuperCell::getNumSupers(); s++) {
						cellCount += SuperCell::isCountable(s);
					}

					logStream << R.reportText << "," << m << "," << cellCount << "\n";

					if (!R.doRepeat) {
						R.fired = true;
					}
				}

				// Check if a cell of type 0 is touching type 1
				if (R.type == 2) {

					int typeA = R.data[0];
					int typeB = R.data[1];

					for (int y = 1; y <= grid->interiorHeight; y++) {

						for (int x = 1; x <= grid->interiorWidth; x++) {

							if (SuperCell::getCellType(grid->getCell(x, y)) == typeA) {

								std::vector<int> neighbourTypes = grid->getNeighboursTypes(x, y);

								if (std::find(neighbourTypes.begin(), neighbourTypes.end(), typeB) != neighbourTypes.end()) {

									logStream << R.reportText << "," << m << "\n";

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
						cellCount += SuperCell::getCellType(s) == R.data[0];
					}

					logStream << R.reportText << "," << m << "," << cellCount << "\n";
				}

				// Check for any cell of type 0 not touching type 1
				if (R.type == 4) {

					int typeA = R.data[0];
					int typeB = R.data[1];

					std::map<int, bool> listOfTypeASupers;

					for (int s = 0; s < SuperCell::getNumSupers(); s++) {
						if (SuperCell::getCellType(s) == typeA) {
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

							logStream << R.reportText << "," << m << "\n";

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

					int typeA = R.data[0];
					int typeB = R.data[1];

					vector<int> confirmedSupers;

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

					logStream << R.reportText << "," << m << "," << confirmedSupers.size() << "\n";

					if (!R.doRepeat) {
						R.fired = true;
					}
				}
			}
		}

		grid->fullTextureRefresh();

		// Artificial delay if desired
		if (SIM_DELAY != 0)
			std::this_thread::sleep_for(std::chrono::milliseconds(SIM_DELAY));

		if (done) {
			break;
		}

		// Increase MCS count for each cell
		SuperCell::increaseMCS();

		// Update event timers
		TransformEvent::updateTimers();

	}

	done = true;

	return 0;
}

unsigned int readConfig(string cfg) {

	std::ifstream ifs(cfg);
	string line;

	int lineNumber = 0;

	while (std::getline(ifs, line)) {

		lineNumber++;

		auto V = split(line, ',');

		if (V.empty() || V[0][0] == '#') {

			continue;

		} else if (V[0] == "SIM_PARAM") {

			string P = V[1];
			string value = V[2];

			if (P == "MCS_HOUR_EST")
				MCS_HOUR_EST = stoi(value);
			else if (P == "MAX_HOURS")
				MAX_MCS = stod(value) * MCS_HOUR_EST;
			else if (P == "PIXEL_SCALE")
				PIXEL_SCALE = stoi(value);
			else if (P == "DELAY")
				SIM_DELAY = stoi(value);
			else if (P == "FPS")
				RENDER_FPS = stoi(value);
			else if (P == "OMEGA")
				OMEGA = stod(value);
			else if (P == "LAMBDA")
				LAMBDA = stoi(value);
			else if (P == "BOLTZ_TEMP")
				BOLTZ_TEMP = stoi(value);
			else if (P == "AUTO_QUIT")
				AUTO_QUIT = (value == "1");
			else if (P == "IMAGE")
				IMAGE_NAME = value;

		}

		else if (V[0] == "CELL_TYPE") {

			CellType T(stoi(V[1]));
			while (line != "END_TYPE") {

				std::getline(ifs, line);
				lineNumber++;

				V = split(line, ',');
				string P = V[0];

				if (P == "J") {
					vector<string> J = split(V[1], ':');
					for (string S : J) {
						T.J.push_back(stod(S));
					}
				} else if (P == "DO_DIVIDE")
					T.doesDivide = (V[1] == "1");
				else if (P == "IS_STATIC")
					T.isStatic = (V[1] == "1");
				else if (P == "IGNORE_VOLUME")
					T.ignoreVolume = (V[1] == "1");
				else if (P == "DIV_MEAN")
					T.divideMean = stod(V[1]) * MCS_HOUR_EST;
				else if (P == "DIV_SD")
					T.divideSD = stod(V[1]) * MCS_HOUR_EST;
				else if (P == "DIV_TYPE")
					T.divideType = stoi(V[1]);
				else if (P == "DIV_MIN_VOL")
					T.divMinVolume = stoi(V[1]);
				else if (P == "DIV_MIN_RATIO")
					T.divMinRatio = stod(V[1]);
				else if (P == "COLOUR")
					T.colourScheme = stoi(V[1]);
				else if (P == "COUNTABLE")
					T.countable = (V[1] == "1");
			}

			CellType::addType(T);

		}

		else if (V[0] == "COLOUR_SCHEME") {

			ColourScheme CS(stoi(V[1]));
			while (line != "END_COLOUR") {

				std::getline(ifs, line);
				lineNumber++;

				V = split(line, ',');

				string c = V[0];

				if (c == "R") {
					CS.rMin = stoi(V[1]);
					CS.rMax = stoi(V[2]);
				} else if (c == "G") {
					CS.gMin = stoi(V[1]);
					CS.gMax = stoi(V[2]);
				} else if (c == "B") {
					CS.bMin = stoi(V[1]);
					CS.bMax = stoi(V[2]);
				}
			}

			ColourScheme::addScheme(CS);

		}

		else if (V[0] == "TEMPLATE") {

			SuperCellTemplate T(stoi(V[1]));

			while (line != "END_TEMPLATE") {

				std::getline(ifs, line);
				lineNumber++;

				V = split(line, ',');

				string c = V[0];

				if (c == "TYPE")
					T.type = stoi(V[1]);
				else if (c == "VOLUME")
					T.volume = stoi(V[1]);
				else if (c == "SPECIAL")
					T.specialType = stoi(V[1]);
			}

			if (T.type != -1) {
				SuperCellTemplate::addTemplate(T);
			}

		}

		else if (V[0] == "MAP_TEMPLATE") {
			templateColourMap[stoi(V[1])] = stoi(V[2]);
		}

		else if (V[0] == "EVENT_DEFINE") {

			TransformEvent T(stoi(V[1]));

			while (line != "END_EVENT") {

				std::getline(ifs, line);
				lineNumber++;

				V = split(line, ',');

				string c = V[0];

				if (c == "TIME_MEAN")
					T.triggerMean = stod(V[1]) * MCS_HOUR_EST;
				else if (c == "TIME_SD")
					T.triggerSD = stod(V[1]) * MCS_HOUR_EST;
				else if (c == "TRANSFORM_FROM")
					T.transformFrom = stoi(V[1]);
				else if (c == "TRANSFORM_TO")
					T.transformTo = stoi(V[1]);
				else if (c == "TRANSFORM_TYPE")
					T.transformType = stoi(V[1]);
				else if (c == "TRANSFORM_DATA")
					T.transformData = stoi(V[1]);
				else if (c == "WAIT_FOR_OTHER")
					T.waitForOther = (V[1] == "1");
				else if (c == "EVENT_TO_WAIT")
					T.eventToWait = stoi(V[1]);
				else if (c == "UPDATE_COLOUR")
					T.updateColour = (V[1] == "1");
				else if (c == "UPDATE_DIV")
					T.updateDiv = (V[1] == "1");
				else if (c == "DO_REPEAT")
					T.doRepeat = (V[1] == "1");
				else if (c == "REPORT_FIRE")
					T.reportFire = (V[1] == "1");
				else if (c == "VOLUME_MULT")
					T.volumeMult = stod(V[1]);
			}

			TransformEvent::addNewEvent(T);
		}

		else if (V[0] == "REPORT_DEFINE") {

			ReportEvent R(stoi(V[1]));

			while (line != "END_REPORT") {

				std::getline(ifs, line);
				lineNumber++;

				V = split(line, ',');

				string c = V[0];

				if (c == "TIME")
					R.triggerOn = (int)(stod(V[1]) * MCS_HOUR_EST);
				else if (c == "TYPE")
					R.type = stoi(V[1]);
				else if (c == "REPEAT")
					R.doRepeat = (V[1] == "1");
				else if (c == "DATA") {
					vector<string> D = split(V[1], ':');
					for (string S : D) {
						R.data.push_back(stod(S));
					}
				} else if (c == "TEXT")
					R.reportText = V[1];
				else if (c != "END_REPORT")
					cout << "Unknown report config on line " << lineNumber << endl;
			}

			ReportEvent::addNewEvent(R);
		}

		else {

			cout << "Unrecognised tag " << V[0] << " on line " << lineNumber << endl;
		}
	}

	ifs.close();

	for (int e = 0; e < TransformEvent::getNumEvents(); e++) {
		TransformEvent &T = TransformEvent::getEvent(e);

		if (T.waitForOther == false) {
			T.generateNewTriggerTime();
			T.startTimer();
		}
	}

	return 0;
}

shared_ptr<SquareCellGrid> initializeGrid(string imgName) {

	std::ifstream ifs(imgName);
	string pgmString;
	getline(ifs, pgmString);
	getline(ifs, pgmString);
	int SIM_WIDTH = stoi(pgmString);
	getline(ifs, pgmString);
	int SIM_HEIGHT = stoi(pgmString);
	getline(ifs, pgmString);

	int boundarySuper = 0;
	int spaceSuper = 1;

	std::map<int, int> tempSuperMap;

	for (const auto &[cVal, superTemplate] : templateColourMap) {
		tempSuperMap[cVal] = SuperCell::makeNewSuperCell(SuperCellTemplate::getTemplate(superTemplate));

		int sType = SuperCellTemplate::getTemplate(superTemplate).specialType;
		if (sType == 1) {
			boundarySuper = tempSuperMap[cVal];
		} else if (sType == 2) {
			spaceSuper = tempSuperMap[cVal];
		}
	}

	shared_ptr<SquareCellGrid> grid(new SquareCellGrid(SIM_WIDTH, SIM_HEIGHT, boundarySuper, spaceSuper));

	for (int y = 1; y <= grid->interiorHeight; y++) {
		for (int x = 1; x <= grid->interiorWidth; x++) {

			uint8_t b = 0;
			ifs >> b;

			try {
				int setSC = tempSuperMap[b];
				grid->setCell(x, y, setSC);
			} catch (out_of_range) {
				grid->setCell(x, y, 0);
			}
		}
	}

	for (int c = 0; c < SuperCell::getNumSupers(); c++) {

		SuperCell::generateNewColour(c);

		if (SuperCell::doDivide(c)) {
			SuperCell::setNextDiv(c, SuperCell::generateNewDivisionTime(c));
		}
	}

	grid->BOLTZ_TEMP = BOLTZ_TEMP;
	grid->OMEGA = OMEGA;
	grid->LAMBDA = LAMBDA;

	return grid;
}

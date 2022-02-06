#include <random>
#include <chrono>
#include <thread>
#include <iostream>
#include <fstream>
#include <atomic>
#include <thread>
#include <math.h>
#include <bitset>

#define _USE_MATH_DEFINES
#include <cmath>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "./headers/SuperCell.h"
#include "./headers/SquareCellGrid.h"
#include "./headers/Vector2D.h"
#include "./headers/RandomNumberGenerators.h"
#include "./headers/MathConstants.h"
#include "./headers/split.h"
#include "./headers/CellType.h"
#include "./headers/ColourScheme.h"
#include "./headers/TransformEvent.h"

using namespace std;

unsigned int PIXEL_SCALE = 4;
unsigned int MAX_MCS = 84000;
unsigned int SIM_DELAY = 0;
unsigned int RENDER_FPS = 60;

double BOLTZ_TEMP = 10.0;
double OMEGA = 1.0;
double LAMBDA = 5.0;
double SIGMA = 0;

//Number of MCS per real hour
int MCS_HOUR_EST = 500.0;

string loadName;

int simLoop(shared_ptr<SquareCellGrid> grid, atomic<bool>& done);
int printGrid(SDL_Renderer* renderer, SDL_Texture* texture, shared_ptr<SquareCellGrid> grid);
unsigned int readConfig(string cfg);
shared_ptr<SquareCellGrid> initializeGrid(string imgName);

map<int, int> initSCMap = map<int, int>();

int main(int argc, char* argv[]) {

	if (argc == 2) {
		loadName = argv[1];
	}
	else {
		loadName = "default";
	}

	cout << "Loading: " << loadName << endl;

	int configStatus = readConfig(loadName + ".cfg");

	if (configStatus) {
		cout << "Missing configuration option: " << configStatus << endl;
		return 1;
	}

	shared_ptr<SquareCellGrid> grid = initializeGrid(loadName + ".pgm");

	SDL_SetMainReady();
	SDL_Event event;
	SDL_Renderer* renderer;
	SDL_Window* window;

	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer((grid->boundaryWidth) * PIXEL_SCALE, (grid->boundaryHeight + 2) * PIXEL_SCALE, 0, &window, &renderer);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);

	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, grid->boundaryWidth, grid->boundaryHeight);

	printGrid(renderer, texture, grid);

	std::atomic<bool> done(false);
	std::thread simLoopThread(simLoop, grid, std::ref(done));

	bool quit = false;
	bool waitForEnd = false;

	unsigned int tickA, tickB, tickDelta;
	tickB = SDL_GetTicks();

	while (!quit) {

		if (!waitForEnd) {

			tickA = SDL_GetTicks();
			tickDelta = tickA - tickB;

			if (done) {
				cout << "done";
				printGrid(renderer, texture, grid);
				simLoopThread.join();
				waitForEnd = true;
			}
			else {

				if (tickDelta > 1000 / RENDER_FPS) {

					//cout << "fps: " << (double)1000 / tickDelta << "\n";
					tickB = tickA;
					printGrid(renderer, texture, grid);

				}

			}

		}

		if (SDL_PollEvent(&event) && event.type == SDL_QUIT) {
			done = true;
			quit = true;
		}

	}

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

int simLoop(shared_ptr<SquareCellGrid> grid, atomic<bool>& done) {

	//Target times for key events

	unsigned int diffStartMCS;

	grid->fullTextureRefresh();
	grid->fullPerimeterRefresh();

	//Number of samples to take before increasing MCS count
	unsigned int iMCS = grid->interiorWidth * grid->interiorHeight;

	//Simulation loop
	for (unsigned int m = 0; m < MAX_MCS; m++) {

		//Monte Carlo Step
		for (unsigned int i = 0; i < iMCS; i++) {

			int x = RandomNumberGenerators::rUnifInt(1, grid->interiorWidth);
			int y = RandomNumberGenerators::rUnifInt(1, grid->interiorHeight);

			bool success = grid->moveCell(x, y);

			if (done) {
				break;
			}

		}

		for (int c = 0; c < SuperCell::getNumSupers(); c++) {

			if (SuperCell::doDivide(c)) {

				if (SuperCell::getMCS(c) > SuperCell::getNextDiv(c) && SuperCell::getVolume(c) >= SuperCell::getDivMinVol(c)) {

					int dType = SuperCell::getDivType(c);
					int newSuper = -1;
					switch (dType)
					{
					case(0):
						newSuper = grid->divideCellRandomAxis(c);
						break;
					case(1):
						newSuper = grid->divideCellShortAxis(c);
						break;
					case(2):
						newSuper = grid->cleaveCell(c);
						break;
					default:
						break;
					}

					if (newSuper > -1) {

						grid->fullPerimeterRefresh();
						
						cout << "Division at " << m << endl;
						
						SuperCell::setNextDiv(newSuper, SuperCell::generateNewDivisionTime(c));

						SuperCell::generateNewColour(newSuper);
						grid->fullTextureRefresh();

					}

					SuperCell::setNextDiv(c, SuperCell::generateNewDivisionTime(c));

					

				}

			}

		}

		for (int e = 0; e < TransformEvent::getNumEvents(); e++) {

			TransformEvent& T = TransformEvent::getEvent(e);

			if (T.triggered) continue;

			if (T.waitForOther && !T.timerStart) {
				if (TransformEvent::getEvent(T.eventToWait).triggered) {
					T.startTimer();
				}
			}

			if (T.mcsTimer >= T.triggerMCS) {

				if(T.reportFire) cout << "Event " << T.id << " fired" << endl;

				if (T.transformType == 0) {

					for (int c = 0; c < SuperCell::getNumSupers(); c++) {
						if (SuperCell::getCellType(c) == T.transformFrom) {

							SuperCell::setCellType(c, T.transformTo);
							if (T.updateColour) SuperCell::generateNewColour(c);
							if (T.updateDiv) {
								SuperCell::setNextDiv(c, SuperCell::generateNewDivisionTime(c));
								SuperCell::setMCS(c, 0);
							}

						}
					}

				}

				else if (T.transformType == 1) {

					for (int y = 1; y <= grid->interiorHeight; y++) {
						for (int x = 1; x <= grid->interiorWidth; x++) {

							int c = grid->getCell(x, y);

							if (SuperCell::getCellType(c) == T.transformFrom) {

								auto N = grid->getNeighboursCoords(x, y, T.transformData);

								if (!N.empty()) {

									SuperCell::setCellType(c, T.transformTo);
									if (T.updateColour) SuperCell::generateNewColour(c);
									if (T.updateDiv) {
										SuperCell::setNextDiv(c, SuperCell::generateNewDivisionTime(c));
										SuperCell::setMCS(c, 0);
									}

								}

							}

						}

					}

				}

				else if (T.transformType == 2) {

					double pTransform = ((double)T.transformData / 100.0);

					for (int c = 0; c < SuperCell::getNumSupers(); c++) {

						if (SuperCell::getCellType(c) == T.transformFrom) {

							if (RandomNumberGenerators::rUnifProb() < pTransform) {
								SuperCell::setCellType(c, T.transformTo);
								if (T.updateColour) SuperCell::generateNewColour(c);
								if (T.updateDiv) {
									SuperCell::setNextDiv(c, SuperCell::generateNewDivisionTime(c));
									SuperCell::setMCS(c, 0);
								}

							}

						}

					}

				}

				else if (T.transformType == 3) {

					bool success = false;

					while (!success) {

						int x = RandomNumberGenerators::rUnifInt(1, grid->interiorWidth);
						int y = RandomNumberGenerators::rUnifInt(1, grid->interiorHeight);

						if (SuperCell::getCellType(grid->getCell(x, y)) == T.transformFrom) {

							int newSuper = SuperCell::makeNewSuperCell(T.transformTo);
							SuperCell::generateNewColour(newSuper);
							grid->setCell(x, y, newSuper);

							success = true;


						}

					}

				}

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

								int newSuper = SuperCell::makeNewSuperCell(T.transformTo);
								SuperCell::generateNewColour(newSuper);
								grid->setCell(x, y, newSuper);

								success = true;

							}

						}

					}

				}

				if (T.doRepeat) {
					T.startTimer();
				}
				else {
					T.triggered = true;
				}

			}



		}

		grid->fullTextureRefresh();
		grid->fullPerimeterRefresh();


		//Fluid expansion
		//int newFluidVolume = max(50, (int)(TARGET_MAX_FLUID * (1 - exp(-((m - diffStartMCS) / (TARGET_SCALE_FLUID))))));
		//SuperCell::setTargetVolume(FLUID_TYPE, newFluidVolume);



		//Artificial delay if desired
		if (SIM_DELAY != 0) std::this_thread::sleep_for(std::chrono::milliseconds(SIM_DELAY));

		if (done) {
			break;
		}

		//Increase MCS count for each cell
		SuperCell::increaseMCS();

		//Update event timers
		TransformEvent::updateTimers();

		//Recalculate Perimeters
		grid->fullPerimeterRefresh();

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

		}
		else if (V[0] == "SIM_PARAM") {

			string P = V[1];
			string value = V[2];

			if (P == "MCS_HOUR_EST")MCS_HOUR_EST = stoi(value);
			else if (P == "MAX_HOURS") MAX_MCS = stod(value) * MCS_HOUR_EST;
			else if (P == "PIXEL_SCALE")PIXEL_SCALE = stoi(value);
			else if (P == "DELAY") SIM_DELAY = stoi(value);
			else if (P == "FPS") RENDER_FPS = stoi(value);			
			else if (P == "OMEGA") OMEGA = stod(value);
			else if (P == "LAMBDA") LAMBDA = stoi(value);
			else if (P == "SIGMA") SIGMA = stoi(value);
			else if (P == "BOLTZ_TEMP") BOLTZ_TEMP = stoi(value);

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
				}
				else if (P == "DO_DIVIDE") T.doesDivide = (V[1] == "1");
				else if (P == "IS_STATIC") T.isStatic = (V[1] == "1");
				else if (P == "IGNORE_VOLUME") T.ignoreVolume = (V[1] == "1");
				else if (P == "IGNORE_SURFACE") T.ignoreSurface = (V[1] == "1");
				else if (P == "DIV_MEAN") T.divideMean = stod(V[1]) * MCS_HOUR_EST;
				else if (P == "DIV_SD") T.divideSD = stod(V[1]) * MCS_HOUR_EST;
				else if (P == "DIV_TYPE") T.divideType = stoi(V[1]);
				else if (P == "DIV_MIN_VOL") T.divMinVolume = stoi(V[1]);
				else if (P == "DIV_MIN_RATIO") T.divMinRatio = stod(V[1]);
				else if (P == "COLOUR") T.colourScheme = stoi(V[1]);

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
				}
				else if (c == "G") {
					CS.gMin = stoi(V[1]);
					CS.gMax = stoi(V[2]);
				}
				else if (c == "B") {
					CS.bMin = stoi(V[1]);
					CS.bMax = stoi(V[2]);
				}
			}

			ColourScheme::addScheme(CS);

		}

		else if (V[0] == "SUPERCELL") {

			int id = stoi(V[1]);

			int type = 0;
			int initVol = 0;
			int initLength = 0;

			while (line != "END_SUPER") {

				std::getline(ifs, line);
				lineNumber++;

				V = split(line, ',');

				string c = V[0];

				if (c == "TYPE") type = stoi(V[1]);
				else if (c == "INITIAL_VOLUME") initVol = stoi(V[1]);
				else if (c == "INITIAL_LENGTH") initLength = stoi(V[1]);
				else if (c == "LOAD_COLOUR") {
					initSCMap[stoi(V[1])] = id;
				}

			}

			SuperCell::makeNewSuperCell(type, 0, initVol, initLength);

		}

		else if (V[0] == "EVENT_DEFINE") {

			TransformEvent T(stoi(V[1]));

			while (line != "END_EVENT") {

				std::getline(ifs, line);
				lineNumber++;

				V = split(line, ',');

				string c = V[0];

				if (c == "TIME_MEAN") T.triggerMean = stod(V[1]) * MCS_HOUR_EST;
				else if (c == "TIME_SD") T.triggerSD = stod(V[1]) * MCS_HOUR_EST;
				else if (c == "TRANSFORM_FROM") T.transformFrom = stoi(V[1]);
				else if (c == "TRANSFORM_TO") T.transformTo = stoi(V[1]);
				else if (c == "TRANSFORM_TYPE") T.transformType = stoi(V[1]);
				else if (c == "TRANSFORM_DATA") T.transformData = stoi(V[1]);
				else if (c == "WAIT_FOR_OTHER") T.waitForOther = (V[1] == "1");
				else if (c == "EVENT_TO_WAIT") T.eventToWait = stoi(V[1]);
				else if (c == "UPDATE_COLOUR") T.updateColour = (V[1] == "1");
				else if (c == "UPDATE_DIV") T.updateDiv = (V[1] == "1");
				else if (c == "DO_REPEAT") T.doRepeat = (V[1] == "1");
				else if (c == "REPORT_FIRE") T.reportFire = (V[1] == "1");

			}

			TransformEvent::addNewEvent(T);

		}

		else {

			cout << "Unrecognised tag " << V[0] << " on line " << lineNumber << endl;

		}

	}

	ifs.close();

	for (int c = 0; c < SuperCell::getNumSupers(); c++) {

		SuperCell::generateNewColour(c);

		if (SuperCell::doDivide(c)) {
			SuperCell::setNextDiv(c, SuperCell::generateNewDivisionTime(c));
		}

	}

	for (int e = 0; e < TransformEvent::getNumEvents(); e++) {
		TransformEvent& T = TransformEvent::getEvent(e);

		if (T.waitForOther == false) {
			T.generateNewTriggerTime();
			T.startTimer();
		}

	}

	return 0;

}

shared_ptr<SquareCellGrid> initializeGrid(string imgName) {

	int targetInitCellLength = (int)BORDER_CONST * sqrt(3600);

	int newSuperCell = 3;

	std::ifstream ifs(imgName);
	string pgmString;
	getline(ifs, pgmString);
	getline(ifs, pgmString);
	int SIM_WIDTH = stoi(pgmString);
	getline(ifs, pgmString);
	int SIM_HEIGHT = stoi(pgmString);
	getline(ifs, pgmString);

	shared_ptr<SquareCellGrid> grid(new SquareCellGrid(SIM_WIDTH, SIM_HEIGHT));

	for (int y = 1; y <= grid->interiorHeight; y++) {
		for (int x = 1; x <= grid->interiorWidth; x++) {

			uint8_t b = 0;
			ifs >> b;

			try {
				int setSC = initSCMap.at(b);
				grid->setCell(x, y, setSC);
			}
			catch (out_of_range) {
				grid->setCell(x, y, 0);
			}

		}
	}

	grid->BOLTZ_TEMP = BOLTZ_TEMP;
	grid->OMEGA = OMEGA;
	grid->SIGMA = SIGMA;
	grid->LAMBDA = LAMBDA;

	return grid;

}

int printGrid(SDL_Renderer* renderer, SDL_Texture* texture, shared_ptr<SquareCellGrid> grid) {

	std::vector<unsigned char> pixels = grid->getPixels();

	SDL_UpdateTexture(texture, NULL, pixels.data(), grid->boundaryWidth * 4);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);

	return 0;

}
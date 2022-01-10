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

unsigned int PIXEL_SCALE = 4;
unsigned int MAX_MCS = 86400;
unsigned int SIM_DELAY = 0;
unsigned int RENDER_FPS = 60;

double BOLTZ_TEMP = 10.0;
double OMEGA = 1.0;
double LAMBDA = 5.0;
double SIGMA = 0;

//Number of MCS per real hour
int MCS_HOUR_EST = 500.0;

//Target time of compaction
double MCS_COMPACT_TARGET = 3 * 24 * MCS_HOUR_EST;
double SD_COMPACT_TARGET = 0.5 * MCS_HOUR_EST;

//Target time of intiial differentiation
double MCS_DIFFERENTIATE_TARGET = 4 * 24 * MCS_HOUR_EST;
double SD_DIFFERENTIATE_TARGET = 1 * MCS_HOUR_EST;

//Fluid cell growth parameters
int TARGET_MAX_FLUID = 6400;
double TARGET_SCALE_FLUID = 36 * MCS_HOUR_EST;

using namespace std;

int simLoop(shared_ptr<SquareCellGrid> grid, atomic<bool>& done);
int printGrid(SDL_Renderer* renderer, SDL_Texture* texture, shared_ptr<SquareCellGrid> grid);
unsigned int readConfig(string cfg);
shared_ptr<SquareCellGrid> initializeGrid(string imgName);

int main(int argc, char* argv[]) {

	int configStatus = readConfig("default.cfg");

	if (configStatus) {
		cout << "Missing configuration option: " << configStatus << endl;
		return 1;
	}

	shared_ptr<SquareCellGrid> grid = initializeGrid("default.pgm");

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
	bool compacted = false;
	unsigned int compactionTime = (unsigned int)RandomNumberGenerators::rNormalDouble(MCS_COMPACT_TARGET, SD_COMPACT_TARGET);

	bool differentationA = false;
	unsigned int differentiationTime = (unsigned int)RandomNumberGenerators::rNormalDouble(MCS_DIFFERENTIATE_TARGET, SD_DIFFERENTIATE_TARGET);

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

						grid->fullTextureRefresh();
						grid->fullPerimeterRefresh();

						SuperCell::setNextDiv(newSuper, SuperCell::generateNewDivisionTime(c));

					}

					SuperCell::setNextDiv(c, SuperCell::generateNewDivisionTime(c));

				}

			}

		}

		//Compaction stage
		if (!compacted && m >= compactionTime) {
			for (int c = (int)CELL_TYPE::GENERIC; c < SuperCell::getNumSupers(); c++) {

				if (SuperCell::getCellType(c) == (int)CELL_TYPE::GENERIC) {

					SuperCell::setCellType(c, (int)CELL_TYPE::GENERIC_COMPACT);

				}

			}
			cout << "Compaction at: " << m << endl;
			compacted = true;
		}



		if (!differentationA && m >= differentiationTime) {

			cout << "Differentiation at: " << m << endl;

			for (int y = 1; y <= grid->interiorHeight; y++) {
				for (int x = 1; x <= grid->interiorWidth; x++) {

					int c = grid->getCell(x, y);

					if (SuperCell::getCellType(c) == (int)CELL_TYPE::GENERIC_COMPACT) {

						auto N = grid->getNeighboursCoords(x, y, (int)CELL_TYPE::EMPTYSPACE);

						if (!N.empty()) {

							SuperCell::setCellType(c, (int)CELL_TYPE::TROPHECTODERM);
							SuperCell::generateNewColour(c);
							SuperCell::setNextDiv(c, SuperCell::generateNewDivisionTime(c));
							SuperCell::setMCS(c, 0);

						}

					}

				}

			}

			for (int y = 1; y <= grid->interiorHeight; y++) {
				for (int x = 1; x <= grid->interiorWidth; x++) {

					int c = grid->getCell(x, y);

					if (SuperCell::getCellType(c) == (int)CELL_TYPE::GENERIC_COMPACT) {

						SuperCell::setCellType(c, (int)CELL_TYPE::ICM);
						SuperCell::generateNewColour(c);
						SuperCell::setNextDiv(c, SuperCell::generateNewDivisionTime(c));
						SuperCell::setMCS(c, 0);

					}

				}

			}

			bool fluidCreation = false;
			while (!fluidCreation) {

				int x = RandomNumberGenerators::rUnifInt(1, grid->interiorWidth);
				int y = RandomNumberGenerators::rUnifInt(1, grid->interiorHeight);

				int c = grid->getCell(x, y);

				if (SuperCell::getCellType(c) == (int)CELL_TYPE::ICM) {

					grid->setCell(x, y, (int)CELL_TYPE::FLUID);
					SuperCell::setColour((int)CELL_TYPE::FLUID, vector<int> {154, 102, 102, 255});

					fluidCreation = true;

				}

			}

			diffStartMCS = m;
			differentationA = true;

			grid->fullTextureRefresh();
			grid->fullPerimeterRefresh();

		}

		if (differentationA) {
				
			//Fluid expansion
			int newFluidVolume = max(50, (int)(TARGET_MAX_FLUID * (1 - exp(-((m - diffStartMCS) / (TARGET_SCALE_FLUID))))));
			SuperCell::setTargetVolume((int)CELL_TYPE::FLUID, newFluidVolume);

		}

		//Artificial delay if desired
		if (SIM_DELAY != 0) std::this_thread::sleep_for(std::chrono::milliseconds(SIM_DELAY));

		if (done) {
			break;
		}

		//Increase MCS count for each cell
		SuperCell::increaseMCS();

		//Recalculate Perimeters
		grid->fullPerimeterRefresh();

	}

	done = true;

	return 0;

}

unsigned int readConfig(string cfg) {

	std::ifstream ifs(cfg);
	string line;

	while (std::getline(ifs, line)) {

		auto V = split(line, ',');

		if (V[0] == "#") {

			continue;

		}
		else if (V[0] == "SIM_PARAM") {

			string P = V[1];
			string value = V[2];

			if (P == "MAX_MCS") MAX_MCS = stoi(value);
			else if (P == "PIXEL_SCALE")PIXEL_SCALE = stoi(value);
			else if (P == "DELAY") SIM_DELAY = stoi(value);
			else if (P == "FPS") RENDER_FPS = stoi(value);
			else if (P == "MCS_HOUR_EST")MCS_HOUR_EST = stoi(value);
			else if (P == "OMEGA") OMEGA = stod(value);
			else if (P == "LAMBDA") LAMBDA = stoi(value);
			else if (P == "SIGMA") SIGMA = stoi(value);
			else if (P == "BOLTZ_TEMP") BOLTZ_TEMP = stoi(value);

		}

		else if (V[0] == "CELL_TYPE") {

			CellType T(stoi(V[1]));
			while (line != "END_TYPE") {

				std::getline(ifs, line);
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
				else if (P == "COLOUR") T.colourScheme = stoi(V[1]);

			}

			CellType::addType(T);

		}

		else if (V[0] == "COLOUR_SCHEME") {

			ColourScheme CS(stoi(V[1]));
			while (line != "END_COLOUR") {

				std::getline(ifs, line);

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

	}

	return 0;

}

shared_ptr<SquareCellGrid> initializeGrid(string imgName) {

	SuperCell::makeNewSuperCell((int)CELL_TYPE::BOUNDARY, 0, 0, 0);
	SuperCell::setColour((int)CELL_TYPE::BOUNDARY, 255, 255, 255, 255);
	SuperCell::makeNewSuperCell((int)CELL_TYPE::EMPTYSPACE, 0, 0, 0);
	SuperCell::setColour((int)CELL_TYPE::EMPTYSPACE, 0, 0, 0, 255);
	SuperCell::makeNewSuperCell((int)CELL_TYPE::FLUID, 0, 0, 50);
	SuperCell::setColour((int)CELL_TYPE::FLUID, 50, 50, 50, 255);

	int targetInitCellLength = (int)BORDER_CONST * sqrt(3600);

	int newSuperCell = SuperCell::makeNewSuperCell((int)CELL_TYPE::GENERIC, 0, 3600, targetInitCellLength);

	SuperCell::setNextDiv(newSuperCell, SuperCell::generateNewDivisionTime(newSuperCell));

	std::ifstream ifs(imgName);
	string pgmString;
	getline(ifs, pgmString);
	getline(ifs, pgmString);
	int SIM_WIDTH = stoi(pgmString);
	getline(ifs, pgmString);
	int SIM_HEIGHT = stoi(pgmString);
	getline(ifs, pgmString);

	shared_ptr<SquareCellGrid> grid(new SquareCellGrid(SIM_WIDTH, SIM_HEIGHT));

	for (int y = 1; y <= grid->interiorWidth; y++) {
		for (int x = 1; x <= grid->interiorHeight; x++) {

			uint8_t b = 0;
			ifs >> b;

			if (b == 0xFF) {
				grid->setCell(x, y, newSuperCell);
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
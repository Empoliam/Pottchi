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

#include "./headers/Cell.h"
#include "./headers/SquareCellGrid.h"
#include "./headers/Vector2D.h"
#include "./headers/RandomNumberGenerators.h"
#include "./headers/MathConstants.h"
#include "./headers/split.h"

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

int TARGET_INIT_CELLS = 3200;

//Target time morula cell division spacing
double MCS_M_DIV_TARGET = 12 * MCS_HOUR_EST;
double SD_M_DIV_TARGET = 0.5 * MCS_HOUR_EST;

//Target time of compaction
double MCS_COMPACT_TARGET = 3 * 24 * MCS_HOUR_EST;
double SD_COMPACT_TARGET = 0.5 * MCS_HOUR_EST;

//Target time of intiial differentiation
double MCS_DIFFERENTIATE_TARGET = 4 * 24 * MCS_HOUR_EST;
double SD_DIFFERENTIATE_TARGET = 1 * MCS_HOUR_EST;

//Fluid cell growth parameters
int TARGET_MAX_FLUID = 6400;
double TARGET_SCALE_FLUID = 36 * MCS_HOUR_EST;

//Trophectoderm division
double MCS_T_DIV_TARGET_INIT = 9 * MCS_HOUR_EST;
double MCS_T_DIV_SCALE_TIME = 250;
double SD_T_DIV_TARGET = 3 * MCS_HOUR_EST;

int inline funcTrophectoderm(int m) {
	return MCS_T_DIV_TARGET_INIT + pow(m / MCS_T_DIV_SCALE_TIME, 2);
}

//ICM division parameters
const double MCS_I_DIV_TARGET = 12 * MCS_HOUR_EST;
const double SD_I_DIV_TARGET = 1 * MCS_HOUR_EST;

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

	SuperCell::makeNewSuperCell(CELL_TYPE::BOUNDARY, 0, 0, 0);
	SuperCell::setColour((int)CELL_TYPE::BOUNDARY, 255, 255, 255, 255);
	SuperCell::makeNewSuperCell(CELL_TYPE::EMPTYSPACE, 0, 0, 0);
	SuperCell::setColour((int)CELL_TYPE::EMPTYSPACE, 0, 0, 0, 255);
	SuperCell::makeNewSuperCell(CELL_TYPE::FLUID, 0, 0, 50);
	SuperCell::setColour((int)CELL_TYPE::FLUID, 50, 50, 50, 255);

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

		//Morula division stage
		if (!compacted) {
			for (int c = (int)CELL_TYPE::GENERIC; c < SuperCell::getCounter(); c++) {

				if (SuperCell::getCellType(c) == CELL_TYPE::GENERIC && SuperCell::getGeneration(c) < 4) {

					if (SuperCell::getMCS(c) >= SuperCell::getNextDiv(c)) {

						cout << "Division: " << c << " at " << SuperCell::getMCS(c) << endl;
						int newSuper = grid->cleaveCell(c);

						grid->fullTextureRefresh();
						grid->fullPerimeterRefresh();

						SuperCell::setNextDiv(c, (int)RandomNumberGenerators::rNormalDouble(MCS_M_DIV_TARGET, SD_M_DIV_TARGET));
						SuperCell::setNextDiv(newSuper, (int)RandomNumberGenerators::rNormalDouble(MCS_M_DIV_TARGET, SD_M_DIV_TARGET));

					}

				}

			}

			//Compaction stage
			if (m >= compactionTime) {
				for (int c = (int)CELL_TYPE::GENERIC; c < SuperCell::getCounter(); c++) {

					if (SuperCell::getCellType(c) == CELL_TYPE::GENERIC) {

						SuperCell::setCellType(c, CELL_TYPE::GENERIC_COMPACT);

					}

				}
				cout << "Compaction at: " << m << endl;
				compacted = true;
			}

		}

		if (compacted && !differentationA) {

			if (m >= differentiationTime) {

				cout << "Differentiation at: " << m << endl;

				for (int y = 1; y <= grid->interiorHeight; y++) {
					for (int x = 1; x <= grid->interiorWidth; x++) {

						Cell& c = grid->getCell(x, y);

						if (c.getType() == CELL_TYPE::GENERIC_COMPACT) {

							auto N = grid->getNeighbours(x, y, CELL_TYPE::EMPTYSPACE);

							if (!N.empty()) {

								c.setType(CELL_TYPE::TROPHECTODERM);
								c.generateNewColour();
								c.setNextDiv((int)RandomNumberGenerators::rNormalDouble(funcTrophectoderm(0), SD_T_DIV_TARGET));
								c.setMCS(0);

							}

						}

					}

				}

				for (int y = 1; y <= grid->interiorHeight; y++) {
					for (int x = 1; x <= grid->interiorWidth; x++) {

						Cell& c = grid->getCell(x, y);

						if (c.getType() == CELL_TYPE::GENERIC_COMPACT) {

							c.setType(CELL_TYPE::ICM);
							c.generateNewColour();
							c.setNextDiv((int)RandomNumberGenerators::rNormalDouble(MCS_I_DIV_TARGET, SD_I_DIV_TARGET));
							c.setMCS(0);

						}

					}

				}

				bool fluidCreation = false;
				while (!fluidCreation) {

					int x = RandomNumberGenerators::rUnifInt(1, grid->interiorWidth);
					int y = RandomNumberGenerators::rUnifInt(1, grid->interiorHeight);

					Cell& c = grid->getCell(x, y);

					if (c.getType() == CELL_TYPE::ICM) {

						c.setSuperCell((int)CELL_TYPE::FLUID);
						SuperCell::setColour((int)CELL_TYPE::FLUID, vector<int> {154, 102, 102, 255});

						fluidCreation = true;

					}

				}

				diffStartMCS = m;
				differentationA = true;

				grid->fullTextureRefresh();
				grid->fullPerimeterRefresh();

			}

		}

		if (differentationA) {

			//TODO Cell type enum removal
			for (int c = (int)CELL_TYPE::GENERIC; c < SuperCell::getCounter(); c++) {

				if (SuperCell::getCellType(c) == CELL_TYPE::TROPHECTODERM) {

					if (SuperCell::getMCS(c) >= SuperCell::getNextDiv(c)) {

						bool divideSuccess = false;
						vector<reference_wrapper<Cell>> cList;

						for (int x = 0; x <= grid->interiorWidth; x++) {

							for (int y = 0; y <= grid->interiorHeight; y++) {

								Cell& activeCell = grid->getCell(x, y);

								if (activeCell.getSuperCell() == c) {

									auto N = grid->getNeighbours(x, y, CELL_TYPE::EMPTYSPACE);

									cList.push_back(activeCell);

									if (!N.empty()) {

										cout << "Division: " << c << " at " << SuperCell::getMCS(c) << endl;
										int newSuper = grid->divideCellRandomAxis(c);

										SuperCell::setNextDiv(newSuper, (int)RandomNumberGenerators::rNormalDouble(funcTrophectoderm(m - diffStartMCS), SD_T_DIV_TARGET));


										divideSuccess = true;
										goto endLoop;

									}

								}

							}

						}

						if (!divideSuccess) {
							for (int L = 0; L < cList.size(); L++) {
								cList[L].get().setSuperCell((int)CELL_TYPE::FLUID);
							}
						}

					endLoop:

						SuperCell::setNextDiv(c, (int)RandomNumberGenerators::rNormalDouble(funcTrophectoderm(m - diffStartMCS), SD_T_DIV_TARGET));

						grid->fullTextureRefresh();
						grid->fullPerimeterRefresh();

					}

				}
				else if (SuperCell::getCellType(c) == CELL_TYPE::ICM) {

					if (SuperCell::getMCS(c) >= SuperCell::getNextDiv(c)) {

						cout << "Division: " << c << " at " << SuperCell::getMCS(c) << endl;
						int newSuper = grid->divideCellShortAxis(c);

						grid->fullTextureRefresh();
						grid->fullPerimeterRefresh();

						SuperCell::setNextDiv(c, (int)RandomNumberGenerators::rNormalDouble(MCS_I_DIV_TARGET, SD_I_DIV_TARGET));
						SuperCell::setNextDiv(newSuper, (int)RandomNumberGenerators::rNormalDouble(MCS_I_DIV_TARGET, SD_I_DIV_TARGET));


					}

				}

			}

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

	}

	return 0;

}

shared_ptr<SquareCellGrid> initializeGrid(string imgName) {

	int targetInitCellLength = (int)BORDER_CONST * sqrt(TARGET_INIT_CELLS);
	int newSuperCell = SuperCell::makeNewSuperCell(CELL_TYPE::GENERIC, 0, TARGET_INIT_CELLS, targetInitCellLength);

	SuperCell::setNextDiv(newSuperCell, (int)RandomNumberGenerators::rNormalDouble(MCS_M_DIV_TARGET, SD_M_DIV_TARGET));

	std::ifstream ifs(imgName);
	string pgmString;
	getline(ifs, pgmString);
	getline(ifs, pgmString);
	int SIM_WIDTH = stoi(pgmString);
	getline(ifs, pgmString);
	int SIM_HEIGHT = stoi(pgmString);
	getline(ifs, pgmString);

	shared_ptr<SquareCellGrid> grid (new SquareCellGrid(SIM_WIDTH, SIM_HEIGHT));

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
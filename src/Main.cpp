#include <random>
#include <chrono>
#include <thread>
#include <iostream>
#include <atomic>
#include <thread>

#include <boost/program_options.hpp>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "./headers/Cell.h"
#include "./headers/SquareCellGrid.h"
#include "./headers/Vector2D.h"
#include "./headers/RandomNumberGenerators.h"

int PIXEL_SCALE;
int MAX_ITERATIONS;
int SIM_WIDTH;
int SIM_HEIGHT;
int SIM_DELAY;
int RENDER_FPS;

const float pDiv = 0.01f;

namespace po = boost::program_options;
using namespace std;

int simLoop(SquareCellGrid& grid, atomic<bool>& done);
int simInit(int argc, char* argv[]);
int printGrid(SDL_Renderer* renderer, SDL_Texture* texture, SquareCellGrid& grid);

int main(int argc, char* argv[]) {

	if (simInit(argc, argv) > 0) {
		return 1;
	}

	SDL_SetMainReady();
	SDL_Event event;
	SDL_Renderer* renderer;
	SDL_Window* window;

	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer((SIM_WIDTH + 2) * PIXEL_SCALE, (SIM_HEIGHT + 2) * PIXEL_SCALE, 0, &window, &renderer);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);

	SuperCell::makeNewSuperCell(CELL_TYPE::BOUNDARY, 0, 0);
	SuperCell::setColour((int)CELL_TYPE::BOUNDARY, 255, 255, 255, 255);
	SuperCell::makeNewSuperCell(CELL_TYPE::EMPTYSPACE, 0, 0);
	SuperCell::setColour((int)CELL_TYPE::EMPTYSPACE, 0, 0, 0, 255);
	SuperCell::makeNewSuperCell(CELL_TYPE::FLUID, 0, 0);
	SuperCell::setColour((int)CELL_TYPE::FLUID, 50, 0, 0, 255);

	SquareCellGrid grid(SIM_WIDTH, SIM_HEIGHT);
	
	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, grid.boundaryWidth, grid.boundaryHeight);
	printGrid(renderer, texture, grid);

	std::atomic<bool> done(false);
	std::thread simLoopThread(simLoop, std::ref(grid), std::ref(done));
	
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

					cout << "fps: " << (float)1000 / tickDelta << "\n";
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

int simLoop(SquareCellGrid& grid, atomic<bool>& done) {

	int midX = SIM_WIDTH / 2;
	int midY = SIM_HEIGHT / 2;

	grid.setCell(midX, midY, SuperCell::makeNewSuperCell(CELL_TYPE::GENERIC, 0, 1600));

	for (int i = 0; i < MAX_ITERATIONS; i++) {

		int x = RandomNumberGenerators::rUnifInt(1, grid.interiorWidth);
		int y = RandomNumberGenerators::rUnifInt(1, grid.interiorWidth);

		bool success = grid.moveCell(x, y);

		Cell& c = grid.getCell(x, y);

		if (c.getType() == CELL_TYPE::GENERIC && c.getGeneration() < 4) {

			float divProb = pDiv * ((float)c.getVolume() / pow((float)c.getTargetVolume(),2));

			if (RandomNumberGenerators::rUnifProb() <= divProb) {

				grid.cleaveCell(x, y);

			}

		}

		if (done) {
			break;
		}

		if (SIM_DELAY != 0 && i % 1000 == 0) std::this_thread::sleep_for(std::chrono::milliseconds(SIM_DELAY));

	}

	done = true;
	
	return 0;

}

int simInit(int argc, char* argv[]) {

	try {

		po::options_description description("Simulation options:");
		description.add_options()
			("help", "Display this help message")
			("maxI,i", po::value<int>()->default_value(10000000), "Maximum iterations")
			("pixel,p", po::value<int>()->default_value(10), "Pixels per cell")
			("height,h", po::value<int>()->default_value(75), "Simulation space height")
			("width,w", po::value<int>()->default_value(75), "Simulation space width")
			("delay,d", po::value<int>()->default_value(0), "Simulation artificial delay, arbitrary, play around for good values, zero for as fast as possible")
			("fps,f", po::value<int>()->default_value(24), "Simulation target fps, default 24");

		po::variables_map vm;
		po::store(po::command_line_parser(argc, argv).options(description).allow_unregistered().run(), vm);
		po::notify(vm);

		if (vm.count("help")) {
			std::cout << description;
			return 1;
		}

		if (vm.count("maxI")) {
			MAX_ITERATIONS = vm["maxI"].as<int>();
		}

		if (vm.count("pixel")) {
			PIXEL_SCALE = vm["pixel"].as<int>();
		}

		if (vm.count("height")) {
			SIM_HEIGHT = vm["height"].as<int>();
		}

		if (vm.count("width")) {
			SIM_WIDTH = vm["width"].as<int>();
		}

		if (vm.count("delay")) {
			SIM_DELAY = vm["delay"].as<int>();
		}

		if (vm.count("fps")) {
			RENDER_FPS = vm["fps"].as<int>();
		}

		return 0;

	}
	catch (const std::exception& e) {
		cout << "Unexpected argument" << endl;
		cout << "use --help for help" << endl;
		cout << e.what();
		return 1;
	}

}

int printGrid(SDL_Renderer* renderer, SDL_Texture* texture, SquareCellGrid& grid) {

	grid.fullTextureRefresh();

	std::vector<unsigned char> pixels = grid.getPixels();

	SDL_UpdateTexture(texture, NULL, pixels.data(), grid.boundaryWidth * 4);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);

	return 0;

}
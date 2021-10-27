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

int pixel_scale = 8;
int MAX_ITERATIONS;
int SIM_WIDTH;
int SIM_HEIGHT;

const float pDiv = 0.01f;
const float pMove = 0.01f;

using namespace std;
namespace po = boost::program_options;


int simLoop(SquareCellGrid& grid, atomic<bool>& done);
int simInit(int argc, char* argv[]);

int main(int argc, char* argv[]) {

	if (simInit(argc, argv)) {
		return 1;
	}

	SDL_SetMainReady();
	SDL_Event event;
	SDL_Renderer* renderer;
	SDL_Window* window;

	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer((SIM_WIDTH+2)*pixel_scale, (SIM_HEIGHT + 2) * pixel_scale, 0, &window, &renderer);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);

	SuperCell::makeNewSuperCell(CELL_TYPE::BOUNDARY, 0, 0);
	SuperCell::setColour((int)CELL_TYPE::BOUNDARY, 255, 255, 255, 255);
	SuperCell::makeNewSuperCell(CELL_TYPE::EMPTYSPACE, 0, 0);
	SuperCell::setColour((int)CELL_TYPE::EMPTYSPACE, 0, 0, 0, 255);

	SquareCellGrid grid(SIM_WIDTH, SIM_HEIGHT);

	int midX = SIM_WIDTH / 2;
	int midY = SIM_HEIGHT / 2;
	
	grid.setCell(midX, midY, SuperCell::makeNewSuperCell(CELL_TYPE::GENERIC, 0, 100));

	grid.printGrid(renderer, pixel_scale);

	std::atomic<bool> done(false);
	std::thread simLoopThread(simLoop, std::ref(grid), std::ref(done));
		
	while (1) {

		if (done) {
			done = false;
			simLoopThread.join();
			grid.printGrid(renderer, pixel_scale);
			cout << "done";
		}

		if (SDL_PollEvent(&event) && event.type == SDL_QUIT) break;
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

int simLoop(SquareCellGrid& grid, atomic<bool>& done) {

	int midX = SIM_WIDTH / 2;
	int midY = SIM_HEIGHT / 2;

	grid.setCell(midX, midY, SuperCell::makeNewSuperCell(CELL_TYPE::GENERIC, 0, 100));

	//grid.printGrid(renderer, pixel_scale);

	for (int i = 0; i < MAX_ITERATIONS; i++) {

		int x = RandomNumberGenerators::rUnifInt(1, grid.interiorWidth);
		int y = RandomNumberGenerators::rUnifInt(1, grid.interiorWidth);

		bool success = grid.moveCell(x, y);

		Cell& c = grid.getCell(x, y);

		if (c.getType() == CELL_TYPE::GENERIC && c.getGeneration() < 4) {

			float divProb = pDiv * (float)c.getVolume() / (float)c.getTargetVolume();

			if (RandomNumberGenerators::rUnifProb() <= divProb) {

				grid.divideCell(x, y);

			}

		}


		/*if (success) {
			grid.printGrid(renderer, pixel_scale);
		}*/

	}


	done = true;
	return 0;

}

int simInit(int argc, char* argv[]) {

	po::options_description description("Simulation options:");
	description.add_options()
		("help", "Display this help message")
		("maxI,i", po::value<int>()->default_value(1000000), "Maximum iterations")
		("pixel,p", po::value<int>()->default_value(10), "Pixels per cell")
		("height,h", po::value<int>()->default_value(75), "Simulation space height")
		("width,w", po::value<int>()->default_value(75), "Simulation space width");

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(description).run(), vm);
	po::notify(vm);

	if (vm.count("help")) {
		std::cout << description;
		return 1;
	}

	if (vm.count("maxI")) {
		MAX_ITERATIONS = vm["maxI"].as<int>();
	}

	if (vm.count("pixel")) {
		pixel_scale = vm["pixel"].as<int>();
	}

	if (vm.count("height")) {
		SIM_HEIGHT = vm["height"].as<int>();
	}

	if (vm.count("width")) {
		SIM_WIDTH = vm["width"].as<int>();
	}

	return 0;

}
#include <random>
#include <chrono>
#include <thread>
#include <iostream>

#include <boost/program_options.hpp>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "./headers/Cell.h"
#include "./headers/SquareCellGrid.h"
#include "./headers/Vector2D.h"
#include "./headers/RandomNumberGenerators.h"

const int pixel_scale = 8;

int SIM_SPEED;
int MAX_ITERATIONS;

int SIM_WIDTH;
int SIM_HEIGHT;

const float pDiv = 0.01f;
const float pMove = 0.01f;

using namespace std;
namespace po = boost::program_options;

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

	grid.getCell(SIM_WIDTH / 2, SIM_HEIGHT / 2) = Cell(CELL_TYPE::GENERIC, 20);

	grid.printGrid(renderer, pixel_scale);

	for (int i = 0; i < MAX_ITERATIONS; i++) {

		for (int x = 1; x <= grid.interiorWidth; x++) {

			for (int y = 1; y <= grid.interiorHeight; y++) {

				Cell c = grid.getCell(x, y);

				if (c.getType() == CELL_TYPE::GENERIC) {

					if (RandomNumberGenerators::rUnifProb() <= pMove) {

						grid.moveCell(x, y);

					}

					if (RandomNumberGenerators::rUnifProb() <= pDiv && c.getGeneration() < 4) {

						grid.divideCell(x, y);

					}

				}

			}

		}

		if (i % SIM_SPEED == 0) {
			grid.printGrid(renderer, pixel_scale);
		}

		cout << i << endl;

	}

	grid.printGrid(renderer, pixel_scale);

	while (1) {
		if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
			break;
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

int simInit(int argc, char* argv[]) {

	po::options_description description("Simulation options:");
	description.add_options()
		("help", "Display this help message")
		("maxI,i", po::value<int>()->default_value(1000), "Maximum iterations")
		("speed,s", po::value<int>()->default_value(1), "Display speed")
		("height,h", po::value<int>()->default_value(100), "Simulation space height")
		("width,w", po::value<int>()->default_value(100), "Simulation space width");

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

	if (vm.count("speed")) {
		SIM_SPEED = vm["speed"].as<int>();
	}

	if (vm.count("height")) {
		SIM_HEIGHT = vm["height"].as<int>();
	}

	if (vm.count("width")) {
		SIM_WIDTH = vm["width"].as<int>();
	}

	return 0;

}
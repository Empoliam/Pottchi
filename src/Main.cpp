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

const float pDiv = 0.01f;
const float pMove = 0.01f;

using namespace std;
namespace po = boost::program_options;


int simLoop(SquareCellGrid& grid, atomic<bool>& done);
int simInit(int argc, char* argv[]);
int printGrid(SDL_Renderer* renderer, SquareCellGrid& gridRef);

int main(int argc, char* argv[]) {

	if (simInit(argc, argv) > 0) {
		return 1;
	}

	SDL_SetMainReady();
	SDL_Event event;
	SDL_Renderer* renderer;
	SDL_Window* window;

	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer((SIM_WIDTH+2)*PIXEL_SCALE, (SIM_HEIGHT + 2) * PIXEL_SCALE, 0, &window, &renderer);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);

	SuperCell::makeNewSuperCell(CELL_TYPE::BOUNDARY, 0, 0);
	SuperCell::setColour((int)CELL_TYPE::BOUNDARY, 255, 255, 255, 255);
	SuperCell::makeNewSuperCell(CELL_TYPE::EMPTYSPACE, 0, 0);
	SuperCell::setColour((int)CELL_TYPE::EMPTYSPACE, 0, 0, 0, 255);

	SquareCellGrid grid(SIM_WIDTH, SIM_HEIGHT);

	std::atomic<bool> done(false);
	std::thread simLoopThread(simLoop, std::ref(grid), std::ref(done));
	
	printGrid(renderer, grid);

	bool quit = false;

	while (!quit) {

		printGrid(renderer, grid);

		if (done) {
			done = false;
			printGrid(renderer, grid);
			simLoopThread.join();
			cout << "done";
		}

		if (SDL_PollEvent(&event) && event.type == SDL_QUIT) {
			done = true;
			quit = true;
		}

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

		if (done) {
			break;
		}

		//std::this_thread::sleep_for(std::chrono::nanoseconds(SIM_DELAY));

	}


	done = true;
	return 0;

}

int simInit(int argc, char* argv[]) {

	try {

		po::options_description description("Simulation options:");
		description.add_options()
			("help", "Display this help message")
			("maxI,i", po::value<int>()->default_value(1000000), "Maximum iterations")
			("pixel,p", po::value<int>()->default_value(10), "Pixels per cell")
			("height,h", po::value<int>()->default_value(75), "Simulation space height")
			("width,w", po::value<int>()->default_value(75), "Simulation space width")
			("delay,d", po::value<int>()->default_value(0), "Simulation artificial delay, ns, for visual purposes");

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

		return 0;

	}
	catch (const std::exception& e){
		cout << "uh oh fucky wucky" << endl;
		cout << e.what();
		return 1;
	}

}

int printGrid(SDL_Renderer* renderer, SquareCellGrid& gridRef) {

	const std::vector<std::vector<std::vector<int>>> internalGridRef = gridRef.getColourGrid();

	for (int y = 0; y < (int) internalGridRef[0].size(); y++) {
		for (int x = 0; x < (int) internalGridRef.size(); x++) {

			vector<int> colour = internalGridRef[x][y];

			if (colour.size() < 4) {
				colour = { 0,0,0,0 };
			}

			SDL_Rect rect = { x * PIXEL_SCALE, y * PIXEL_SCALE, PIXEL_SCALE, PIXEL_SCALE };

			SDL_SetRenderDrawColor(renderer, colour[0], colour[1], colour[2], colour[3]);
			SDL_RenderFillRect(renderer, &rect);

		}

	}

	SDL_RenderPresent(renderer);

	return 0;

}
#include <random>
#include <chrono>
#include <thread>
#include <iostream>

#include <curses.h>

#include "./headers/Cell.h"
#include "./headers/SquareCellGrid.h"
#include "./headers/Vector2D.h"
#include "./headers/RandomNumberGenerators.h"

const int width = 30;
const int height = 30;

const float iMax = 1000;
const float pDiv = 0.01f;
const float pMove = 0.1f;

using namespace std;

int main() {

	initscr();
	resize_term(height+2,width+2);

	if (has_colors() == FALSE) {
		endwin();
		cout << "No colour support";
		exit(1);
	}

	start_color();
	init_pair(0, COLOR_WHITE, COLOR_BLACK);
	init_pair(1, COLOR_WHITE, COLOR_RED);
	init_pair(2, COLOR_WHITE, COLOR_YELLOW);
	init_pair(3, COLOR_WHITE, COLOR_GREEN);
	init_pair(4, COLOR_WHITE, COLOR_BLUE);

	SquareCellGrid grid(width, height);

	grid.getCell(width / 2, height / 2).setType(CELL_TYPE::GENERIC);

	grid.printGrid();
	getch();

	for (int i = 0; i < iMax; i++) {

		for (int x = 1; x <= grid.interiorWidth; x++) {

			for (int y = 1; y < grid.interiorHeight; y++) {

				Cell c = grid.getCell(x, y);
				if (c.getType() == CELL_TYPE::GENERIC && c.getGeneration() < 4) {
					
					if (RandomNumberGenerators::rUnifProb() <= pMove) {

						grid.moveCell(x,y);

					}

					if (RandomNumberGenerators::rUnifProb() <= pDiv) {

						grid.divideCell(x, y);
												
					}

				}

			}

		}

		grid.printGrid();
		
		if (i % 1 == 0) {
			this_thread::sleep_for(chrono::milliseconds(1000 / 60));
		}
	}
	
	getch();
	endwin();

	return 0;
}
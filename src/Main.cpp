#include <atomic>
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
#include <mutex>

#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdint>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window.hpp>

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
#include "./headers/DivisionHandler.h"
#include "./headers/TransformHandler.h"
#include "./headers/ReportHandler.h"
#include "./headers/CellDeathHandler.h"
#include "./headers/CellDeathEvent.h"

#include "../lib/cxxopts.hpp"

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
bool HEADLESS = true;

// Thread safety stuff
std::mutex mData;
std::mutex mNext;
std::mutex mLowPriority;

void highPriorityLock() {
	mNext.lock();
	mData.lock();
	mNext.unlock();
}

void highPriorityUnlock() {
	mData.unlock();
}

void lowPriorityLock() {
	mLowPriority.lock();
	mNext.lock();
	mData.lock();
	mNext.unlock();
}

void lowPriorityUnlock() {
	mData.unlock();
	mLowPriority.unlock();
}

std::string logName;

int simLoop(std::shared_ptr<SquareCellGrid> grid, std::atomic<bool> &done);
unsigned int readConfig(std::string cfg);
std::shared_ptr<SquareCellGrid> initializeGrid(std::string imgName);

std::map<int, int> templateColourMap;

int main(int argc, char *argv[]) {

	cxxopts::Options options("Pottchi", "CPM Software");

	options.add_options()
		("h,headless", "Run in headless mode")
		("f,file","File name to load", cxxopts::value<std::string>()->default_value("default"));
	
	auto result = options.parse(argc, argv);
	std::string loadName = result["f"].as<std::string>();
	HEADLESS = result["h"].as<bool>();

	#ifdef SSH_HEADLESS
	HEADLESS = true;
	#endif

	std::cout << "Loading: " << loadName << std::endl;
	int configStatus = readConfig(loadName + ".cfg");
	std::cout << "Done loading" << std::endl;

	// TODO sort this out
	if (configStatus) {
		std::cout << "Missing configuration option: " << configStatus << std::endl;
		return 1;
	}

	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);

	std::string fileName;

	// Random number to add to filename to avoid conflicts
	int randName = RandomNumberGenerators::rUnifInt(0,1000);

	// Try extra hard to avoid conflicts
	int attempt = 0;
	do {

		std::ostringstream oss;
		oss << std::put_time(&tm, "%Y-%m-%d %H-%M-%S") << "-" << randName << attempt;
		fileName = oss.str();

		++attempt;

	} while (std::filesystem::exists(fileName));

	std::ofstream temp(fileName);
	logName = fileName+".log";

	// Initialize grid and event handlers
	std::shared_ptr<SquareCellGrid> grid = initializeGrid(IMAGE_NAME + ".pgm");
	DivisionHandler::initializeHandler(grid);
	TransformHandler::initializeHandler(grid);
	ReportHandler::initializeHandler(grid);
	CellDeathHandler::initializeHandler(grid);

	#ifndef SSH_HEADLESS
	// Texture to render simulation to
	sf::Texture gridTexture;
	gridTexture.create(grid->boundaryWidth, grid->boundaryHeight);
	sf::Sprite sprite(gridTexture);
	sprite.setScale(PIXEL_SCALE, PIXEL_SCALE);
	

	// Grid render method
	auto refreshGridTexture = [&] {
		highPriorityLock();
		grid->fullTextureRefresh();
		highPriorityUnlock();
		uint8_t *pixels = grid->getPixels().data();
		gridTexture.update(pixels);
	};

	#endif

	// RUN WITH GUI

	if (!HEADLESS) {

		#ifndef SSH_HEADLESS
		// Start simulation loop
		std::atomic<bool> done(false);
		std::thread simLoopThread(simLoop, grid, std::ref(done));

		bool quit = AUTO_QUIT;

		// Initialize window
		sf::RenderWindow window(sf::VideoMode(PIXEL_SCALE * grid->boundaryWidth, PIXEL_SCALE * grid->boundaryHeight), "Pottchi");
		window.setFramerateLimit(RENDER_FPS);

		// Graphics loop
		while (window.isOpen()) {

			// Draw to window
			refreshGridTexture();
			window.draw(sprite);
			window.display();

			sf::Event event;
			while (window.pollEvent(event)) {
				if (event.type == sf::Event::Closed) {
					done = true;
					quit = true;
				}
			}

			if (done) {

				if (static bool doOnce; !std::exchange(doOnce, true)) {
					std::cout << "Simulation finished\n";
				}

				if (quit) {
					simLoopThread.join();
					window.close();
				}
			}
		}

		#endif

	} else {

		std::cout << "Headless Launch\n";

		std::atomic<bool> done(false);
		simLoop(grid, std::ref(done));
		
	}

	#ifndef SSH_HEADLESS
	refreshGridTexture();
	gridTexture.copyToImage().saveToFile(fileName + ".png");
	#endif

	//Clean up temporary file
	remove(fileName.c_str());

	return 0;
}

int simLoop(std::shared_ptr<SquareCellGrid> grid, std::atomic<bool> &done) {

	std::ofstream logFile(logName, std::ofstream::out);

	// Number of samples to take before increasing MCS count
	unsigned int iMCS = grid->interiorWidth * grid->interiorHeight;

	// Simulation loop
	for (unsigned int m = 0; m < MAX_MCS; m++) {

		if(!HEADLESS) {
			lowPriorityLock();
		}

		if (done) {
			lowPriorityUnlock();
			break;			
		}

		// Monte Carlo Step
		for (unsigned int i = 0; i < iMCS; i++) {

			int x = RandomNumberGenerators::rUnifInt(1, grid->interiorWidth);
			int y = RandomNumberGenerators::rUnifInt(1, grid->interiorHeight);

			bool success = grid->moveCell(x, y);
		}

		CellDeathHandler::runDeathLoop(m);

		// Cell division
		DivisionHandler::runDivisionLoop();

		// Transform Events
		TransformHandler::runTransformLoop();

		if(!HEADLESS) {
			lowPriorityUnlock();
		}

		// Reporting
		ReportHandler::runReportLoop(m, logFile);

		// Artificial delay if desired
		/*
		if (SIM_DELAY != 0)
			std::this_thread::sleep_for(std::chrono::milliseconds(SIM_DELAY));
		*/

		// Increase MCS count for each cell
		SuperCell::increaseMCS();

		// Update event timers
		TransformEvent::updateTimers();
	}

	//Ensure Mutex unlock
	lowPriorityUnlock();

	logFile.close();

	done = true;

	return 0;
}

unsigned int readConfig(std::string cfg) {

	std::ifstream ifs(cfg);
	std::string line;

	int lineNumber = 0;

	while (std::getline(ifs, line)) {

		lineNumber++;

		auto V = split(line, ',');

		if (V.empty() || V[0][0] == '#') {

			continue;

		} else if (V[0] == "SIM_PARAM") {

			std::string P = V[1];
			std::string value = V[2];

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
				std::string P = V[0];

				if (P == "J") {
					std::vector<std::string> J = split(V[1], ':');
					for (std::string S : J) {
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

				std::string c = V[0];

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

				std::string c = V[0];

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

				std::string c = V[0];

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

				std::string c = V[0];

				if (c == "TIME")
					R.triggerOn = (int)(stod(V[1]) * MCS_HOUR_EST);
				else if (c == "TYPE")
					R.type = stoi(V[1]);
				else if (c == "REPEAT")
					R.doRepeat = (V[1] == "1");
				else if (c == "DATA") {
					std::vector<std::string> dat = split(V[1], ':');
					for (std::string S : dat) {
						R.data.push_back(stoi(S));
					}
				} else if (c == "TEXT")
					R.reportText = V[1];
				else if (c != "END_REPORT")
					std::cout << "Unknown report config on line " << lineNumber << std::endl;
			}

			ReportEvent::addNewEvent(R);
		}

		else if (V[0] == "DEATH_DEFINE") {

			CellDeathEvent D(stoi(V[1]));

			while (line != "END_DEATH") {

				std::getline(ifs, line);
				lineNumber++;

				V = split(line, ',');

				std::string c = V[0];

				if (c == "TIME")
					D.fireOn = (int)(stod(V[1]) * MCS_HOUR_EST);
				else if (c == "TYPE")
					D.type = stoi(V[1]);
				else if (c == "TARGET_TYPE")
					D.targetType = stoi(V[1]);
				else if (c == "DATA") {
					std::vector<std::string> dat = split(V[1], ':');
					for (std::string S : dat) {
						D.data.push_back(stod(S));
					}
				}
				else if (c != "END_DEATH")
					std::cout << "Unknown report config on line " << lineNumber << std::endl;

			}

			CellDeathEvent::AddNewEvent(D);
		}


		else {

			std::cout << "Unrecognised tag " << V[0] << " on line " << lineNumber << std::endl;
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

std::shared_ptr<SquareCellGrid> initializeGrid(std::string imgName) {

	std::ifstream ifs(imgName);
	std::string pgmString;
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

	std::shared_ptr<SquareCellGrid> grid(new SquareCellGrid(SIM_WIDTH, SIM_HEIGHT, boundarySuper, spaceSuper));

	for (int y = 1; y <= grid->interiorHeight; y++) {
		for (int x = 1; x <= grid->interiorWidth; x++) {

			uint8_t b = 0;
			ifs >> b;

			try {
				int setSC = tempSuperMap[b];
				grid->setCell(x, y, setSC);
			} catch (std::out_of_range) {
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

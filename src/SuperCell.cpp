#include "headers/SuperCell.h"

#include<vector>
#include<algorithm>

#include "headers/RandomNumberGenerators.h"

using namespace std;

static vector<SuperCell> superCells = vector<SuperCell>();
static int idCounter = 0;

SuperCell::SuperCell(int type, int generation, int targetVolume, int targetSurface) {

	this->ID = idCounter;
	this->cellType = type;
	this->generation = generation;
	this->targetVolume = targetVolume;
	this->targetSurface = targetSurface;

	this->colour = generateNewColour(type);

}

int SuperCell::makeNewSuperCell(int type, int gen, int targetV, int targetSurface) {


	SuperCell sc = SuperCell(type, gen, targetV, targetSurface);
	superCells.push_back(sc);

	std::sort(superCells.begin(), superCells.end(), [](const SuperCell& lhs, const SuperCell& rhs) {
		return lhs.ID < rhs.ID;
	});

	return idCounter++;

}

int SuperCell::makeNewSuperCell(int sC) {
	return SuperCell::makeNewSuperCell(SuperCell::getCellType(sC), SuperCell::getGeneration(sC), SuperCell::getTargetVolume(sC), SuperCell::getTargetSurface(sC));
}

int SuperCell::getID(int i) {
	return superCells[i].ID;
}

bool SuperCell::isStatic(int c){
	return CellType::getType(superCells[c].cellType).isStatic;
}

bool SuperCell::doDivide(int c) {
	return CellType::getType(superCells[c].cellType).doesDivide;
}

bool SuperCell::ignoreVolume(int c) {
	return CellType::getType(superCells[c].cellType).ignoreVolume;
}

bool SuperCell::ignoreSurface(int c) {
	return CellType::getType(superCells[c].cellType).ignoreSurface;
}

double SuperCell::getDivMean(int c) {
	return CellType::getType(superCells[c].cellType).divideMean;
}

double SuperCell::getDivSD(int c) {
	return CellType::getType(superCells[c].cellType).divideSD;
}

int SuperCell::getDivType(int c) {
	return CellType::getType(superCells[c].cellType).divideType;
}

int SuperCell::getDivMinVol(int c) {
	return CellType::getType(superCells[c].cellType).divMinVolume;
}

int SuperCell::getGeneration(int i) {
	return superCells[i].generation;
}

void SuperCell::increaseGeneration(int i) {
	superCells[i].generation++;
}

void SuperCell::setGeneration(int i, int gen) {
	superCells[i].generation = gen;
}

int SuperCell::getMCS(int c) {
	return superCells[c].lastDivMCS;
}

void SuperCell::setMCS(int c, int i) {
	superCells[c].lastDivMCS = i;
}

void SuperCell::increaseMCS() {
	for (int x = 0; x < superCells.size(); x++) {
		superCells[x].lastDivMCS++;
	}
}

int SuperCell::getNextDiv(int c) {
	return superCells[c].nextDivMCS;
}

void SuperCell::setNextDiv(int c, int i) {
	superCells[c].nextDivMCS = i;
}

int SuperCell::getTargetVolume(int c) {
	return superCells[c].targetVolume;
}

void SuperCell::setTargetVolume(int i, int target) {

	superCells[i].targetVolume = target;

}

int SuperCell::getCellType(int c) {
	return superCells[c].cellType;
}

void SuperCell::setCellType(int c, int t) {
	superCells[c].cellType = t;
}

std::vector<double>& SuperCell::getJ(int c) {
	return CellType::getType(superCells[c].cellType).J;
}

int SuperCell::getCounter() {
	return idCounter;
}

void SuperCell::setColour(int i, int r, int g, int b, int a) {
	SuperCell& C = superCells[i];
	C.colour[0] = b;
	C.colour[1] = g;
	C.colour[2] = r;
	C.colour[3] = a;

}

void SuperCell::setColour(int i, std::vector<int> col) {
	superCells[i].colour = col;
}

std::vector<int> SuperCell::getColour(int i) {
	return superCells[i].colour;
}

//TODO Update to new colour generation method
std::vector<int> SuperCell::generateNewColour(int c) {

	std::vector<int> newCol = std::vector<int>(4, 0);

	if (c == 3 || c == 4) {

		int r = RandomNumberGenerators::rUnifInt(150, 255);
		int gb = RandomNumberGenerators::rUnifInt(0, 75);

		newCol[0] = r;
		newCol[1] = gb;
		newCol[2] = gb;
		newCol[3] = 255;
	}
	else if (c == 5) {

		int b = RandomNumberGenerators::rUnifInt(150, 255);
		int rg = RandomNumberGenerators::rUnifInt(0, 75);

		newCol[0] = rg;
		newCol[1] = rg;
		newCol[2] = b;
		newCol[3] = 255;
	}
	else if (c == 6) {

		int g = RandomNumberGenerators::rUnifInt(150, 255);
		int rb = RandomNumberGenerators::rUnifInt(0, 75);

		newCol[0] = rb;
		newCol[1] = g;
		newCol[2] = rb;
		newCol[3] = 255;
	}
	else {
		newCol[0] = RandomNumberGenerators::rUnifInt(0, 255);
		newCol[1] = RandomNumberGenerators::rUnifInt(0, 255);
		newCol[2] = RandomNumberGenerators::rUnifInt(0, 255);
		newCol[3] = 255;
	}

	return newCol;

}

int SuperCell::generateNewDivisionTime(int c) {
	return (int) RandomNumberGenerators::rNormalDouble(SuperCell::getDivMean(c), SuperCell::getDivSD(c));
}

void SuperCell::changeVolume(int i, int delta) {
	superCells[i].volume += delta;
}

void SuperCell::setVolume(int i, int v) {
	superCells[i].volume = v;
}

int SuperCell::getVolume(int i) {
	return superCells[i].volume;
}

void SuperCell::setTargetSurface(int c, int t) {

	superCells[c].targetSurface = t;

}

int SuperCell::getTargetSurface(int c) {
	return superCells[c].targetSurface;
}

void SuperCell::setSurface(int c, int l) {

	superCells[c].surface = l;

}

int SuperCell::getSurface(int c) {
	return superCells[c].surface;
}

void SuperCell::changeSurface(int c, int delta) {

	superCells[c].surface += delta;

}

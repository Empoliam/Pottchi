#include "headers/SuperCell.h"

#include<vector>

#include "headers/RandomNumberGenerators.h"

using namespace std;

static vector<SuperCell> superCells = vector<SuperCell>();
static int idCounter = 0;

SuperCell::SuperCell(CELL_TYPE type, int generation, int targetVolume, int targetSurface) {
	
	this->ID = idCounter;
	this->type = type;
	this->generation = generation;
	this->targetVolume = targetVolume;
	this->targetSurface = targetSurface;

	this->colour = generateNewColour(type);

}

int SuperCell::makeNewSuperCell(CELL_TYPE type, int gen, int targetV, int targetSurface) {


	SuperCell sc = SuperCell(type, gen, targetV, targetSurface);
	superCells.push_back(sc);

	return idCounter++;

}

int SuperCell::makeNewSuperCell(Cell& c)
{
	int cSuper = c.getSuperCell();
	return SuperCell::makeNewSuperCell(SuperCell::getCellType(cSuper), SuperCell::getGeneration(cSuper), SuperCell::getTargetVolume(cSuper), SuperCell::getTargetSurface(cSuper));
}

int SuperCell::getID(int i) {
	return superCells[i].ID;
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

CELL_TYPE SuperCell::getCellType(int c) {
	return superCells[c].type;
}

void SuperCell::setCellType(int c, CELL_TYPE t) {
	superCells[c].type = t;
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

std::vector<int> SuperCell::generateNewColour(CELL_TYPE c) {

	std::vector<int> newCol = std::vector<int>(4, 0);

	if (c == CELL_TYPE::GENERIC) {

		int r = RandomNumberGenerators::rUnifInt(150, 255);
		int gb = RandomNumberGenerators::rUnifInt(0, 75);

		newCol[0] = r;
		newCol[1] = gb;
		newCol[2] = gb;
		newCol[3] = 255;
	}
	else if (c == CELL_TYPE::TROPHECTODERM) {

		int b = RandomNumberGenerators::rUnifInt(150, 255);
		int rg = RandomNumberGenerators::rUnifInt(0, 75);

		newCol[0] = rg;
		newCol[1] = rg;
		newCol[2] = b;
		newCol[3] = 255;
	}
	else if (c == CELL_TYPE::ICM) {

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

void SuperCell::increaseSurface(int c, int delta) {

	superCells[c].surface += delta;

}

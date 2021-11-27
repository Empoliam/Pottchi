#include "headers/SuperCell.h"

#include<vector>

#include "headers/RandomNumberGenerators.h"

using namespace std;

static vector<SuperCell> superCells = vector<SuperCell>();
static int idCounter = 0;

SuperCell::SuperCell(CELL_TYPE type, int generation, int targetVolume) {
	
	this->ID = idCounter;
	this->type = type;
	this->generation = generation;
	this->targetVolume = targetVolume;

	if (type == CELL_TYPE::GENERIC) {

		int r = RandomNumberGenerators::rUnifInt(150, 255);
		int gb = RandomNumberGenerators::rUnifInt(0, 100);

		this->colour[0] = r;
		this->colour[1] = gb;
		this->colour[2] = gb;
		this->colour[3] = 255;
	} else {
		this->colour[0] = RandomNumberGenerators::rUnifInt(0, 255);
		this->colour[1] = RandomNumberGenerators::rUnifInt(0, 255);
		this->colour[2] = RandomNumberGenerators::rUnifInt(0, 255);
		this->colour[3] = 255;
	}

}

int SuperCell::makeNewSuperCell(CELL_TYPE type, int gen, int targetV) {


	SuperCell sc = SuperCell(type, gen, targetV);
	superCells.push_back(sc);


	return idCounter++;

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

std::vector<int> SuperCell::getColour(int i) {
	return superCells[i].colour;
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

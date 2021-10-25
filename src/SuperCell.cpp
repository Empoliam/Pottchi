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

	this->colour[0] = RandomNumberGenerators::rUnifInt(0, 255);
	this->colour[1] = RandomNumberGenerators::rUnifInt(0, 255);
	this->colour[2] = RandomNumberGenerators::rUnifInt(0, 255);
	this->colour[3] = 255;

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

int SuperCell::getTargetVolume(int i) {
	return superCells[i].targetVolume;
}

CELL_TYPE SuperCell::getCellType(int i)
{
	return superCells[i].type;
}

int SuperCell::getCounter()
{
	return idCounter;
}


void SuperCell::setColour(int i, int r, int g, int b, int a) {
	SuperCell& C = superCells[i];
	C.colour[0] = r;
	C.colour[1] = g;
	C.colour[2] = b;
	C.colour[3] = a;

}

std::vector<int> SuperCell::getColour(int i) {
	return superCells[i].colour;
}

void SuperCell::changeVolume(int i, int delta) {
	superCells[i].volume += delta;
}

int SuperCell::getVolume(int i) {
	return superCells[i].volume;
}

#include "headers/SuperCell.h"

#include<vector>

using namespace std;

static vector<SuperCell> superCells = vector<SuperCell>();
static int idCounter = 0;

SuperCell::SuperCell(CELL_TYPE type, int generation, int targetVolume) {
	
	this->ID = idCounter;
	this->type = type;
	this->generation = generation;
	this->targetVolume = targetVolume;

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

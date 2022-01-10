#include "headers/SuperCell.h"

#include<vector>
#include<algorithm>
#include<iostream>

#include "headers/RandomNumberGenerators.h"
#include "headers/ColourScheme.h"

using namespace std;

static vector<SuperCell> superCells = vector<SuperCell>();

SuperCell::SuperCell(int type, int generation, int targetVolume, int targetSurface) {

	this->ID = superCells.size();
	this->cellType = type;
	this->generation = generation;
	this->targetVolume = targetVolume;
	this->targetSurface = targetSurface;

}

int SuperCell::makeNewSuperCell(int type, int gen, int targetV, int targetSurface) {

	SuperCell sc = SuperCell(type, gen, targetV, targetSurface);
	superCells.push_back(sc);

	std::sort(superCells.begin(), superCells.end(), [](const SuperCell& lhs, const SuperCell& rhs) {
		return lhs.ID < rhs.ID;
	});

	return superCells.size() - 1;

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

int SuperCell::getNumSupers() {
	return superCells.size();
}

int SuperCell::getColourScheme(int c) {
	return CellType::getType(superCells[c].cellType).colourScheme;
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
void SuperCell::generateNewColour(int c) {

	setColour(c, ColourScheme::generateColour(getColourScheme(c)));

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

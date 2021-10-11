#include "Cell.h"

using namespace std;

Cell::Cell(CELL_TYPE type, int generation) {
	this->type = type;
	this->generation = generation;
}

Cell::Cell(CELL_TYPE type) : Cell(type, 0){}

Cell::Cell() : Cell(CELL_TYPE::EMPTYSPACE) {}

CELL_TYPE Cell::getType() const {
	return this->type;
}

void Cell::setType(CELL_TYPE type) {
	this->type = type;
}

int Cell::getGeneration() const {
	return generation;
}

void Cell::setGeneration(int g) {
	generation = g;
}

void Cell::increaseGeneration() {
	generation++;
}

string Cell::toString() const {

	switch (type)
	{
	case CELL_TYPE::EMPTYSPACE:
		return ".";
		break;
	case CELL_TYPE::GENERIC:
		return "C";
		break;
	case CELL_TYPE::BOUNDARY:
		return "#";
		break;
	default:
		return "?";
		break;
	}

}


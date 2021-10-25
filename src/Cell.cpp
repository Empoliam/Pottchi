#include "./headers/Cell.h"

#include "./headers/RandomNumberGenerators.h"

Cell::Cell(int superCell) {
	this->superCell = superCell;
}

Cell::Cell() : Cell((int)CELL_TYPE::EMPTYSPACE) {}

Cell::Cell(CELL_TYPE t, int targetVolume) {

	superCell =  SuperCell::makeNewSuperCell(t, 0, targetVolume);

}

int Cell::getSuperCell() const {
	return superCell;
}

void Cell::setSuperCell(int i) {
	this->superCell = i;
}

CELL_TYPE Cell::getType() const {
	return SuperCell::getCellType(superCell);
}


int Cell::getGeneration() const {
	return SuperCell::getGeneration(superCell);
}

void Cell::increaseGeneration() {
	SuperCell::increaseGeneration(superCell);
}

void Cell::setGeneration(int gen) {
	SuperCell::setGeneration(superCell, gen);
}

int Cell::getTargetVolume() const {
	return SuperCell::getTargetVolume(superCell);
}

int Cell::getVolume() const{
	return SuperCell::getVolume(superCell);
}

std::vector<int> Cell::getColour() const {
	return SuperCell::getColour(superCell);
}

char Cell::toChar() const {

	CELL_TYPE type = SuperCell::getCellType(superCell);

	switch (type)
	{
	case CELL_TYPE::EMPTYSPACE:
		return '.';
		break;
	case CELL_TYPE::GENERIC:
		return (char) (superCell+63);
		break;
	case CELL_TYPE::BOUNDARY:
		return '#';
		break;
	default:
		return '?';
		break;
	}

}

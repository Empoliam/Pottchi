#include "Cell.h"

using namespace std;

Cell::Cell(CELL_TYPE type) {
	this->type = type;
}

Cell::Cell() {
	this->type = CELL_TYPE::EMPTYSPACE;
}

CELL_TYPE Cell::getType() const
{
	return this->type;
}

void Cell::setType(CELL_TYPE type) {
	this->type = type;
}

string Cell::toString() const
{

	switch (type)
	{
	case CELL_TYPE::EMPTYSPACE:
		return ".";
		break;
	case CELL_TYPE::GENERIC:
		return "G";
		break;
	case CELL_TYPE::BOUNDARY:
		return "B";
		break;
	default:
		return "#";
		break;
	}

}


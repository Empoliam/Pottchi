#include "./headers/CellType.h"

static std::vector<CellType> CellTypes = std::vector<CellType>();

void CellType::addType(CellType T) {
	CellTypes.push_back(T);
}

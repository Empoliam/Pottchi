#include "./headers/CellType.h"
#include <algorithm>

static std::vector<CellType> cellTypes = std::vector<CellType>();

CellType::CellType(int id) {
	this->id = id;
	this->J = std::vector<double>(0.0);
};

void CellType::addType(CellType T) {

	cellTypes.push_back(T);
	
	std::sort(cellTypes.begin(), cellTypes.end(), [](const CellType& lhs, const CellType& rhs) {
		return lhs.id < rhs.id;
		});

}

CellType& CellType::getType(int t) {
	return cellTypes[t];
}
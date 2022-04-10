#include "./headers/CellType.h"
#include <algorithm>

static std::vector<CellType> cellTypes = std::vector<CellType>();

/**
 * @brief Construct a new CellType::CellType object
 * 
 * @param id  ID to assign to new type
 */
CellType::CellType(int id) {
	this->id = id;
	this->J = std::vector<double>(0.0);
};

/**
 * @brief Add a CellType object to the list of cell types.
 * 
 * @param T Cell Type to add
 */
void CellType::addType(CellType T) {

	cellTypes.push_back(T);
	
	std::sort(cellTypes.begin(), cellTypes.end(), [](const CellType& lhs, const CellType& rhs) {
		return lhs.id < rhs.id;
		});

}

/**
 * @brief Get the type with the target ID from the cell type list
 * 
 * @param t ID of type to fetch
 * @return reference to requested cell type
 */
CellType& CellType::getType(int t) {
	return cellTypes[t];
}
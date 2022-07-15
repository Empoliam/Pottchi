#include "./headers/SquareCellGrid.h"

#include <algorithm>
#include <iostream>
#include <math.h>
#include <random>
#include <vector>

#include "./headers/MathConstants.h"
#include "./headers/RandomNumberGenerators.h"
#include "./headers/SuperCell.h"

std::vector<std::vector<int>> internalGrid;
std::vector<uint8_t> pixels;

SquareCellGrid::SquareCellGrid(int w, int h, int boundarySC, int spaceSC) {

	internalGrid = std::vector<std::vector<int>>(w + 2, std::vector<int>(h + 2, spaceSC));
	pixels = std::vector<uint8_t>((w + 2) * (h + 2) * 4);

	// Temporary initialization
	BOLTZ_TEMP = 0.0;
	OMEGA = 0.0;
	LAMBDA = 0.0;

	interiorWidth = w;
	interiorHeight = h;
	boundaryWidth = w + 2;
	boundaryHeight = h + 2;

	for (int x = 0; x < boundaryWidth; x++) {
		internalGrid[x][0] = boundarySC;
		internalGrid[x][boundaryHeight - 1] = boundarySC;
	};

	for (int y = 0; y < boundaryHeight; y++) {
		internalGrid[0][y] = boundarySC;
		internalGrid[boundaryWidth - 1][y] = boundarySC;
	};

	SuperCell::setVolume(spaceSC, interiorWidth * interiorHeight);
	SuperCell::setVolume(boundarySC, (boundaryWidth * boundaryHeight) - (interiorWidth * interiorHeight));
}

std::vector<Vector2D<int>> SquareCellGrid::getNeighboursCoords(int row, int col) {
	std::vector<Vector2D<int>> neighbours;

	neighbours.reserve(8);

	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {

			if (x == 0 && y == 0)
				continue;
			neighbours.push_back(Vector2D<int>(row + x, col + y));
		}
	}
	return neighbours;
}

std::vector<int> SquareCellGrid::getNeighboursSuperCells(int row, int col) {

	std::vector<int> neighbours;

	neighbours.reserve(8);

	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {

			if (x == 0 && y == 0)
				continue;
			neighbours.push_back(internalGrid[row + x][col + y]);
		}
	}
	return neighbours;
}

std::vector<int> SquareCellGrid::getNeighboursTypes(int row, int col) {

	std::vector<int> neighbours;

	neighbours.reserve(8);

	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {

			if (x == 0 && y == 0)
				continue;

			neighbours.push_back(SuperCell::getCellType(internalGrid[row + x][col + y]));
		}
	}
	return neighbours;
}

std::vector<Vector2D<int>> SquareCellGrid::getNeighboursCoords(int row, int col, int type) {

	std::vector<Vector2D<int>> neighbours;

	neighbours.reserve(8);

	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {

			if (x == 0 && y == 0)
				continue;
			if (SuperCell::getCellType(internalGrid[row + x][col + y]) == type)
				neighbours.push_back(Vector2D<int>(row + x, col + y));
		}
	}
	return neighbours;
}

int SquareCellGrid::divideCell(int c) {

	int minX = interiorWidth;
	int minY = interiorHeight;
	int maxX = 1;
	int maxY = 1;

	std::vector<Vector2D<int>> cellList;
	std::vector<Vector2D<int>> newList;

	for (int X = 1; X <= interiorWidth; X++) {
		for (int Y = 1; Y <= interiorHeight; Y++) {

			if (internalGrid[X][Y] == c) {

				cellList.push_back(Vector2D<int>(X, Y));

				if (X < minX)
					minX = X;
				if (X > maxX)
					maxX = X;
				if (Y < minY)
					minY = Y;
				if (Y > maxY)
					maxY = Y;
			}
		}
	}

	if (cellList.size() <= 1) {
		return 1;
	}

	int splitAxis;
	int midPoint;

	if ((maxX - minX) > (maxY - minY)) {
		splitAxis = 0;
		midPoint = (maxX + minX) / 2;
	} else {
		splitAxis = 1;
		midPoint = (maxY + minY) / 2;
	}

	for (unsigned int k = 0; k < cellList.size(); k++) {
		if (cellList[k][splitAxis] < midPoint) {
			newList.push_back(cellList[k]);
		}
	}

	SuperCell::increaseGeneration(c);
	int newSuperCell = SuperCell::makeNewSuperCell(c);

	SuperCell::setMCS(c, 0);
	SuperCell::setMCS(newSuperCell, 0);

	for (unsigned int c = 0; c < newList.size(); c++) {
		Vector2D<int> &V = newList[c];
		setCell(V[0], V[1], newSuperCell);
	}

	return newSuperCell;
}

int SquareCellGrid::divideCellRandomAxis(int c) {

	int minX = interiorWidth;
	int minY = interiorHeight;
	int maxX = 1;
	int maxY = 1;

	std::vector<Vector2D<int>> cellList;
	std::vector<Vector2D<int>> newList;

	for (int X = 1; X <= interiorWidth; X++) {
		for (int Y = 1; Y <= interiorHeight; Y++) {

			if (internalGrid[X][Y] == c) {

				cellList.push_back(Vector2D<int>(X, Y));

				if (X < minX)
					minX = X;
				if (X > maxX)
					maxX = X;
				if (Y < minY)
					minY = Y;
				if (Y > maxY)
					maxY = Y;
			}
		}
	}

	if (cellList.size() <= 1) {
		return -1;
	}

	int midX = (int)(0.5 * (minX + maxX));
	int midY = (int)(0.5 * (minY + maxY));

	int gradM = RandomNumberGenerators::rUnifInt(-89, 89);
	double grad = tan(gradM * PI_D / 180.f);

	for (unsigned int k = 0; k < cellList.size(); k++) {
		if (cellList[k][1] > grad * (cellList[k][0] - midX) + midY) {
			newList.push_back(cellList[k]);
		}
	}

	SuperCell::increaseGeneration(c);
	int newSuperCell = SuperCell::makeNewSuperCell(c);

	SuperCell::setMCS(c, 0);
	SuperCell::setMCS(newSuperCell, 0);

	for (unsigned int c = 0; c < newList.size(); c++) {
		Vector2D<int> &V = newList[c];
		setCell(V[0], V[1], newSuperCell);
	}

	return newSuperCell;
}

int SquareCellGrid::divideCellShortAxis(int c) {

	std::vector<Vector2D<int>> cellList;
	std::vector<Vector2D<int>> newList;

	// Find all subcells in cell
	for (int X = 1; X <= interiorWidth; X++) {
		for (int Y = 1; Y <= interiorHeight; Y++) {

			if (internalGrid[X][Y] == c) {

				cellList.push_back(Vector2D<int>(X, Y));
			}
		}
	}

	// Abort if cell less than one subcell
	if (cellList.size() <= 1) {
		return -1;
	}

	// Calculate short axis of cell
	double m00 = calculateRawImageMoment(cellList, 0, 0);
	double m10 = calculateRawImageMoment(cellList, 1, 0);
	double m01 = calculateRawImageMoment(cellList, 0, 1);

	double xBar = m10 / m00;
	double yBar = m01 / m00;

	double mu20 = (calculateRawImageMoment(cellList, 2, 0) / m00) - pow(xBar, 2);
	double mu02 = (calculateRawImageMoment(cellList, 0, 2) / m00) - pow(yBar, 2);
	double mu11 = (calculateRawImageMoment(cellList, 1, 1) / m00) - xBar * yBar;

	double covTrace = mu20 + mu02;
	double covDet = mu20 * mu02 - pow(mu11, 2);

	double eigA = (covTrace + sqrt(pow(covTrace, 2) - 4 * covDet)) / 2;
	double eigB = (covTrace - sqrt(pow(covTrace, 2) - 4 * covDet)) / 2;

	double smallEig = std::min(abs(eigA), abs(eigB));

	Vector2D<double> eigVec(mu11, smallEig - mu20);
	double grad = eigVec[1] / eigVec[0];

	int newSuperCell = -1;
	double minRatio = SuperCell::getDivMinRatio(c);

	if (std::max(abs(eigA), abs(eigB)) / std::min(abs(eigA), abs(eigB)) > minRatio) {

		for (unsigned int k = 0; k < cellList.size(); k++) {
			if (cellList[k][1] > grad * (cellList[k][0] - xBar) + yBar) {
				newList.push_back(cellList[k]);
			}
		}

		SuperCell::increaseGeneration(c);
		newSuperCell = SuperCell::makeNewSuperCell(c);

		SuperCell::setMCS(newSuperCell, 0);

		for (unsigned int c = 0; c < newList.size(); c++) {
			Vector2D<int> &V = newList[c];
			setCell(V[0], V[1], newSuperCell);
		}
	}

	SuperCell::setMCS(c, 0);
	return newSuperCell;
}

double SquareCellGrid::calculateRawImageMoment(std::vector<Vector2D<int>> data, int iO, int jO) {

	double moment = 0.0f;

	for (Vector2D<int> V : data) {

		moment += pow(V[0], iO) * pow(V[1], jO);
	}

	return moment;
}

int SquareCellGrid::cleaveCell(int c) {

	int superCellA = c;
	int superCellB = divideCellShortAxis(c);

	if (superCellB == -1)
		return -1;

	int newTargetVolume = SuperCell::getTargetVolume(c) / 2;

	SuperCell::setTargetVolume(superCellA, newTargetVolume);
	SuperCell::setTargetVolume(superCellB, newTargetVolume);

	return superCellB;
}

int SquareCellGrid::moveCell(int x, int y) {

	std::vector<Vector2D<int>> neighbours = getNeighboursCoords(x, y);

	int r = RandomNumberGenerators::rUnifInt(0, (int)neighbours.size() - 1);

	int targetX = neighbours[r][0];
	int targetY = neighbours[r][1];

	int origin = internalGrid[x][y];
	int target = internalGrid[targetX][targetY];

	if (!SuperCell::isStatic(target) &&
		!SuperCell::isStatic(origin) &&
		target != internalGrid[x][y] &&
		!SuperCell::isDead(origin)) {

		double deltaH = 0;

		if (SuperCell::isDead(target)) {
			deltaH = 0;
		} else {
			deltaH = getAdhesionDelta(x, y, targetX, targetY) * OMEGA + getVolumeDelta(x, y, targetX, targetY) * LAMBDA;
		}
		if (deltaH <= 0 || (RandomNumberGenerators::rUnifProb() < exp(-deltaH / BOLTZ_TEMP))) {
			setCell(targetX, targetY, internalGrid[x][y]);

			return 1;
		}
	}

	return 0;
}

int SquareCellGrid::getCell(int row, int col) {
	return internalGrid[row][col];
}

void SquareCellGrid::setCell(int x, int y, int superCell) {

	int originalSuper = internalGrid[x][y];

	// Volume Change
	SuperCell::changeVolume(originalSuper, -1);
	SuperCell::changeVolume(superCell, 1);

	internalGrid[x][y] = superCell;
}

double SquareCellGrid::getAdhesionDelta(int sourceX, int sourceY, int destX, int destY) {

	int sourceSuper = internalGrid[sourceX][sourceY];
	int destSuper = internalGrid[destX][destY];

	std::vector<double> &sourceJ = SuperCell::getJ(sourceSuper);
	std::vector<double> &destJ = SuperCell::getJ(destSuper);

	double initH = 0.0f;
	double postH = 0.0f;

	auto neighbours = getNeighboursCoords(destX, destY);

	for (int i = 0; i < 8; i++) {

		int nSuper = internalGrid[neighbours[i][0]][neighbours[i][1]];
		int nType = SuperCell::getCellType(nSuper);

		initH += destJ[nType] * (nSuper != destSuper);
		postH += sourceJ[nType] * (nSuper != sourceSuper);
	}

	return (postH - initH);
}

double SquareCellGrid::getVolumeDelta(int sourceX, int sourceY, int destX, int destY) {

	int destSuper = internalGrid[destX][destY];

	// Prevent destruction of cells
	if (SuperCell::getVolume(destSuper) - 1 == 0)
		return 1000000.0f;

	int sourceSuper = internalGrid[sourceX][sourceY];

	int sourceVol = SuperCell::getVolume(sourceSuper);
	int destVol = SuperCell::getVolume(destSuper);

	int sourceTarget = SuperCell::getTargetVolume(sourceSuper);
	int destTarget = SuperCell::getTargetVolume(destSuper);

	double deltaH = 0.0f;

	// Prevent medium volume from affecting energy
	bool sourceIgnore = SuperCell::ignoreVolume(sourceSuper);
	bool destIgnore = SuperCell::ignoreVolume(destSuper);

	deltaH = (!sourceIgnore) * ((double)pow(sourceVol + 1 - sourceTarget, 2) - (double)pow(sourceVol - sourceTarget, 2)) + (!destIgnore) * ((double)pow(destVol - 1 - destTarget, 2) - (double)pow(destVol - destTarget, 2));

	return deltaH;
}

void SquareCellGrid::fullTextureRefresh() {

	for (int x = 0; x < boundaryWidth; x++) {

		for (int y = 0; y < boundaryHeight; y++) {

			const unsigned int pixOffset = (boundaryWidth * 4 * y) + x * 4;
			std::vector<int> colourIn = SuperCell::getColour(internalGrid[x][y]);

			if (colourIn.size() == 0) {
				colourIn = {0, 0, 0, 0};
			}

			pixels[pixOffset + 0] = (uint8_t)colourIn[0];
			pixels[pixOffset + 1] = (uint8_t)colourIn[1];
			pixels[pixOffset + 2] = (uint8_t)colourIn[2];
			pixels[pixOffset + 3] = (uint8_t)255;
		}
	}
}

std::vector<uint8_t> SquareCellGrid::getPixels() {

	return pixels;
}

#include "./headers/SquareCellGrid.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <random>
#include <math.h>

#include "./headers/RandomNumberGenerators.h"

using namespace std;

const float  PI_F = 3.14159265358979f;

const float BOLTZ_TEMP = 10.0f;

//Volume Constraint Strength
const float LAMBDA = 5.0f;

//Surface constraint strength
const float SIGMA = 0.0f;

const auto J = CellTypes::J;

SquareCellGrid::SquareCellGrid(int w, int h) : internalGrid(w + 2, std::vector<Cell>(h + 2)), pixels((w+2) * (h+2) * 4, 0) {

	interiorWidth = w;
	interiorHeight = h;
	boundaryWidth = w + 2;
	boundaryHeight = h + 2;

	for (int x = 0; x < boundaryWidth; x++) {
		internalGrid[x][0] = Cell((int)CELL_TYPE::BOUNDARY);
		internalGrid[x][boundaryHeight - 1] = Cell((int)CELL_TYPE::BOUNDARY);
	};

	for (int y = 0; y < boundaryHeight; y++) {
		internalGrid[0][y] = Cell((int)CELL_TYPE::BOUNDARY);
		internalGrid[boundaryWidth - 1][y] = Cell((int)CELL_TYPE::BOUNDARY);
	};
	
	SuperCell::setVolume(1, interiorWidth * interiorHeight);
	SuperCell::setVolume(0, (boundaryWidth*boundaryHeight) - (interiorWidth * interiorHeight));

}

vector<Cell*> SquareCellGrid::getNeighbours(int row, int col)
{

	vector<Cell*> neighbours;

	neighbours.reserve(8);

	for (int x = -1; x <= 1; x++) {

		for (int y = -1; y <= 1; y++) {

			if (x == 0 && y == 0) continue;
			neighbours.push_back(&(internalGrid[row + x][col + y]));

		}

	}

	return neighbours;

}

std::vector<Cell*> SquareCellGrid::getNeighbours(int row, int col, CELL_TYPE t) {
	
	vector<Cell*> neighbours = getNeighbours(row, col);

	neighbours.erase(std::remove_if(
		neighbours.begin(),
		neighbours.end(),
		[t](const Cell* c) { return c->getType() != t; })
		, neighbours.end()
	);

	return neighbours;

}

vector<Vector2D<int>> SquareCellGrid::getNeighboursCoords(int row, int col)
{
	vector<Vector2D<int>> neighbours;

	neighbours.reserve(8);

	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {

			if (x == 0 && y == 0) continue;
			neighbours.push_back(Vector2D<int>(row + x, col + y));

		}
	}
	return neighbours;
}

std::vector<Vector2D<int>> SquareCellGrid::getNeighboursCoords(int row, int col, CELL_TYPE t) {
	
	vector<Vector2D<int>> neighbours;

	neighbours.reserve(8);

	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {

			if (x == 0 && y == 0) continue;
			if (internalGrid[row - 1][col - 1].getType() == t) neighbours.push_back(Vector2D<int>(row + x, col + y));
			
		}
	}
	return neighbours;

}

bool SquareCellGrid::checkSurface(int row, int col) {

	return checkSurface(row, col, internalGrid[row][col].getSuperCell());

}

bool SquareCellGrid::checkSurface(int row, int col, int sC) {

	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
			if (x == 0 && y == 0) continue;
			if (sC != internalGrid[row + x][col + y].getSuperCell()) return true;
		}
	}

	return false;
}

int SquareCellGrid::getPerimeter(int row, int col){

	return getPerimeter(row, col, internalGrid[row][col].getSuperCell());
}

int SquareCellGrid::getPerimeter(int row, int col, int sC) {

	int perimeter = 0;

	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
			if (x == 0 && y == 0) continue;
			if (sC != internalGrid[row + x][col + y].getSuperCell()) perimeter++;
		}
	}

	return perimeter;
}

int SquareCellGrid::divideCell(int c) {

	int minX = interiorWidth;
	int minY = interiorHeight;
	int maxX = 1;
	int maxY = 1;

	vector<Vector2D<int>> cellList;
	vector<Vector2D<int>> newList;

	for (int X = 1; X <= interiorWidth; X++) {
		for (int Y = 1; Y <= interiorHeight; Y++) {

			if (internalGrid[X][Y].getSuperCell() == c) {

				cellList.push_back(Vector2D<int>(X, Y));

				if (X < minX) minX = X;
				if (X > maxX) maxX = X;
				if (Y < minY) minY = Y;
				if (Y > maxY) maxY = Y;

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
	}
	else {
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
		Vector2D<int>& V = newList[c];
		setCell(V[0], V[1], newSuperCell);
	}

	return newSuperCell;

}

int SquareCellGrid::divideCellRandomAxis(int c) {

	int minX = interiorWidth;
	int minY = interiorHeight;
	int maxX = 1;
	int maxY = 1;

	vector<Vector2D<int>> cellList;
	vector<Vector2D<int>> newList;

	for (int X = 1; X <= interiorWidth; X++) {
		for (int Y = 1; Y <= interiorHeight; Y++) {

			if (internalGrid[X][Y].getSuperCell() == c) {

				cellList.push_back(Vector2D<int>(X, Y));

				if (X < minX) minX = X;
				if (X > maxX) maxX = X;
				if (Y < minY) minY = Y;
				if (Y > maxY) maxY = Y;

			}

		}
	}

	if (cellList.size() <= 1) {
		return -1;
	}

	int midX = (int)(0.5 * (minX + maxX));
	int midY = (int)(0.5 * (minY + maxY));

	int gradM = RandomNumberGenerators::rUnifInt(-89, 89);
	float grad = tan(gradM*PI_F/180.f);

	cout << gradM << endl;

	for (unsigned int k = 0; k < cellList.size(); k++) {
		if (cellList[k][1] > grad*(cellList[k][0]-midX)+midY) {
			newList.push_back(cellList[k]);
		}
	}

	SuperCell::increaseGeneration(c);
	int newSuperCell = SuperCell::makeNewSuperCell(c);

	SuperCell::setMCS(c, 0);
	SuperCell::setMCS(newSuperCell, 0);

	for (unsigned int c = 0; c < newList.size(); c++) {
		Vector2D<int>& V = newList[c];
		setCell(V[0], V[1], newSuperCell);
	}

	return newSuperCell;

}

int SquareCellGrid::cleaveCell(int c) {

	int superCellA = c;
	int superCellB = divideCell(c);

	if (superCellB == -1) return -1;

	int newTargetVolume = SuperCell::getTargetVolume(c) / 2;
	int newTargetSurface = (int)2 * sqrt(3.14159 * newTargetVolume);

	SuperCell::setTargetVolume(superCellA, newTargetVolume);
	SuperCell::setTargetVolume(superCellB, newTargetVolume);

	SuperCell::setTargetSurface(superCellA, newTargetSurface);
	SuperCell::setTargetSurface(superCellB, newTargetSurface);

	return superCellB;
}

int SquareCellGrid::moveCell(int x, int y) {

	vector<Vector2D<int>> neighbours = getNeighboursCoords(x, y);

	int r = RandomNumberGenerators::rUnifInt(0, (int)neighbours.size() - 1);

	int targetX = neighbours[r][0];
	int targetY = neighbours[r][1];

	Cell& origin = internalGrid[x][y];
	Cell& swap = internalGrid[neighbours[r][0]][neighbours[r][1]];

	if (swap.getType() != CELL_TYPE::BOUNDARY &&
		swap.getSuperCell() != internalGrid[x][y].getSuperCell()) {

		float deltaH = getAdhesionDelta(x, y, targetX, targetY) + getVolumeDelta(x, y, targetX, targetY); //+ getSurfaceDelta(x, y, targetX, targetY);

		if (deltaH <= 0 || (RandomNumberGenerators::rUnifProb() < exp(-deltaH / BOLTZ_TEMP))) {
			setCell(targetX, targetY, internalGrid[x][y].getSuperCell());
			
			localTextureRefresh(x, y);
			localTextureRefresh(targetX, targetY);

			return 1;	
		}

	}

	return 0;

}

void SquareCellGrid::recalculateCellSurfaces() {

	for (int s = 0; s < SuperCell::getCounter(); s++) {

		SuperCell::setSurface(s, 0);

	}

	for (int x = 1; x <= interiorWidth; x++) {

		for (int y = 1; y <= interiorHeight; y++) {

			Cell& c = internalGrid[x][y];

			if (c.getType() == CELL_TYPE::EMPTYSPACE || c.getType() == CELL_TYPE::FLUID) continue;

			c.increaseSurface(getPerimeter(x,y));
						

		}

	}

}

Cell& SquareCellGrid::getCell(int row, int col) {
	return internalGrid[row][col];
}

void SquareCellGrid::setCell(int x, int y, int superCell) {

	int originalSuper = internalGrid[x][y].getSuperCell();

	//Volume Change
	SuperCell::changeVolume(originalSuper, -1);
	SuperCell::changeVolume(superCell, 1);

	//Surface Change
	Vector2D<int> surfaceChanges = calcSurfaceChange(x, y, superCell);
	SuperCell::changeSurface(superCell, surfaceChanges[0]);
	SuperCell::changeSurface(originalSuper, surfaceChanges[1]);

	internalGrid[x][y].setSuperCell(superCell);

}

Vector2D<int> SquareCellGrid::calcSurfaceChange(int row, int col, int superCell){
	
	int superSource = superCell;
	int superDest = internalGrid[row][col].getSuperCell();

	auto N = getNeighboursCoords(row, col);
	std::vector<Vector2D<int>> Nsource;
	std::vector<Vector2D<int>> Ndest;

	for (Vector2D<int> n : N) {

		int c = internalGrid[n[0]][n[1]].getSuperCell();
		if (c == superSource) {
			Nsource.push_back(n);
		}
		else if (c == superDest) {
			Ndest.push_back(n);
		}

	}

	//Calculate local boundary size before:

	int sourcePre = 0;
	int destPre = 0;

	for (Vector2D<int> n : Nsource) {
		sourcePre += getPerimeter(n[0], n[1]);
	}
	for (Vector2D<int> n : Ndest) {
		destPre += getPerimeter(n[0], n[1]);
	}

	//Calculate local boundary size after:

	int sourcePost = 0;
	int destPost = 0;

	for (Vector2D<int> n : Nsource) {
		sourcePost += getPerimeter(n[0], n[1], superSource);
	}
	for (Vector2D<int> n : Ndest) {
		destPost += checkSurface(n[0], n[1], superSource);
	}

	int deltaSource = sourcePost - sourcePre;
	int deltaDest = destPost - destPre;

	return Vector2D<int>(deltaSource, deltaDest);

}

float SquareCellGrid::getAdhesionDelta(int sourceX, int sourceY, int destX, int destY) {

	Cell& source = internalGrid[sourceX][sourceY];
	Cell& dest = internalGrid[destX][destY];

	int sourceSuper = source.getSuperCell();
	int destSuper = dest.getSuperCell();

	float initH = 0.0f;
	float postH = 0.0f;

	vector<Cell*> neighbours = getNeighbours(destX, destY);
	for (int i = 0; i < 8; i++) {

		int nSuper = neighbours[i]->getSuperCell();

		initH += J[(int)dest.getType()][(int)neighbours[i]->getType()] * (nSuper != destSuper);
		postH += J[(int)source.getType()][(int)neighbours[i]->getType()] * (nSuper != sourceSuper);

	}

	return (postH - initH);
}

float SquareCellGrid::getVolumeDelta(int sourceX, int sourceY, int destX, int destY) {

	int destSuper = internalGrid[destX][destY].getSuperCell();

	//Prevent destruction of cells
	if (SuperCell::getVolume(destSuper) - 1 == 0) return 1000000.0f;

	int sourceSuper = internalGrid[sourceX][sourceY].getSuperCell();

	int sourceVol = SuperCell::getVolume(sourceSuper);
	int destVol = SuperCell::getVolume(destSuper);

	int sourceTarget = SuperCell::getTargetVolume(sourceSuper);
	int destTarget = SuperCell::getTargetVolume(destSuper);

	float deltaH = 0.0f;

	//Prevent medium volume from affecting energy
	bool sourceIgnore = sourceSuper == (int)CELL_TYPE::EMPTYSPACE;
	bool destIgnore = destSuper == (int)CELL_TYPE::EMPTYSPACE;

	deltaH = (!sourceIgnore)*((float)pow(sourceVol + 1 - sourceTarget, 2) - (float)pow(sourceVol - sourceTarget, 2))
			+ (!destIgnore)*((float)pow(destVol - 1 - destTarget, 2) - (float)pow(destVol - destTarget, 2));

	deltaH *= LAMBDA;

	return deltaH;
}

float SquareCellGrid::getSurfaceDelta(int sourceX, int sourceY, int destX, int destY) {
	
	int superSource = internalGrid[sourceX][sourceY].getSuperCell();
	int superDest = internalGrid[destX][destY].getSuperCell();

	Vector2D<int> changes = calcSurfaceChange(destX, destY, superSource);

	int deltaSource = changes[0];
	int deltaDest = changes[1];

	int sourceSurf = SuperCell::getSurface(superSource);
	int sourceTarget = SuperCell::getTargetSurface(superSource);

	int destSurf = SuperCell::getSurface(superDest);
	int destTarget = SuperCell::getTargetSurface(superDest);

	//Ignore surface of fluid/medium
	bool ignoreSource = (superSource == (int)CELL_TYPE::EMPTYSPACE) || (superSource == (int)CELL_TYPE::FLUID);
	bool ignoreDest = (superDest == (int)CELL_TYPE::EMPTYSPACE) || (superDest == (int)CELL_TYPE::FLUID);

	float deltaH = (!ignoreSource) * ((float)pow(sourceSurf + deltaSource - sourceTarget, 2) - (float)pow(sourceSurf - sourceTarget, 2))
					+ (!ignoreDest) * ((float)pow(destSurf + deltaDest  - destTarget, 2) - (float)pow(destSurf - destTarget, 2));

	return SIGMA * deltaH;

}

void SquareCellGrid::fullTextureRefresh() {

	for (int x = 0; x < boundaryWidth; x++) {

		for (int y = 0; y < boundaryHeight; y++) {

			const unsigned int pixOffset = (boundaryWidth * 4 * y) + x * 4;
			vector<int> colourIn = internalGrid[x][y].getColour();

			if (colourIn.size() == 0) {
				colourIn = { 0,0,0,0 };
			}

			pixels[pixOffset + 3] = (char) colourIn[0];
			pixels[pixOffset + 2] = (char) colourIn[1];
			pixels[pixOffset + 1] = (char) colourIn[2];
			pixels[pixOffset + 0] = SDL_ALPHA_OPAQUE;

		}

	}

}

void SquareCellGrid::localTextureRefresh(int x, int y) {

	const unsigned int pixOffset = (boundaryWidth * 4 * y) + x * 4;
	vector<int> colourIn = internalGrid[x][y].getColour();

	if (colourIn.size() == 0) {
		colourIn = { 0,0,0,0 };
	}

	pixels[pixOffset + 3] = (char)colourIn[0];
	pixels[pixOffset + 2] = (char)colourIn[1];
	pixels[pixOffset + 1] = (char)colourIn[2];
	pixels[pixOffset + 0] = SDL_ALPHA_OPAQUE;

}

std::vector<unsigned char> SquareCellGrid::getPixels() {

	return pixels;

}

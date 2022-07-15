#pragma once

#include "CellType.h"
#include "SuperCellTemplate.h"
#include <map>
#include <string>
#include <vector>

class SuperCell {

public:
	static int makeNewSuperCell(int type, int gen, int targetV);
	static int makeNewSuperCell(int sC);
	static int makeNewSuperCell(SuperCellTemplate &T);

	static int getID(int i);

	static bool isStatic(int c);
	static bool doDivide(int c);
	static bool ignoreVolume(int c);
	static bool ignoreSurface(int c);

	static double getDivMean(int c);
	static double getDivSD(int c);
	static int getDivType(int c);
	static int getDivMinVol(int c);
	static int getDivMinRatio(int c);

	static int getGeneration(int i);
	static void increaseGeneration(int i);
	static void setGeneration(int i, int gen);

	static int getMCS(int c);
	static void setMCS(int c, int i);
	static void increaseMCS();
	static int getNextDiv(int c);
	static void setNextDiv(int c, int i);

	static int getTargetVolume(int c);
	static void setTargetVolume(int i, int target);

	static void changeVolume(int i, int delta);
	static void setVolume(int i, int v);
	static int getVolume(int i);

	static int getCellType(int c);
	static void setCellType(int c, int t);

	static bool isCountable(int c);

	static std::vector<double> &getJ(int c);

	static int getNumSupers();

	static int getColourScheme(int c);
	static void setColour(int i, int r, int g, int b, int a);
	static void setColour(int i, std::vector<int> col);
	static std::vector<int> getColour(int i);
	static void generateNewColour(int c);

	static int generateNewDivisionTime(int c);

	static bool isDead(int c);
	static void setDead(int c, bool d);

private:
	int ID;
	int generation;

	int cellType = 0;

	int targetVolume;
	int volume = 0;

	int lastDivMCS = 0;
	int nextDivMCS = 9999999;

	bool dead = false;

	SuperCell(int type, int generation, int targetVolume);

	std::vector<int> colour = std::vector<int>(4, 255);
};

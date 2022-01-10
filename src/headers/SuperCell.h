#pragma once

#include "CellTypes.h"
#include "CellType.h"
#include <vector>
#include <string>
#include <map>

class SuperCell {

public:
	
	static int makeNewSuperCell(int type, int gen, int targetV, int targetSurface);
	static int makeNewSuperCell(int sC);

	static int getID(int i);

	static bool isStatic(int c);
	static bool doDivide(int c);
	static bool ignoreVolume(int c);
	static bool ignoreSurface(int c);

	static double getDivMean(int c);
	static double getDivSD(int c);
	static int getDivType(int c);
	static int getDivMinVol(int c);

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
	
	static void setTargetSurface(int c, int t);
	static int getTargetSurface(int c);

	static void setSurface(int c, int l);
	static int getSurface(int c);
	static void changeSurface(int c, int delta);

	static int getCellType(int c);
	static void setCellType(int c, int t);

	static std::vector<double>& getJ(int c);

	static int getCounter();

	static void setColour(int i, int r, int g, int b, int a);
	static void setColour(int i, std::vector<int> col);
	static std::vector<int> getColour(int i);
	static std::vector<int> generateNewColour(int c);
	
	static int generateNewDivisionTime(int c);

private:

	int ID;
	int generation;

	int cellType = 0;

	int targetVolume;
	int volume = 0;
	
	int targetSurface;
	int surface = 0;

	int lastDivMCS = 0;
	int nextDivMCS = 9999999;
		
	SuperCell(int type, int generation, int targetVolume, int targetSurface);

	std::vector<int> colour = std::vector<int>(4,0);

};


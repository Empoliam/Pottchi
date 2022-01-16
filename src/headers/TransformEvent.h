#pragma once

#include <climits>

class TransformEvent {

public:

	TransformEvent(int id);

	static int addNewEvent(TransformEvent T);

	static void updateTimers();

	static TransformEvent& getEvent(int e);

	static int getNumEvents();

	void generateNewTriggerTime();
	void startTimer();

	int id;

	int mcsTimer = 0;
	

	bool triggered = false;

	int triggerMCS = INT_MAX;
	double triggerMean = 0;
	double triggerSD = 0;

	int transformFrom = 0;
	int transformTo = 0;

	int transformType = 0;
	int transformData = 0;

	bool waitForOther = false;
	int eventToWait = 0;

	bool updateColour = true;
	bool updateDiv = true;

private:

	bool timerStart = false;

};
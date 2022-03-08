#include<vector>
#include <algorithm>

#include "./headers/TransformEvent.h"
#include "./headers/RandomNumberGenerators.h"

static std::vector<TransformEvent> transformEvents = std::vector<TransformEvent>();

TransformEvent::TransformEvent(int id) {

	this->id = id;

}

void TransformEvent::generateNewTriggerTime() {

	if (triggerSD == 0) {
		triggerMCS = (int) triggerMean;
	}
	else {
		triggerMCS = (int)RandomNumberGenerators::rNormalDouble(triggerMean, triggerSD);
	}

}

void TransformEvent::startTimer() {
	mcsTimer = 0;
	timerStart = true;
	generateNewTriggerTime();
}

void TransformEvent::updateTimers() {

	for (TransformEvent& T : transformEvents) {

		if (T.timerStart) {
			T.mcsTimer++;
		}

	}

}

int TransformEvent::addNewEvent(TransformEvent T) {

	transformEvents.push_back(T);

	std::sort(transformEvents.begin(), transformEvents.end(), [](const TransformEvent& lhs, const TransformEvent& rhs) {
		return lhs.id < rhs.id;
		});

	return 0;

}

int TransformEvent::getNumEvents() {
	return transformEvents.size();
}

TransformEvent& TransformEvent::getEvent(int e) {
	return transformEvents[e];
}

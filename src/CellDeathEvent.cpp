#include "headers/CellDeathEvent.h"

#include <iostream>
#include <algorithm>

static std::vector<CellDeathEvent> deathEvents = std::vector<CellDeathEvent>();

CellDeathEvent::CellDeathEvent(int id) {
    this->id = id;
}

int CellDeathEvent::AddNewEvent(CellDeathEvent E) {

    for(CellDeathEvent C : deathEvents) {
        if(C.id == E.id) {
            std::cout << "Warning: death event with ID " << E.id << " already defined. Skipping." << std::endl;
        }
    }

    deathEvents.push_back(E);

	std::sort(deathEvents.begin(), deathEvents.end(), [](const CellDeathEvent& lhs, const CellDeathEvent& rhs) {
		return lhs.id < rhs.id;
		});

	return 0;
}

int CellDeathEvent::getNumEvents() {
	return deathEvents.size();
}

CellDeathEvent& CellDeathEvent::getEvent(int e) {
	return deathEvents[e];
}
#include <algorithm>

#include "./headers/ReportEvent.h"

static std::vector<ReportEvent> reportEvents = std::vector<ReportEvent>();

ReportEvent::ReportEvent(int id) {
	this->id = id;
}

int ReportEvent::getNumEvents() {
    return reportEvents.size();
}

ReportEvent& ReportEvent::getEvent(int r) {
    return reportEvents[r];
}

int ReportEvent::addNewEvent(ReportEvent R) {
	reportEvents.push_back(R);

	std::sort(reportEvents.begin(), reportEvents.end(), [](const ReportEvent &lhs, const ReportEvent &rhs) {
		return lhs.id < rhs.id;
	});

	return 0;
}
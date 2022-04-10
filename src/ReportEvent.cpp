#include <algorithm>

#include "./headers/ReportEvent.h"

static std::vector<ReportEvent> reportEvents = std::vector<ReportEvent>();

/**
 * @brief Construct a new Report object
 *
 * @param id ID to assign to report
 */
ReportEvent::ReportEvent(int id) {
	this->id = id;
}

/**
 * @brief Gets the number of registered events
 *
 * @return int
 */
int ReportEvent::getNumEvents() {
	return reportEvents.size();
}

/**
 * @brief Returns the report ojected with the requested ID
 *
 * @param r Requested ID
 * @return Reference to requested report
 */
ReportEvent &ReportEvent::getEvent(int r) {
	return reportEvents[r];
}

/**
 * @brief Add the provided report to the report list
 *
 * @param R Report to add
 * @return Return 0 if successful
 */
int ReportEvent::addNewEvent(ReportEvent R) {
	reportEvents.push_back(R);

	std::sort(reportEvents.begin(), reportEvents.end(), [](const ReportEvent &lhs, const ReportEvent &rhs) {
		return lhs.id < rhs.id;
	});

	return 0;
}
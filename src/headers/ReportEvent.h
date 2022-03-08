#pragma once

#include <vector>
#include <string>

class ReportEvent {

public:
    
    ReportEvent(int id);

    static int addNewEvent(ReportEvent R);

    static ReportEvent &getEvent(int e);
    static int getNumEvents();

    int id;
    int triggerOn = 0;
    int type = 0;

    std::string reportText = "Undefined Report";

    bool doRepeat = true;
    bool fired = false;
    std::vector<int> data;

};
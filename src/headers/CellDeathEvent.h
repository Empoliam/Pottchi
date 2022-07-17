#pragma once

#include <vector>

class CellDeathEvent {

    public:

    CellDeathEvent(int id);
    
    static int AddNewEvent(CellDeathEvent E);
    static int getNumEvents();   
    static CellDeathEvent& getEvent(int e); 

    int id;
    int fireOn = 0;
    int type = 0;
    int targetType = 0;

    std::vector<double> data;

};
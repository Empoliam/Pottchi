#pragma once

#include "SquareCellGrid.h"

#include <memory>

class CellDeathHandler{
public:

    static void initializeHandler(std::shared_ptr<SquareCellGrid> ptr);
    static void runDeathLoop(int m);

};
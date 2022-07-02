#pragma once

#include <memory>

#include "SquareCellGrid.h"

class DivisionHandler {
    public:

    static void initializeHandler(std::shared_ptr<SquareCellGrid> ptr);
    static void runDivisionLoop();

};
#pragma once

#include <memory>

#include "SquareCellGrid.h"

class TransformHandler {
    public:

    static void initializeHandler(std::shared_ptr<SquareCellGrid> ptr);
    static void runTransformLoop();

};
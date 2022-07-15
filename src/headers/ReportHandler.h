#pragma once

#include <memory>

#include "SquareCellGrid.h"

class ReportHandler {
    public:

    static void initializeHandler(std::shared_ptr<SquareCellGrid> ptr);
    static void runReportLoop(int m, std::ofstream& logFile);

};
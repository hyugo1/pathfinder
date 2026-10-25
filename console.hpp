#pragma once
#include "appstate.hpp"
#include "grid.hpp"
#include "algorithms.hpp"

void printPerformanceStats(const SearchResult& result, const Algorithm& chosenAlgo, double executionTime);
void printTerminalGridAndStats(Grid& copy, const SearchResult& result);
int askForAlgorithmChoice();
bool chooseThreadedMode();
int askForModeChoice();

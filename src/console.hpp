#pragma once
#include "appstate.hpp"
#include "grid.hpp"
#include "algorithms.hpp"

void printPerformanceStats(const SearchResult& result, const Algorithm& chosenAlgo, double executionTime);
void displayPerformanceStats(bool statsRequested, const Algorithm& chosenAlgo, const SearchResult& result, double executionTime);
void printTerminalGridAndStats(Grid& copy, const SearchResult& result, const Algorithm& chosenAlgo);
Algorithm askForAlgorithmChoice();
bool chooseThreadedMode();
int askForModeChoice();

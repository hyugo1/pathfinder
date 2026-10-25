#pragma once

#include "appstate.hpp"
#include "grid.hpp"
#include <string>

void runTerminalMode(AppState& appState);
std::string parseLine(const std::string& line);
Grid readFromFile(std::string filename);
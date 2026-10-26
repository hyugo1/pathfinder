#include "terminal.hpp"
#include "appstate.hpp"
#include "grid.hpp"
#include "console.hpp"
#include <string>
#include <fstream>

std::string parseLine(const std::string& line) {
    std::string result;
    for (size_t i = 0; i < line.size(); ++i) {
        if (line[i] == '\'') {
            if (i + 1 < line.size()) {
                result += line[i + 1];
                i += 2; // skip 'X'
            }
        }
    }
    return result;
}

Grid readFromFile(const std::string filename) {
    std::ifstream file(filename);
    std::string line;
    Grid grid;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '@') {
            continue;
        }
        if (line[0] == '+' || line[0] == '-') {
            line = line.substr(1);
        }
        std::string parsed = parseLine(line);
        if (!parsed.empty()) {
            std::vector<CellType> row;
            for (char c : parsed) {
                switch (c) {
                    case '#': row.push_back(CellType::Wall); break;
                    case '.': row.push_back(CellType::Open); break;
                    case 'S': row.push_back(CellType::Start); break;
                    case 'E': row.push_back(CellType::End); break;
                    case '*': row.push_back(CellType::Path); break;
                    case 'o': row.push_back(CellType::Explored); break;
                    default: row.push_back(CellType::Open);
                }
            }
            grid.push_back(row);
        }
    }

    return grid;
}

void runTerminalMode(AppState& appState) {
    appState.grid = readFromFile("grids/biggrid.txt");
    if (!validateGrid(appState.grid)) {
        return;
    }

    auto [start, end] = findStartAndEndPositions(appState.grid);
    appState.start = start;
    appState.end = end;

    Algorithm algorithmChoice {askForAlgorithmChoice()};
    
    appState.chosenAlgo = algorithmChoice;

    appState.result = runSearch(appState.grid, appState.start, appState.end, appState.chosenAlgo);
    
    Grid copy = appState.grid;
    printTerminalGridAndStats(copy, appState.result, appState.chosenAlgo);
}
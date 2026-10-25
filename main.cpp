// cmake --build build --clean-first  
// ./build/main           

#include <iostream>
#include <vector>
#include <algorithm>
#include <array>
#include <queue>
#include <fstream>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <stack>
#include <limits>
#include <chrono>
#include <optional>
#include <functional>

#include "grid.hpp"
#include "algorithms.hpp"
#include "SearchWorker.hpp"
#include "appstate.hpp"
#include "renderer.hpp"
#include "console.hpp"

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

Grid readFromFile(std::string filename) {
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

    int algorithmChoice {askForAlgorithmChoice()};
    
    appState.chosenAlgo = handleAlgorithmChoice(algorithmChoice);

    appState.result = runSearch(appState.grid, appState.start, appState.end, appState.chosenAlgo);
    
    Grid copy = appState.grid;
    printTerminalGridAndStats(copy, appState.result);
}

void runApp(AppState& appState) {
    int choice = askForModeChoice();
    if (choice == 1) {
        runTerminalMode(appState);
    } else {
        bool threadedMode = chooseThreadedMode();
        sf::RenderWindow window(
            sf::VideoMode({COLS * CELL_SIZE, ROWS * CELL_SIZE}),
            "Pathfinding Visualizer"
        );
        runGuiMode(appState, window, threadedMode);
    }
}

int main() {

    AppState appState;
    appState.grid = Grid(ROWS, std::vector<CellType>(COLS, CellType::Open));

    runApp(appState);
    return 0;
}


// fixme: add button toggle to switch between threaded and non-threaded mode in GUI.
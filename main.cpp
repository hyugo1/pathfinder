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



std::ostream& operator<<(std::ostream& os, CellType cell) {
    switch (cell) {
        case CellType::Open:  os << '.'; break;
        case CellType::Wall:  os << '#'; break;
        case CellType::Start: os << 'S'; break;
        case CellType::End:   os << 'E'; break;
        case CellType::Path:  os << '*'; break;
        case CellType::Explored: os << 'o'; break;
    }
    return os;
}

void printTerminalGridAndStats(Grid& copy, const SearchResult& result) {
    for (const Position& position : result.explorationOrder) {
        if (copy[position.first][position.second] != CellType::Start && copy[position.first][position.second] != CellType::End) {
            copy[position.first][position.second] = CellType::Explored;
        }
    }
    for (const Position& position : result.path) {
        if (copy[position.first][position.second] != CellType::Start && copy[position.first][position.second] != CellType::End) {
            copy[position.first][position.second] = CellType::Path;
        }
    }
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string BLUE = "\033[34m";
    const std::string LIGHT_BLUE = "\033[36m";
    const std::string RESET = "\033[0m";
    
    for (const auto& row : copy) {
        for (const CellType cell : row) {
            switch (cell) {
                case CellType::Explored:
                    std::cout << LIGHT_BLUE << cell << RESET;
                    break;
                case CellType::Path:
                    std::cout << RED << cell << RESET;
                    break;
                case CellType::Start:
                    std::cout << GREEN << cell << RESET;
                    break;
                case CellType::End:
                    std::cout << BLUE << cell << RESET;
                    break;
                default:
                    std::cout << cell;
                    break;
                }
            }
        std::cout << '\n';
    }
    std::cout << "Total Nodes Explored: " << result.explorationOrder.size() << ".\n";

    if (!result.path.empty()) {
        std::cout << "Path Length: " << result.path.size() << ".\n";
    } else {
        std::cout << "No path found.\n";
    }
}

void sleepForGuiUpdate(int seconds) {
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}

int askForAlgorithmChoice() {
    int algorithmChoice {};
    std::cout << "Which algorithm do you want to use? "; 
    for (int i = 1; i <= 3; ++i) {
        std::cout << i << " for " << (i == 1 ? "DFS" : (i == 2 ? "BFS" : "A*")) << ", ";
    }
    std::cout << "\nEnter your algorithmChoice: ";
    if (!(std::cin >> algorithmChoice) || algorithmChoice < 1 || algorithmChoice > 3) {
        std::cerr << "Invalid input. Please enter a number between 1 and 3.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return askForAlgorithmChoice(); // recursively ask again
    }

    return algorithmChoice;
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

void printPerformanceStats(const SearchResult& result, const Algorithm& chosenAlgo, double executionTime) {
    std::cout << "\n--- Performance Stats ---\n";
    std::cout << "Algorithm: " << algorithmName(chosenAlgo) << "\n";
    std::cout << "Execution Time: " << executionTime << " ms\n";
    std::cout << "Total Nodes Explored: " << result.explorationOrder.size() << "\n";
    std::cout << "Path Length: " << result.path.size() << "\n";
}

bool chooseThreadedMode() {
    int choice {};
    std::cout << "Choose mode: 1 for Threaded, 2 for Non-Threaded: ";
    if (!(std::cin >> choice) || choice < 1 || choice > 2) {
        std::cerr << "Invalid input. Please enter a number between 1 and 2.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return chooseThreadedMode(); // recursively ask again
    }
    if (choice == 1) {
        std::cout << "Threaded mode selected.\n";
    } else {
        std::cout << "Non-threaded mode selected.\n";
    }
    return choice == 1;
}

int askForModeChoice() {
    int choice {};
    std::cout << "Choose mode: 1 for Terminal, 2 for GUI: ";
    if (!(std::cin >> choice) || choice < 1 || choice > 2) {
        std::cerr << "Invalid input. Please enter a number between 1 and 2.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return askForModeChoice();
    }
    if (choice == 1) {
        std::cout << "Terminal mode selected.\n";
    } else {
        std::cout << "GUI mode selected.\n";
    }
    return choice;
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
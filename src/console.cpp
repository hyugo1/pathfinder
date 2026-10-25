#include <iostream>

#include "console.hpp"
#include "grid.hpp"
#include "algorithms.hpp"
#include <limits>
#include <ios>

void printPerformanceStats(const SearchResult& result, const Algorithm& chosenAlgo, double executionTime) {
    std::cout << "\n--- Performance Stats ---\n";
    std::cout << "Algorithm: " << algorithmName(chosenAlgo) << "\n";
    std::cout << "Execution Time: " << executionTime << " ms\n";
    std::cout << "Total Nodes Explored: " << result.explorationOrder.size() << "\n";
    std::cout << "Path Length: " << result.path.size() << "\n";
}

void displayPerformanceStats(bool statsRequested, const Algorithm& chosenAlgo, const SearchResult& result, double executionTime) {
    if (statsRequested && chosenAlgo != Algorithm::None) {
        printPerformanceStats(result, chosenAlgo, executionTime);
    }
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

int askForAlgorithmChoice() {
    int algorithmChoice {};
    std::cout << "Which algorithm do you want to use? "; 
    for (int i = 1; i <= 4; ++i) {
        std::cout << i << " for " << (i == 1 ? "DFS" : (i == 2 ? "BFS" : (i == 3 ? "A*" : "Dijkstra")));
    }
    std::cout << "\nEnter your algorithmChoice: ";
    while (!(std::cin >> algorithmChoice) || algorithmChoice < 1 || algorithmChoice > 4) {
        std::cerr << "Invalid input. Please enter a number between 1 and 4.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    return algorithmChoice;
}

bool chooseThreadedMode() {
    int choice {};
    std::cout << "Choose mode: 1 for Threaded, 2 for Non-Threaded: ";
    while (!(std::cin >> choice) || choice < 1 || choice > 2) {
        std::cerr << "Invalid input. Please enter a number between 1 and 2.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
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
    while (!(std::cin >> choice) || choice < 1 || choice > 2) {
        std::cerr << "Invalid input. Please enter a number between 1 and 2.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    if (choice == 1) {
        std::cout << "Terminal mode selected.\n";
    } else {
        std::cout << "GUI mode selected.\n";
    }
    return choice;
}
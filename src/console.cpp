#include <iostream>

#include "console.hpp"
#include "grid.hpp"
#include "algorithms.hpp"
#include <string>
#include <optional>
#include <random>
#include <limits>
#include <stdexcept>
#include <ios>

void printAlgorithmName(const Algorithm& algo) {
    std::cout << "Using " << algorithmName(algo) << ".\n";
}

std::string algorithmName(const Algorithm& algo) {
    switch (algo) {
        case Algorithm::DFS:
            return "DFS";
        case Algorithm::BFS:
            return "BFS";
        case Algorithm::AStar:
            return "A*";
        case Algorithm::Dijkstra:
            return "Dijkstra";
        default:
            return "None";
    }
}

void printPerformanceStats(const SearchResult& result, const Algorithm& chosenAlgo, double executionTime) {
    std::cout << "\n--- Performance Stats ---\n";
    std::cout << "Algorithm: " << algorithmName(chosenAlgo) << "\n";
    if (executionTime > 0.0) {
        std::cout << "Execution Time: " << executionTime << " ms\n";
    }
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
        case CellType::Mud:   os << 'M'; break;
        case CellType::Water: os << 'W'; break;
        default:              os << '?'; break;
    }
    return os;
}

void printTerminalGridAndStats(Grid& copy, const SearchResult& result, const Algorithm& chosenAlgo) {
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
    printPerformanceStats(result, chosenAlgo, 0.0); // No execution time for terminal display
}

Algorithm askForAlgorithmChoice() {
    int choice {};
    std::cout << "Which algorithm do you want to use? "; 
    std::cout << "Choose an algorithm:\n"
              << "1. DFS\n"
              << "2. BFS\n"
              << "3. A-star\n"
              << "4. Dijkstra\n"
              << "Enter your choice: ";
    while (!(std::cin >> choice) || choice < 1 || choice > 4) {
        std::cerr << "Invalid input. Please enter a number between 1 and 4.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    return static_cast<Algorithm>(choice);
}

bool chooseThreadedMode() {
    int choice {};
    std::cout << "Choose mode: 1 for Threaded, 2 for Non-Threaded: ";
    while (!(std::cin >> choice) || choice < 1 || choice > 2) {
        std::cerr << "Invalid input. Please enter a number between 1 and 2.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
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
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (choice == 1) {
        std::cout << "Terminal mode selected.\n";
    } else {
        std::cout << "GUI mode selected.\n";
    }
    return choice;
}

std::optional<unsigned int> askForSeed() {
    std::string input;
    while (true) {
        std::cout << "Enter a seed for random grid generation "
                     "(positive integer, or press Enter for random): ";
        if (!std::getline(std::cin, input)) {
            return std::nullopt;
        }
        if (input.empty()) {
            std::random_device rd;
            unsigned int seed = rd();
            std::cout << "Using random seed: " << seed << '\n';
            return seed;
        }
        try {
            if (input.find_first_not_of("0123456789") != std::string::npos) {
                throw std::invalid_argument("Seed must contain digits only");
            }
            std::size_t pos = 0;
            unsigned long value = std::stoul(input, &pos);
            if (pos != input.size() ||
                value == 0 ||
                value > std::numeric_limits<unsigned int>::max()) {
                throw std::invalid_argument("Seed out of range");
            }
            unsigned int seed = static_cast<unsigned int>(value);
            std::cout << "Using seed: " << seed << '\n';
            return seed;
        }
        catch (const std::exception&) {
            std::cerr << "Invalid input. Please enter a positive integer.\n";
        }
    }
}

std::optional<double> askForWallDensity() {
    double wallDensity {};
    std::cout << "Enter wall density(probability that a cell will be a wall) (0.0 to 1.0): ";
    while (!(std::cin >> wallDensity) || wallDensity < 0.0 || wallDensity > 1.0) {
        if (std::cin.eof()) {
            return std::nullopt;
        }
        std::cerr << "Invalid input. Please enter a number between 0.0 and 1.0.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Using wall density: " << wallDensity << "\n";
    return wallDensity;
}
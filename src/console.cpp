#include <iostream>

#include "console.hpp"
#include "grid.hpp"
#include "algorithms.hpp"
#include <string>
#include <optional>
#include <random>
#include <limits>
#include <stdexcept>
#include <cmath>
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
    std::cout << "Total Cost: " << result.totalCost << "\n";
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
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string PURPLE = "\033[35m";
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
                    std::cout << PURPLE << cell << RESET;
                    break;
                case CellType::Mud:
                    std::cout << YELLOW << cell << RESET;
                    break;
                case CellType::Water:
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
            std::size_t pos = 0;// length of the number
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
    std::string input;
    while (true) {
        std::cout << "Enter wall density(probability that a cell will be a wall) (0.0 to 1.0): ";
        if (!std::getline(std::cin, input)) {
            return std::nullopt;
        }
        if (input.empty()) {
            double wallDensity = 0.3;// Default wall density
            std::cout << "Using default wall density: " << wallDensity << '\n';
            return wallDensity;
        }

        try {
            std::size_t pos;//size of the string
            double wallDensity = std::stod(input, &pos);
            if (pos != input.size() || wallDensity < 0.0 || wallDensity > 1.0 || !std::isfinite(wallDensity)) {
                throw std::invalid_argument("Invalid wall density");
            }
            std::cout << "Using density: " << wallDensity << '\n';
            return wallDensity;

        } catch (const std::exception&) {
            std::cerr << "Invalid input. Please enter a number between 0.0 and 1.0.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

std::optional<double> askForMudCount() {
    std::string input;
    while (true) {
        std::cout << "Enter the number of mud blobs (0 to 100): ";
        if (!std::getline(std::cin, input)) {
            return std::nullopt;
        }
        if (input.empty()) {
            double mudCount = 10; // Default mud count
            std::cout << "Using default mud count: " << mudCount << '\n';
            return mudCount;
        }

        try {
            std::size_t pos;
            double mudCount = std::stod(input, &pos);
            if (pos != input.size() || mudCount < 0.0 || mudCount > 100.0 || !std::isfinite(mudCount)) {
                throw std::invalid_argument("Invalid mud count");
            }
            std::cout << "Using mud count: " << mudCount << '\n';
            return mudCount;

        } catch (const std::exception&) {
            std::cerr << "Invalid input. Please enter a number between 0 and 100.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

std::optional<double> askForWaterCount() {
    std::string input;
    while (true) {
        std::cout << "Enter the number of water blobs (0 to 100): ";
        if (!std::getline(std::cin, input)) {
            return std::nullopt;
        }
        if (input.empty()) {
            double waterCount = 10; // Default water count
            std::cout << "Using default water count: " << waterCount << '\n';
            return waterCount;
        }

        try {
            std::size_t pos;
            double waterCount = std::stod(input, &pos);
            if (pos != input.size() || waterCount < 0.0 || waterCount > 100.0 || !std::isfinite(waterCount)) {
                throw std::invalid_argument("Invalid water count");
            }
            std::cout << "Using water count: " << waterCount << '\n';
            return waterCount;

        } catch (const std::exception&) {
            std::cerr << "Invalid input. Please enter a number between 0 and 100.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}
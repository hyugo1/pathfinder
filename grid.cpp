#include "grid.hpp"
#include <iostream>

Position findCellPosition(const Grid& grid, CellType target) {
    for (int r {0}; r < static_cast<int>(grid.size()); ++r) {
        for (int c {0}; c < static_cast<int>(grid[0].size()); ++c) {
            if (grid[r][c] == target) {
                return {r, c};
            }
        }
    }
    return {-1, -1};
}

bool isInBounds(const Grid& grid, int row, int col) {
    return row >= 0 && row < static_cast<int>(grid.size()) &&
           col >= 0 && col < static_cast<int>(grid[0].size());
}

bool isTraversable(const Grid& grid, int row, int col) {
    return isInBounds(grid, row, col) && grid[row][col] != CellType::Wall;
}

std::vector<std::vector<bool>> createVisitedGrid(int rows, int cols) {
    return std::vector<std::vector<bool>>(rows, std::vector<bool>(cols, false));
}

std::pair<Position, Position> findStartAndEndPositions(const Grid& grid) {
    Position start = findCellPosition(grid, CellType::Start);
    Position end = findCellPosition(grid, CellType::End);
    return {start, end};
}

bool checkIfGridIsEmpty(const Grid& grid) {
    if (grid.empty() || grid[0].empty()) {
        std::cerr << "Grid is empty.\n";
        return false;
    }
    return true;
}

bool validateExactlyOneStartAndEnd(const Grid& grid) {
    int startCount = 0;
    int endCount = 0;

    for (const auto& row : grid) {
        startCount += std::count(row.begin(), row.end(), CellType::Start);
        endCount += std::count(row.begin(), row.end(), CellType::End);
    }

    if (startCount != 1 || endCount != 1) {
        std::cerr << "Grid must contain exactly one Start and one End.\n";
        return false;
    }

    return true;
}

bool validateGrid(const Grid& grid) {
    if (!checkIfGridIsEmpty(grid)) {
        return false;
    }
    if (!validateExactlyOneStartAndEnd(grid)) {
        return false;
    }

    return true;
}

void resetGrid(Grid& grid) {
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            grid[r][c] = CellType::Open;
        }
    }
}
#pragma once

#include <utility>
#include <vector>

enum class CellType { Open, Wall, Start, End, Path, Explored, Mud, Water };

constexpr int ROWS = 80;
constexpr int COLS = 80;

using Position = std::pair<int, int>;
using Grid = std::vector<std::vector<CellType>>;

Position findCellPosition(const Grid& grid, CellType target);
bool isInBounds(const Grid& grid, int row, int col);
bool isTraversable(const Grid& grid, int row, int col);
std::vector<std::vector<bool>> createVisitedGrid(int rows, int cols);
bool checkIfGridIsEmpty(const Grid& grid);
std::pair<Position, Position> findStartAndEndPositions(const Grid& grid);
bool validateExactlyOneStartAndEnd(const Grid& grid);
bool validateGrid(const Grid& grid);
void resetGrid(Grid& grid);
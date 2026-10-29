#include "generator.hpp"
#include <random>
#include "grid.hpp"
#include "appstate.hpp"

Grid generateRandomGrid(int rows, int cols, double wallDensity, unsigned int seed) {
    std::mt19937 rng(seed);// seed so its not truly random, hence repeatable.
    std::uniform_real_distribution<double> dist(0.0, 1.0);// squash the long range into 0 to 1.

    if (rows <= 0 || cols <= 0) return {};

    Grid grid(rows, std::vector<CellType>(cols, CellType::Open));//empty board
    std::vector<Position> openCells;
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            // Roll a number in [0, 1]. If it’s below wallDensity, make it a wall.
            if (dist(rng) < wallDensity) {
                grid[r][c] = CellType::Wall;
            } else {
                openCells.emplace_back(r, c);
            }
        }
    }

    if (openCells.size() < 2) {
        return grid; // or report failure
    }

    Position start, end;
    std::uniform_int_distribution<size_t> indexDist(0, openCells.size() - 1);//pick random index into the list of open cells

    start = openCells[indexDist(rng)];
    grid[start.first][start.second] = CellType::Start;

    do {
        end = openCells[indexDist(rng)];
    } while (end == start);
    grid[end.first][end.second] = CellType::End;

    return grid;
}
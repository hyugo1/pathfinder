#include "generator.hpp"
#include <random>
#include <queue>
#include <array>
#include <cmath>
#include <iostream>
#include <cstdlib>
#include "grid.hpp"

Grid generateRandomGrid(int rows, int cols, double wallDensity, double mudCount, double waterCount, unsigned int seed) {
    if (rows <= 0 || cols <= 0) return {};
    std::mt19937 rng(seed);
    std::uniform_real_distribution<double> dist(0.0, 1.0);// squash the long range into 0 to 1.


    Grid grid(rows, std::vector<CellType>(cols, CellType::Open));//empty board
    std::vector<Position> openCells;
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
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

    std::uniform_int_distribution<std::size_t> indexDist(0, openCells.size() - 1);
    for (int i = 0; i < mudCount; ++i)
        frontierGrowth(grid, rng, 0.5, 0.1, CellType::Mud);
    for (int i = 0; i < waterCount; ++i)
        frontierGrowth(grid, rng, 0.5, 0.1, CellType::Water);

    Position start;
    
    do {
        start = openCells[indexDist(rng)];
    } while (grid[start.first][start.second] != CellType::Open);
    grid[start.first][start.second] = CellType::Start;

    Position end;
    do {
        end = openCells[indexDist(rng)];
    } while (end == start || (grid[end.first][end.second] != CellType::Open));
    grid[end.first][end.second] = CellType::End;

    return grid;
}

void frontierGrowth(Grid& grid, std::mt19937& rng, double initialProbability, double decayRate, CellType terrainType) {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    std::queue<std::pair<Position, int>> q;
    int startDistance = 0;
    Position start;
    do {
        start = {rng() % grid.size(), rng() % grid[0].size()};
    } while (grid[start.first][start.second] != CellType::Open);
    
    q.push({start, startDistance});
    grid[start.first][start.second] = terrainType;

    static const std::array<Position, 4> DIRECTIONS = {
        Position{-1, 0}, Position{1, 0}, Position{0, -1}, Position{0, 1}
    };

    while (!q.empty()) {
        auto [current, distance] = q.front();
        q.pop();
        distance++;

        for (const Position& direction : DIRECTIONS) {
            int nr = current.first + direction.first;
            int nc = current.second + direction.second;
            double probability = initialProbability - (decayRate * distance);

            if (nr >= 0 && nr < grid.size() && nc >= 0 && nc < grid[0].size() && grid[nr][nc] == CellType::Open) {
                if (dist(rng) < probability) {
                    grid[nr][nc] = terrainType;
                    q.push({{nr, nc}, distance});
                }
            }
        }
    }
}
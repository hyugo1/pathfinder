#pragma once

#include "grid.hpp"
#include <vector>
#include <array>

enum class Algorithm { None, DFS, BFS, AStar, Dijkstra };

constexpr int OPEN_COST = 1;
constexpr int WALL_COST = -1; // impassable
constexpr int PATH_COST = 1;
constexpr int EXPLORED_COST = 1;
constexpr int START_COST = 1;
constexpr int END_COST = 1;
constexpr int MUD_COST = 5;
constexpr int WATER_COST = 10;

struct SearchResult {
    std::vector<Position> path;
    std::vector<Position> explorationOrder;
};

struct SearchState {
    std::vector<std::vector<Position>> cameFrom;
    std::vector<Position> explorationOrder;
};

constexpr std::array<Position, 4> DIRECTIONS {{
    {-1, 0}, // Up
    { 1, 0}, // Down
    { 0,-1}, // Left
    { 0, 1}  // Right
}};


SearchState initializeSearchState(const Grid& grid);
// cameFrom means which cell did I come from to reach this cell
std::vector<Position> reconstructPath(const std::vector<std::vector<Position>>& cameFrom, Position end);
int getMovementCost(CellType cellType);
SearchResult breadthFirstSearch(const Grid& grid, Position start, Position end);
SearchResult depthFirstSearch(const Grid& grid, Position start, Position end);
int manhattanDistance(const Position& current, const Position& goal);
SearchResult aStarSearch(const Grid& grid, Position start, Position end);
SearchResult runSearch(const Grid& grid, Position start, Position end, Algorithm algo);
void clearSearchResult(SearchResult& result);
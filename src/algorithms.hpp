#pragma once

#include "grid.hpp"
#include <vector>

enum class Algorithm { None, DFS, BFS, AStar, Dijkstra };

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
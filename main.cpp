#include <iostream>
#include <vector>
#include <algorithm>
#include <array>
#include <queue>
#include <fstream>

using Grid = std::vector<std::vector<char>>; //'#' wall, '.' open, 'S' start, 'E' end
using Position = std::pair<int, int>;// {row, col}

Position findChar(const Grid& grid, char target) {
    for (int r {0}; r < static_cast<int>(grid.size()); ++r) {
        for (int c {0}; c < static_cast<int>(grid[0].size()); ++c) {
            if (grid[r][c] == target) {
                return {r, c};
            }
        }
    }
    return {-1, -1};
}

bool inBounds(const Grid& grid, int row, int col) {
    return row >= 0 && row < static_cast<int>(grid.size()) &&
           col >= 0 && col < static_cast<int>(grid[0].size());
}

std::vector<Position> reconstructPath(const std::vector<std::vector<Position>>& cameFrom, Position end) {
    std::vector<Position> path;
    Position current = end;

    while (current.first != -1 && current.second != -1) {
        path.push_back(current);
        current = cameFrom[current.first][current.second];
    }

    std::reverse(path.begin(), path.end());// reverse it so it is from start again.
    return path;
}

std::vector<Position> bfs(const Grid& grid, Position start, Position end) {
    
    std::vector<std::vector<bool>> visited(grid.size(), std::vector<bool>(grid[0].size(), false));
    std::vector<std::vector<Position>> cameFrom(grid.size(), std::vector<Position>(grid[0].size(), {-1, -1})); //INITIALIZE with {-1, -1};

    std::queue<Position> q;
    q.push(start);
    visited[start.first][start.second] = true;// mark start as true to prevent going there again.

    const std::array<Position, 4> directions {{
        {-1, 0}, {1, 0}, {0, -1}, {0, 1}
    }};

    while (!q.empty()) {
        Position current = q.front();
        q.pop();

        if (current == end) {
            return reconstructPath(cameFrom, end);
        }

        for (const Position& direction : directions) {
            int nr = current.first + direction.first;
            int nc = current.second + direction.second;

            if (inBounds(grid, nr, nc) && !visited[nr][nc] && grid[nr][nc] != '#') {
                visited[nr][nc] = true;
                cameFrom[nr][nc] = current;
                q.push({nr, nc});
            }
        }
    }

    return {}; // no path found
}


void printGrid(Position start, Position end, Grid& copy, std::vector<Position>& path) {
    if (!path.empty()) {
        for (const Position& position : path) {
            if (copy[position.first][position.second] != 'S' && copy[position.first][position.second] != 'E') {
                copy[position.first][position.second] = '*';
            }
        }
        
        for (const auto& row : copy) {
            for (char cell : row) {
                std::cout << cell;
            }
            std::cout << '\n';
        }
    } else {
        std::cout << "No path found.\n";
    }
}
    
int main() {
   Grid grid = {
        {'S', '.', '#', '.', '.'},
        {'.', '#', '.', '.', '#'},
        {'.', '.', '.', '#', '.'},
        {'#', '#', '.', '.', '.'},
        {'.', '.', '.', '#', 'E'}
    };
    // Grid grid = readFromFile();

    Position start = findChar(grid, 'S');
    Position end = findChar(grid, 'E');
    std::vector<Position> path = bfs(grid, start, end);

    Grid copy = grid;
    printGrid(start, end, copy, path);


    return 0;
}
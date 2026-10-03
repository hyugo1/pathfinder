#include <iostream>
#include <vector>
#include <algorithm>
#include <array>
#include <queue>
#include <fstream>
#include <stack>

using Grid = std::vector<std::vector<char>>; //'#' wall, '.' open, 'S' start, 'E' end
using Position = std::pair<int, int>;// {row, col}

struct SearchResult {
    std::vector<Position> path;
    int nodesExplored;
};

std::string parseLine(const std::string& line) {
    std::string result;
    for (size_t i = 0; i < line.size(); ++i) {
        if (line[i] == '\'') {
            if (i + 1 < line.size()) {
                result += line[i + 1];
                i += 2; // skip 'X'
            }
        }
    }
    return result;
}

Grid readFromFile(std::string filename) {
    std::ifstream file(filename);
    std::string line;
    Grid grid;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '@') {
            continue;
        }
        if (line[0] == '+' || line[0] == '-') {
            line = line.substr(1);
        }
        std::string parsed = parseLine(line);
        if (!parsed.empty()) {
            std::vector<char> row(parsed.begin(), parsed.end());
            grid.push_back(row);
        }
    }

    return grid;
}

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
    Position current = end;
    std::vector<Position> path;
    
    while (current.first != -1 && current.second != -1) {
        path.push_back(current);
        current = cameFrom[current.first][current.second];
    }

    std::reverse(path.begin(), path.end());// reverse it so it is from start again.
    return path;
}

SearchResult bfs(const Grid& grid, Position start, Position end) {
    
    std::vector<std::vector<bool>> visited(grid.size(), std::vector<bool>(grid[0].size(), false));
    std::vector<std::vector<Position>> cameFrom(grid.size(), std::vector<Position>(grid[0].size(), {-1, -1})); //INITIALIZE with {-1, -1};

    std::queue<Position> q;
    q.push(start);
    visited[start.first][start.second] = true;// mark start as true to prevent going there again.
    int explored = 0;

    const std::array<Position, 4> directions {{
        {-1, 0}, {1, 0}, {0, -1}, {0, 1}
    }};

    while (!q.empty()) {
        Position current = q.front();
        explored++;
        q.pop();

        if (current == end) {
            return { reconstructPath(cameFrom, end), explored };
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

    return {{}, explored}; // no path found
}

SearchResult iterativeDfs(const Grid& grid, Position start, Position end) {
    std::vector<std::vector<bool>> visited(grid.size(), std::vector<bool>(grid[0].size(), false));
    std::vector<std::vector<Position>> cameFrom(grid.size(), std::vector<Position>(grid[0].size(), {-1, -1})); //INITIALIZE with {-1, -1};

    std::stack<Position> st;
    st.push(start);
    visited[start.first][start.second] = true;// mark start as true to prevent going there again.
    int explored = 0;

    const std::array<Position, 4> directions {{
        {-1, 0}, {1, 0}, {0, -1}, {0, 1}
    }};

    while (!st.empty()) {
        Position current = st.top();
        st.pop();
        explored++;

        if (current == end) {
            return {reconstructPath(cameFrom, end), explored};
        }

        // for (const Position& direction : directions) {
        for (int i = directions.size() - 1; i >= 0; --i) {
            const Position& direction = directions[i];
            int nr = current.first + direction.first;
            int nc = current.second + direction.second;

            if (inBounds(grid, nr, nc) && !visited[nr][nc] && grid[nr][nc] != '#') {
                visited[nr][nc] = true;
                cameFrom[nr][nc] = current;
                st.push({nr, nc});
            }
        }
    }

    return {{}, explored}; // no path found
}

void printGrid(Position start, Position end, Grid& copy, const std::vector<Position>& path) {
    if (!path.empty()) {
        for (const Position& position : path) {
            if (copy[position.first][position.second] != 'S' && copy[position.first][position.second] != 'E') {
                copy[position.first][position.second] = '*';
            }
        }
        const std::string RED = "\033[31m";
        const std::string GREEN = "\033[32m";
        const std::string BLUE = "\033[34m";
        const std::string RESET = "\033[0m";
        
        for (const auto& row : copy) {
            for (char cell : row) {
                if (cell == '*') {
                    std::cout << RED << cell << RESET;
                } else if (cell == 'S') {
                    std::cout << GREEN << cell << RESET;
                } else if (cell == 'E') {
                    std::cout << BLUE << cell << RESET;
                } else {
                    std::cout << cell;
                }
            }
            std::cout << '\n';
        }
    } else {
        std::cout << "No path found.\n";
    }
}
    
int main() {
    Grid grid = readFromFile("grids/biggrid.txt");
 
    if (grid.empty() || grid[0].empty()) {
        std::cerr << "Invalid or empty grid.\n";
        return 1;
    }

    Position start = findChar(grid, 'S');
    Position end = findChar(grid, 'E');

    if (start.first == -1 || end.first == -1) {
        std::cerr << "Start or End not found.\n";
        return 1;
    }

    int choice {};
    std::cout << "Which algorithm do you want to use? (1 for BFS, 2 for DFS): ";
    std::cin >> choice;

    SearchResult result;
    if (choice == 1) {
        result = bfs(grid, start, end);
    } else {
        result = iterativeDfs(grid, start, end);
    }

    Grid copy = grid;
    printGrid(start, end, copy, result.path);

    std::cout << "Explored: " << result.nodesExplored << " nodes.\n";
    std::cout << "Path Length: " << result.path.size() << ".\n";
    return 0;
}
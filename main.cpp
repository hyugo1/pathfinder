#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>
#include <array>
#include <queue>
#include <fstream>
#include <stack>
#include <limits>

using Position = std::pair<int, int>;// {row, col}

constexpr int ROWS = 20;
constexpr int COLS = 20;
constexpr int CELL_SIZE = 30; // pixels per grid cell

enum class CellType { Open, Wall, Start, End, Path, Explored };

enum class Algorithm { None, DFS, BFS, AStar };
Algorithm currentAlgo = Algorithm::None;

using Grid = std::vector<std::vector<CellType>>;

struct SearchResult {
    std::vector<Position> path;
    int totalNodesExplored;
    // animation
    std::vector<Position> explorationOrder;
    // final state (optional but useful)
    std::vector<std::vector<bool>> exploredGrid;

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
            std::vector<CellType> row;
            for (char c : parsed) {
                switch (c) {
                    case '#': row.push_back(CellType::Wall); break;
                    case '.': row.push_back(CellType::Open); break;
                    case 'S': row.push_back(CellType::Start); break;
                    case 'E': row.push_back(CellType::End); break;
                    case '*': row.push_back(CellType::Path); break;
                    case 'o': row.push_back(CellType::Explored); break;
                    default: row.push_back(CellType::Open);
                }
            }
            grid.push_back(row);
        }
    }

    return grid;
}

Position findChar(const Grid& grid, CellType target) {
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
    std::vector<std::vector<bool>> explored(grid.size(), std::vector<bool>(grid[0].size(), false)); // needed for animation to show explored nodes
    std::vector<std::vector<bool>> visited(grid.size(), std::vector<bool>(grid[0].size(), false));
    std::vector<std::vector<Position>> cameFrom(grid.size(), std::vector<Position>(grid[0].size(), {-1, -1}));

    std::queue<Position> q;
    q.push(start);
    visited[start.first][start.second] = true;
    int exploredCount = 0;
    std::vector<Position> explorationOrder;

    const std::array<Position, 4> directions {{
        {-1, 0}, {1, 0}, {0, -1}, {0, 1}
    }};

    while (!q.empty()) {
        Position current = q.front();
        exploredCount++;
        explored[current.first][current.second] = true;
        explorationOrder.push_back(current);
        q.pop();

        if (current == end) {
            return {reconstructPath(cameFrom, end), exploredCount, explorationOrder, explored};
        }

        for (const Position& direction : directions) {
            int nr = current.first + direction.first;
            int nc = current.second + direction.second;

            if (inBounds(grid, nr, nc) && !visited[nr][nc] && grid[nr][nc] != CellType::Wall) {
                visited[nr][nc] = true;
                cameFrom[nr][nc] = current;
                q.push({nr, nc});
            }
        }
    }

    return {{}, exploredCount, {}, explored}; // no path found
}

SearchResult iterativeDfs(const Grid& grid, Position start, Position end) {
    std::vector<std::vector<bool>> explored(grid.size(), std::vector<bool>(grid[0].size(), false)); // needed for animation to show explored nodes
    std::vector<std::vector<bool>> visited(grid.size(), std::vector<bool>(grid[0].size(), false));
    std::vector<std::vector<Position>> cameFrom(grid.size(), std::vector<Position>(grid[0].size(), {-1, -1}));

    std::stack<Position> st;
    st.push(start);
    visited[start.first][start.second] = true;
    int exploredCount = 0;
    std::vector<Position> explorationOrder;

    const std::array<Position, 4> directions {{
        {-1, 0}, {1, 0}, {0, -1}, {0, 1}
    }};

    while (!st.empty()) {
        Position current = st.top();
        st.pop();
        exploredCount++;
        explorationOrder.push_back(current);
        explored[current.first][current.second] = true;
        if (current == end) {
            return {reconstructPath(cameFrom, end), exploredCount, explorationOrder, explored};
        }

        // backtrack in reverse order to maintain the original order of exploration
        for (int i = directions.size() - 1; i >= 0; --i) {
            const Position& direction = directions[i];
            int nr = current.first + direction.first;
            int nc = current.second + direction.second;

            if (inBounds(grid, nr, nc) && !visited[nr][nc] && grid[nr][nc] != CellType::Wall) {
                visited[nr][nc] = true;
                cameFrom[nr][nc] = current;
                st.push({nr, nc});
            }
        }
    }

    return {{}, exploredCount, {}, explored}; // no path found
}

// Out of all the places I could go next, which one seems closest to the goal while also not being too expensive so far?
SearchResult aStar(const Grid& grid, Position start, Position end) {
    // * g(n) → how far I’ve actually moved from start
    // * h(n) → guess to the goal (Manhattan distance)
    // * f(n) = g + h → “how promising this path looks”
    int rowCount = grid.size();
    int colCount = grid[0].size();
    
    std::vector<std::vector<bool>> visited(rowCount, std::vector<bool>(colCount, false));//[[False for _ in range(cols)] for _ in range(rows)]
    std::vector<std::vector<Position>> cameFrom(rowCount, std::vector<Position>(colCount, {-1, -1}));
    std::vector<std::vector<bool>> explored(grid.size(), std::vector<bool>(grid[0].size(), false)); // needed for animation to show explored nodes
    std::vector<std::vector<int>> gCost(rowCount, std::vector<int>(colCount, std::numeric_limits<int>::max())); // cost from start to this node

    std::priority_queue<std::pair<int, Position>, std::vector<std::pair<int, Position>>, std::greater<>> pq;//std::greater makes it a minheap.default is maxheap.
    
    int hStart = abs(end.first - start.first) + abs(end.second - start.second);
    pq.push({hStart, start});

    gCost[start.first][start.second] = 0;
    const std::array<Position, 4> directions {{
        {-1, 0}, {1, 0}, {0, -1}, {0, 1}
    }};

    int exploredCount = 0;
    std::vector<Position> explorationOrder;
    
    while (!pq.empty()) {
        auto [f, current] = pq.top();
        pq.pop();
        explored[current.first][current.second] = true;
        
        int currentx = current.first;
        int currenty = current.second;
        int endx = end.first;
        int endy = end.second;

        int hCost = abs(end.first - currentx) + abs(end.second - currenty);
        if (f > gCost[currentx][currenty] + hCost) {
            continue; // skip outdated entry
        }
        exploredCount++;
        explorationOrder.push_back(current);

        if (current == end) {
            return {reconstructPath(cameFrom, end), exploredCount, explorationOrder, explored};
        }
        
        int costtomove = 1;
        for (int i = 0; i < 4; ++i) {
            int nr = currentx + directions[i].first;
            int nc = currenty + directions[i].second;
            if (!inBounds(grid, nr, nc) || grid[nr][nc] == CellType::Wall) {
                continue;
            }
            
            int tentativeG = gCost[currentx][currenty] + costtomove; // distance from Start node to current node. cheapest way to get from start to current node. 
            if (tentativeG < gCost[nr][nc]) { // if the new path to neighbor is cheaper than any previous one
                gCost[nr][nc] = tentativeG; // update the cost to reach this neighbor
                cameFrom[nr][nc] = current; // update the path to reach this neighbor
                int hCost = abs(end.first - nr) + abs(end.second - nc); // heuristic cost to end node
                int fCost = tentativeG + hCost; // fCost = gCost + hCost
                
                pq.push({fCost, {nr, nc}}); // push the neighbor with its fCost as priority
            }
        }
    }

    return {{}, exploredCount, {}, explored}; // no path found
}

std::ostream& operator<<(std::ostream& os, CellType cell) {
    switch (cell) {
        case CellType::Open:  os << '.'; break;
        case CellType::Wall:  os << '#'; break;
        case CellType::Start: os << 'S'; break;
        case CellType::End:   os << 'E'; break;
        case CellType::Path:  os << '*'; break;
        case CellType::Explored: os << 'o'; break;
    }
    return os;
}

void printGrid(Position start, Position end, Grid& copy, const std::vector<Position>& path, const std::vector<Position>& exploredNodes) {
    if (!path.empty()) {
        for (const Position& position : path) {
            if (copy[position.first][position.second] != CellType::Start && copy[position.first][position.second] != CellType::End) {
                copy[position.first][position.second] = CellType::Path;
            }
        }
        for (const Position& position : exploredNodes) {
            if (copy[position.first][position.second] != CellType::Start && copy[position.first][position.second] != CellType::End) {
                copy[position.first][position.second] = CellType::Explored;
            }
        }
        const std::string RED = "\033[31m";
        const std::string GREEN = "\033[32m";
        const std::string BLUE = "\033[34m";
        const std::string LIGHT_BLUE = "\033[36m";
        const std::string RESET = "\033[0m";
        
        for (const auto& row : copy) {
            for (CellType cell : row) {
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
    } else {
        std::cout << "No path found.\n";
    }
}

void rerunAlgorithm(const Grid& grid, Position start, Position end,
                    Algorithm& algo, SearchResult& result) {
    if (start.first == -1 || end.first == -1) return;

    switch (algo) {
        case Algorithm::DFS:
            result = iterativeDfs(grid, start, end);
            break;
        case Algorithm::BFS:
            result = bfs(grid, start, end);
            break;
        case Algorithm::AStar:
            result = aStar(grid, start, end);
            break;
        default:
            break;
    }
}

void handleTerminal(Grid& grid, Position& start, Position& end, SearchResult& result) {
    grid = readFromFile("grids/biggrid.txt");
    
    if (grid.empty() || grid[0].empty()) {
        std::cerr << "Invalid or empty grid.\n";
        return;
    }
    
    start = findChar(grid, CellType::Start);
    end = findChar(grid, CellType::End);
    
    if (start.first == -1 || end.first == -1) {
        std::cerr << "Start or End not found.\n";
        return;
    }
    
    int algorithmChoice {};
    std::cout << "Which algorithm do you want to use? "; 
    for (int i = 1; i <= 3; ++i) {
        std::cout << i << " for " << (i == 1 ? "BFS" : (i == 2 ? "DFS" : "A*")) << ", ";
    }
    std::cout << "\nEnter your algorithmChoice: ";
    std::cin >> algorithmChoice;
    
    switch (algorithmChoice) {
        case 1:
            result = bfs(grid, start, end);
            break;
        case 2:
            result = iterativeDfs(grid, start, end);
            break;
        case 3:
            result = aStar(grid, start, end);
            break;
        default:
            std::cerr << "Invalid choice.\n";
            return;
    }
    
    Grid copy = grid;
    printGrid(start, end, copy, result.path, result.explorationOrder);
    
    std::cout << "Explored: " << result.totalNodesExplored << " nodes.\n";
    std::cout << "Path Length: " << result.path.size() << ".\n";
}


void handleGUI(Grid& grid, Position& start, Position& end,
               SearchResult& result, Position& lastClicked,
               sf::RenderWindow& window) {
    bool gridChanged = false;

    while (window.isOpen()) {

        // --- EVENT LOOP ---
        while (const std::optional event = window.pollEvent()) {

            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            // --- KEYBOARD ---
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {

                if ((keyPressed->code == sf::Keyboard::Key::Num1 ||
                     keyPressed->code == sf::Keyboard::Key::Num2 ||
                     keyPressed->code == sf::Keyboard::Key::Num3) &&
                    (start.first == -1 || end.first == -1)) {

                    std::cout << "Set start and end first!\n";
                    continue;
                }

                switch (keyPressed->code) {
                    case sf::Keyboard::Key::Escape:
                        window.close();
                        break;
                    case sf::Keyboard::Key::Num1:
                        std::cout << "Using DFS.\n";
                        currentAlgo = Algorithm::DFS;
                        gridChanged = true;
                        break;
                    case sf::Keyboard::Key::Num2:
                        std::cout << "Using BFS.\n";
                        currentAlgo = Algorithm::BFS;
                        gridChanged = true;
                        break;
                    case sf::Keyboard::Key::Num3:
                        std::cout << "Using A*.\n";
                        currentAlgo = Algorithm::AStar;
                        gridChanged = true;
                        break;
                    case sf::Keyboard::Key::C:
                        std::cout << "Clearing the entire grid.\n";
                        gridChanged = true;
                        for (int r = 0; r < ROWS; ++r) {
                            for (int c = 0; c < COLS; ++c) {
                                grid[r][c] = CellType::Open;
                            }
                        }

                        start = {-1, -1};
                        end   = {-1, -1};
                        result.path.clear();
                        currentAlgo = Algorithm::None;
                        break;
                    default:
                        std::cout << "Unknown key pressed.\n";
                        break;
                }
            }

            // --- MOUSE ---
            if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {

                int col = mousePressed->position.x / CELL_SIZE;
                int row = mousePressed->position.y / CELL_SIZE;

                if (row >= 0 && row < ROWS && col >= 0 && col < COLS) {
                    if (mousePressed->button == sf::Mouse::Button::Left) {
                        // toggle wall
                        grid[row][col] =
                            (grid[row][col] == CellType::Wall)
                            ? CellType::Open
                            : CellType::Wall;
                        result.path.clear();
                        gridChanged = true;
                    }
                    else if (mousePressed->button == sf::Mouse::Button::Right) {
                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
                            // set END
                            for (auto& r : grid)
                                for (auto& c : r)
                                    if (c == CellType::End) c = CellType::Open;
                            result.path.clear();
                            end = {row, col};
                            grid[row][col] = CellType::End;
                            gridChanged = true;
                        }
                        else {
                            // set START
                            for (auto& r : grid)
                                for (auto& c : r)
                                    if (c == CellType::Start) c = CellType::Open;
                            result.path.clear();
                            start = {row, col};
                            grid[row][col] = CellType::Start;
                            gridChanged = true;
                        }
                    }
                }
            }
        }

        if (gridChanged) {
            rerunAlgorithm(grid, start, end, currentAlgo, result);
            gridChanged = false;
        }

        // --- RENDER ---
        window.clear(sf::Color::White);

        for (int row = 0; row < ROWS; ++row) {
            for (int col = 0; col < COLS; ++col) {

                sf::RectangleShape cell({CELL_SIZE, CELL_SIZE});
                cell.setPosition({col * CELL_SIZE * 1.f, row * CELL_SIZE * 1.f});

                // base color
                switch (grid[row][col]) {
                    case CellType::Open:  cell.setFillColor(sf::Color::White); break;
                    case CellType::Wall:  cell.setFillColor(sf::Color::Black); break;
                    case CellType::Path:  cell.setFillColor(sf::Color::Yellow); break;
                    case CellType::Explored:  cell.setFillColor(sf::Color::Cyan); break;
                    case CellType::Start: cell.setFillColor(sf::Color::Green); break;
                    case CellType::End:   cell.setFillColor(sf::Color::Blue); break;
                }

                for (const auto& p : result.exploredNodes) {
                    if (p.first == row && p.second == col) {
                        cell.setFillColor(sf::Color(173, 216, 230)); // light blue
                        break;
                    }
                }


                for (const auto& p : result.explorationOrder) { // animation of explored nodes
                    if (result.exploredGrid[p.first][p.second] == true) {
                        if (p.first == row && p.second == col) {
                            cell.setFillColor(sf::Color(128, 128, 128)); // grayish blue
                            break;
                        }
                    }
                }

                // highlight path
                for (const auto& p : result.path) {
                    if (p.first == row && p.second == col) {
                        cell.setFillColor(sf::Color::Red);
                        break;
                    }
                }

                cell.setOutlineThickness(1);
                cell.setOutlineColor(sf::Color(200, 200, 200));

                window.draw(cell);
            }
        }

        window.display();
    }
}


int main() {
    Grid grid(ROWS, std::vector<CellType>(COLS, CellType::Open));

    Position start = {-1, -1};
    Position end = {-1, -1};
    // SearchResult result = {{}, 0, {}, {}};
    SearchResult result;
    Position lastClicked = {-1, -1};

    int choice {};
    std::cout << "Choose mode: 1 for Terminal, 2 for GUI: ";
    std::cin >> choice;
    if (choice == 1) {
        handleTerminal(grid, start, end, result);
    } else {
        sf::RenderWindow window(
            sf::VideoMode({COLS * CELL_SIZE, ROWS * CELL_SIZE}),
            "Pathfinding Visualizer"
        );
        handleGUI(grid, start, end, result, lastClicked, window);
    }
    return 0;
}
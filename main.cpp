#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>
#include <array>
#include <queue>
#include <fstream>
#include <stack>
#include <limits>
#include <chrono>

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
    int totalNodesExplored {0};
    // animation
    std::vector<Position> explorationOrder;
};

struct SearchState {
    std::vector<std::vector<Position>> cameFrom;
    int exploredCount {0};
    std::vector<Position> explorationOrder;
};

struct AppState {
    Grid grid;
    Position start {-1, -1};
    Position end {-1, -1};
    SearchResult result;
    bool gridChanged {false};
    bool statsRequested {false};
};

struct AnimationState {
    std::vector<std::vector<bool>> animatedGrid = std::vector<std::vector<bool>>(ROWS, std::vector<bool>(COLS, false));
    std::vector<bool> animatedPath = std::vector<bool>(ROWS * COLS, false);
    int animationStep {0};
    sf::Clock clock;
};

SearchState createSearchState(const Grid& grid) {
    return {
        std::vector<std::vector<Position>>(grid.size(), std::vector<Position>(grid[0].size(), {-1, -1})),
        0,
        {}
    };
}

constexpr std::array<Position, 4> DIRECTIONS {{
    {-1, 0}, // Up
    { 1, 0}, // Down
    { 0,-1}, // Left
    { 0, 1}  // Right
}};

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

Position findCell(const Grid& grid, CellType target) {
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

// cameFrom means which cell did I come from to reach this cell
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
    SearchState state = createSearchState(grid);
    std::vector<std::vector<bool>> visited(grid.size(), std::vector<bool>(grid[0].size(), false)); //[[False for _ in range(cols)] for _ in range(rows)]

    std::queue<Position> q;
    q.push(start);
    visited[start.first][start.second] = true;

    while (!q.empty()) {
        Position current = q.front();
        state.exploredCount++;
        state.explorationOrder.push_back(current);
        q.pop();

        if (current == end) {
            return {reconstructPath(state.cameFrom, end), state.exploredCount, state.explorationOrder};
        }

        for (const Position& direction : DIRECTIONS) {
            int nr = current.first + direction.first;
            int nc = current.second + direction.second;

            if (isInBounds(grid, nr, nc) && !visited[nr][nc] && grid[nr][nc] != CellType::Wall) {
                visited[nr][nc] = true;
                // to get from this place, we came from the current
                state.cameFrom[nr][nc] = current;
                q.push({nr, nc});
            }
        }
    }

    return {{}, state.exploredCount, state.explorationOrder}; // no path found
}

SearchResult iterativeDfs(const Grid& grid, Position start, Position end) {
    SearchState state = createSearchState(grid);
    std::vector<std::vector<bool>> visited(grid.size(), std::vector<bool>(grid[0].size(), false)); //[[False for _ in range(cols)] for _ in range(rows)]

    std::stack<Position> st;
    st.push(start);
    visited[start.first][start.second] = true;

    while (!st.empty()) {
        Position current = st.top();
        st.pop();
        state.exploredCount++;
        state.explorationOrder.push_back(current);
        if (current == end) {
            return {reconstructPath(state.cameFrom, end), state.exploredCount, state.explorationOrder};
        }

        // backtrack in reverse order to maintain the original order of exploration
        for (int i = DIRECTIONS.size() - 1; i >= 0; --i) {
            const Position& direction = DIRECTIONS[i];
            int nr = current.first + direction.first;
            int nc = current.second + direction.second;

            if (isInBounds(grid, nr, nc) && !visited[nr][nc] && grid[nr][nc] != CellType::Wall) {
                visited[nr][nc] = true;
                state.cameFrom[nr][nc] = current;
                st.push({nr, nc});
            }
        }
    }

    return {{}, state.exploredCount, state.explorationOrder}; // no path found
}

// Out of all the places I could go next, which one seems closest to the goal while also not being too expensive so far?
SearchResult aStar(const Grid& grid, Position start, Position end) {
    // * g(n) → how far I’ve actually moved from start
    // * h(n) → guess to the goal (Manhattan distance)
    // * f(n) = g + h → “how promising this path looks”
    int rowCount = grid.size();
    int colCount = grid[0].size();
    
    SearchState state = createSearchState(grid);
    std::vector<std::vector<int>> gCost(rowCount, std::vector<int>(colCount, std::numeric_limits<int>::max())); // cost from start to this node

    std::priority_queue<std::pair<int, Position>, std::vector<std::pair<int, Position>>, std::greater<>> pq;//std::greater makes it a minheap.default is maxheap.
    
    int hStart = abs(end.first - start.first) + abs(end.second - start.second);
    pq.push({hStart, start});

    gCost[start.first][start.second] = 0;

    while (!pq.empty()) {
        auto [f, current] = pq.top();
        pq.pop();
        
        int currentx = current.first;
        int currenty = current.second;
        int hCost = abs(end.first - currentx) + abs(end.second - currenty);
        if (f > gCost[currentx][currenty] + hCost) {
            continue; // skip outdated entry
        }
        state.exploredCount++;
        state.explorationOrder.push_back(current);
        
        if (current == end) {
            return {reconstructPath(state.cameFrom, end), state.exploredCount, state.explorationOrder};
        }
        
        int costtomove = 1;
        for (int i = 0; i < 4; ++i) {
            int nr = currentx + DIRECTIONS[i].first;
            int nc = currenty + DIRECTIONS[i].second;
            if (!isInBounds(grid, nr, nc) || grid[nr][nc] == CellType::Wall) {
                continue;
            }
            
            int tentativeG = gCost[currentx][currenty] + costtomove; // distance from Start node to current node. cheapest way to get from start to current node. 
            if (tentativeG < gCost[nr][nc]) { // if the new path to neighbor is cheaper than any previous one
                gCost[nr][nc] = tentativeG; // update the cost to reach this neighbor
                state.cameFrom[nr][nc] = current; // update the path to reach this neighbor
                int hCost = abs(end.first - nr) + abs(end.second - nc); // heuristic cost to end node
                int fCost = tentativeG + hCost; // fCost = gCost + hCost
                
                pq.push({fCost, {nr, nc}}); // push the neighbor with its fCost as priority
            }
        }
    }

    return {{}, state.exploredCount, state.explorationOrder}; // no path found
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

void printTerminalGridAndStats(Grid& copy, const std::vector<Position>& path, const std::vector<Position>& explorationOrder) {
    if (!path.empty()) {
        for (const Position& position : explorationOrder) {
            if (copy[position.first][position.second] != CellType::Start && copy[position.first][position.second] != CellType::End) {
                copy[position.first][position.second] = CellType::Explored;
            }
        }
        for (const Position& position : path) {
            if (copy[position.first][position.second] != CellType::Start && copy[position.first][position.second] != CellType::End) {
                copy[position.first][position.second] = CellType::Path;
            }
        }
        const std::string RED = "\033[31m";
        const std::string GREEN = "\033[32m";
        const std::string BLUE = "\033[34m";
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
                        std::cout << BLUE << cell << RESET;
                        break;
                    default:
                        std::cout << cell;
                        break;
                    }
                }
            std::cout << '\n';
            }
        std::cout << "Explored: " << explorationOrder.size() << " nodes.\n";
        std::cout << "Path Length: " << path.size() << ".\n";
        
        } else {
            std::cout << "No path found.\n";
        }
}

void runAlgorithm(const Grid& grid, Position start, Position end,
                    const Algorithm& algo, SearchResult& result) {
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

std::string algoName(const Algorithm& algo) {
    switch (algo) {
        case Algorithm::DFS:
            return "DFS";
        case Algorithm::BFS:
            return "BFS";
        case Algorithm::AStar:
            return "A*";
        default:
            return "None";
    }
}
void resetResults(SearchResult& result) {
    result.path.clear();
    result.explorationOrder.clear();
    result.totalNodesExplored = 0;
}

// void resetAnimationStates(std::vector<std::vector<bool>>& animatedGrid, std::vector<bool>& animatedPath, int& animationStep) {
//     animatedGrid = std::vector<std::vector<bool>>(ROWS, std::vector<bool>(COLS, false));
//     animatedPath = std::vector<bool>(ROWS * COLS, false);
//     animationStep = 0;
// }

void resetSearch(SearchResult& result,
                 std::vector<std::vector<bool>>& animatedGrid,
                 std::vector<bool>& animatedPath,
                 int& animationStep) {

    resetResults(result);
    // resetAnimationStates(animatedGrid, animatedPath, animationStep);
}

bool validateGrid(const Grid& grid, Position& start, Position& end) {
    if (grid.empty() || grid[0].empty()) {
        std::cerr << "Invalid or empty grid.\n";
        return false;
    }
    
    start = findCell(grid, CellType::Start);
    end = findCell(grid, CellType::End);
    
    if (start.first == -1 || end.first == -1) {
        std::cerr << "Start or End not found.\n";
        return false;
    }

    if (std::count_if(grid.begin(), grid.end(), [](const std::vector<CellType>& row) {
        return std::count(row.begin(), row.end(), CellType::Start) > 1;
    }) > 0 || std::count_if(grid.begin(), grid.end(), [](const std::vector<CellType>& row) {
        return std::count(row.begin(), row.end(), CellType::End) > 1;
    }) > 0) {
        std::cerr << "More than one Start or End found.\n";
        return false;
    }

    return true;
}

int askAlgorithm() {
    int algorithmChoice {};
    std::cout << "Which algorithm do you want to use? "; 
    for (int i = 1; i <= 3; ++i) {
        std::cout << i << " for " << (i == 1 ? "BFS" : (i == 2 ? "DFS" : "A*")) << ", ";
    }
    std::cout << "\nEnter your algorithmChoice: ";
    if (!(std::cin >> algorithmChoice) || algorithmChoice < 1 || algorithmChoice > 3) {
        std::cerr << "Invalid input. Please enter a number between 1 and 3.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return askAlgorithm(); // recursively ask again
    }

    return algorithmChoice;
}

bool handleAlgorithmChoice(Grid& grid, Position& start, Position& end, int algorithmChoice, SearchResult& result) {
    switch (algorithmChoice) {
        case 1:
            std::cout << "Using BFS.\n";
            currentAlgo = Algorithm::BFS;
            break;
        case 2:
            std::cout << "Using DFS.\n";
            currentAlgo = Algorithm::DFS;
            break;
        case 3:
            std::cout << "Using A*.\n";
            currentAlgo = Algorithm::AStar;
            break;
        default:
            std::cerr << "Invalid choice. Exiting.\n";
            return false;
    }
    runAlgorithm(grid, start, end, currentAlgo, result);
    return true;
}

void handleTerminal(Grid& grid, Position& start, Position& end, SearchResult& result) {
    grid = readFromFile("grids/biggrid.txt");
    
    if (!validateGrid(grid, start, end)) {
        return;
    }

    int algorithmChoice {askAlgorithm()};
    
    if (!handleAlgorithmChoice(grid, start, end, algorithmChoice, result)) {
        return;
    }
    
    Grid copy = grid;
    printTerminalGridAndStats(copy, result.path, result.explorationOrder);
}



void handleKeyboard(AppState& appState, const std::optional<sf::Event>& event) {
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
                std::cout << "Using " << algoName(Algorithm::DFS) << ".\n";
                currentAlgo = Algorithm::DFS;
                gridChanged = true;
                statsRequested = true;
                break;
            case sf::Keyboard::Key::Num2:
                std::cout << "Using " << algoName(Algorithm::BFS) << ".\n";
                currentAlgo = Algorithm::BFS;
                gridChanged = true;
                statsRequested = true;
                break;
            case sf::Keyboard::Key::Num3:
                std::cout << "Using " << algoName(Algorithm::AStar) << ".\n";
                currentAlgo = Algorithm::AStar;
                gridChanged = true;
                statsRequested = true;
                break;
            case sf::Keyboard::Key::C:
                std::cout << "Clearing the entire grid.\n";
                gridChanged = true;
                statsRequested = false;
                for (int r = 0; r < ROWS; ++r) {
                    for (int c = 0; c < COLS; ++c) {
                        grid[r][c] = CellType::Open;
                    }
                }

                start = {-1, -1};
                end   = {-1, -1};
                resetSearch(result, animatedGrid, animatedPath, animationStep);
                currentAlgo = Algorithm::None;
                break;
            case sf::Keyboard::Key::LShift:
                std::cout << "Hold LShift and right click to set the END position.\n";
                break;
            default:
                std::cout << "Unknown key pressed.\n";
                break;
        }
    }
}

void handleMouse(AppState& appState, const std::optional<sf::Event>& event) {
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
                statsRequested = false;
                gridChanged = true;
                
                resetSearch(result, animatedGrid, animatedPath, animationStep);
            }
            else if (mousePressed->button == sf::Mouse::Button::Right) {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
                    // set END
                    for (auto& r : grid)
                        for (auto& c : r)
                            if (c == CellType::End) c = CellType::Open;
                    
                    
                    statsRequested = false;
                    gridChanged = true;
                    end = {row, col};
                    grid[row][col] = CellType::End;

                    resetSearch(result, animatedGrid, animatedPath, animationStep);
                }
                else {
                    // set START
                    for (auto& r : grid)
                        for (auto& c : r)
                            if (c == CellType::Start) c = CellType::Open;
                    start = {row, col};
                    grid[row][col] = CellType::Start;
                    gridChanged = true;
                    statsRequested = false;

                    resetSearch(result, animatedGrid, animatedPath, animationStep);
                }
            }
        }
    }
}

void handleGUI(Grid& grid, Position& start, Position& end,
               SearchResult& result,
               sf::RenderWindow& window) {

    AppState appState;
    AnimationState animationState;

    while (window.isOpen()) {

        // --- EVENT LOOP ---
        while (const std::optional event = window.pollEvent()) {

            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            // --- KEYBOARD ---
            handleKeyboard(appState, event);

            // --- MOUSE ---
            handleMouse(appState, event);
        }

        if (gridChanged) {
            auto algoStart = std::chrono::steady_clock::now();
            runAlgorithm(grid, start, end, currentAlgo, result);
            auto algoEnd = std::chrono::steady_clock::now();
            double executionTime =
                std::chrono::duration<double, std::milli>(
                    algoEnd - algoStart
                ).count();

            if (statsRequested && currentAlgo != Algorithm::None) {
                std::cout << "\n--- Performance Stats ---\n";
                std::cout << "Algorithm: " << algoName(currentAlgo) << "\n";
                std::cout << "Path length: "
                        << result.path.size() << "\n";
                std::cout << "Execution time: "
                        << executionTime << " ms\n";
                std::cout << "Total nodes explored: "
                    << result.totalNodesExplored << "\n";
            }

            // resetAnimationStates(animatedGrid, animatedPath, animationStep);
            clock.restart();
            gridChanged = false;
        }

        if (animationStep < static_cast<int>(result.explorationOrder.size())) {
            //only reveal a new cell every, say, 10-20 milliseconds, so a human can actually perceive the expansion.
            if (clock.getElapsedTime().asMilliseconds() > 10) {
                animatedGrid[result.explorationOrder[animationStep].first][result.explorationOrder[animationStep].second] = true;
                animationStep++;
                clock.restart();
            }
        }

        if (animationStep >= static_cast<int>(result.explorationOrder.size()) &&
            animationStep < static_cast<int>(result.explorationOrder.size() + result.path.size())) {
            if (clock.getElapsedTime().asMilliseconds() > 10) {
                int pathIndex = animationStep - result.explorationOrder.size();
                animatedPath[result.path[pathIndex].first * COLS + result.path[pathIndex].second] = true;
                animationStep++;
                clock.restart();
            }
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

                if (animatedGrid[row][col] && grid[row][col] != CellType::Start && grid[row][col] != CellType::End) {
                    cell.setFillColor(sf::Color::Cyan);
                }
                
                if (animatedPath[row * COLS + col] && grid[row][col] != CellType::Start && grid[row][col] != CellType::End) {
                    cell.setFillColor(sf::Color::Red);
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
    SearchResult result;

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
        handleGUI(grid, start, end, result, window);
    }
    return 0;
}
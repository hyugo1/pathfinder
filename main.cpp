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
#include <optional>
#include <functional>

using Position = std::pair<int, int>;// {row, col}

constexpr int ROWS = 50;
constexpr int COLS = 50;
constexpr int CELL_SIZE = 30; // pixels per grid cell

constexpr int ANIMATION_INTERVAL_MS = 1;

enum class AnimationPhase { Exploring, DrawingPath, Finished };

enum class CellType { Open, Wall, Start, End, Path, Explored };

enum class Algorithm { None, DFS, BFS, AStar };
Algorithm currentAlgo = Algorithm::None;

using Grid = std::vector<std::vector<CellType>>;

struct SearchResult {
    std::vector<Position> path;
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

SearchState initializeSearchState(const Grid& grid) {
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

bool isTraversable(const AppState& appstate, int row, int col) {
    return isInBounds(appstate.grid, row, col) && appstate.grid[row][col] != CellType::Wall;
}

auto createVisitedGrid(int rows, int cols) {
    return std::vector<std::vector<bool>>(rows, std::vector<bool>(cols, false));
}

SearchResult breadthFirstSearch(AppState& appstate) {
    SearchState searchState = initializeSearchState(appstate.grid);
    std::vector<std::vector<bool>> visited = createVisitedGrid(appstate.grid.size(), appstate.grid[0].size());

    std::queue<Position> q;
    q.push(appstate.start);
    visited[appstate.start.first][appstate.start.second] = true;

    while (!q.empty()) {
        Position current = q.front();
        searchState.explorationOrder.push_back(current);
        q.pop();

        if (current == appstate.end) {
            return {reconstructPath(searchState.cameFrom, appstate.end), searchState.explorationOrder};
        }

        for (const Position& direction : DIRECTIONS) {
            int nr = current.first + direction.first;
            int nc = current.second + direction.second;

            if (isTraversable(appstate, nr, nc) && !visited[nr][nc]) {
                visited[nr][nc] = true;
                // to get from this place, we came from the current
                searchState.cameFrom[nr][nc] = current;
                q.push({nr, nc});
            }
        }
    }

    return {{}, searchState.explorationOrder}; // no path found
}

SearchResult depthFirstSearch(AppState& appstate) {
    SearchState searchState = initializeSearchState(appstate.grid);
    std::vector<std::vector<bool>> visited = createVisitedGrid(appstate.grid.size(), appstate.grid[0].size());

    std::stack<Position> st;
    st.push(appstate.start);
    visited[appstate.start.first][appstate.start.second] = true;

    while (!st.empty()) {
        Position current = st.top();
        st.pop();
        searchState.explorationOrder.push_back(current);
        if (current == appstate.end) {
            return {reconstructPath(searchState.cameFrom, appstate.end), searchState.explorationOrder};
        }

        // backtrack in reverse order to maintain the original order of exploration
        for (int i = DIRECTIONS.size() - 1; i >= 0; --i) {
            const Position& direction = DIRECTIONS[i];
            int nr = current.first + direction.first;
            int nc = current.second + direction.second;

            if (isTraversable(appstate, nr, nc) && !visited[nr][nc]) {
                visited[nr][nc] = true;
                searchState.cameFrom[nr][nc] = current;
                st.push({nr, nc});
            }
        }
    }

    return {{}, searchState.explorationOrder}; // no path found
}

int manhattanDistance(const Position& current, const Position& goal) {
    return abs(goal.first - current.first) + abs(goal.second - current.second); // Manhattan distance
}

// Out of all the places I could go next, which one seems closest to the goal while also not being too expensive so far?
SearchResult aStarSearch(AppState& appstate) {
    // * g(n) → how far I’ve actually moved from start
    // * h(n) → guess to the goal (Manhattan distance)
    // * f(n) = g + h → “how promising this path looks”
    int rowCount = appstate.grid.size();
    int colCount = appstate.grid[0].size();
    
    SearchState searchState = initializeSearchState(appstate.grid);
    std::vector<std::vector<int>> gCost(rowCount, std::vector<int>(colCount, std::numeric_limits<int>::max())); // cost from start to this node

    std::priority_queue<std::pair<int, Position>, std::vector<std::pair<int, Position>>, std::greater<>> pq;//std::greater makes it a minheap.default is maxheap.
    
    int hStart = manhattanDistance(appstate.start, appstate.end);
    pq.push({hStart, appstate.start});

    gCost[appstate.start.first][appstate.start.second] = 0;

    while (!pq.empty()) {
        auto [f, current] = pq.top();
        pq.pop();
        
        int hCost = manhattanDistance(current, appstate.end);
        if (f > gCost[current.first][current.second] + hCost) {
            continue; // skip outdated entry
        }
        searchState.explorationOrder.push_back(current);
        
        if (current == appstate.end) {
            return {reconstructPath(searchState.cameFrom, appstate.end), searchState.explorationOrder};
        }
        
        int costtomove = 1;
        for (int i = 0; i < 4; ++i) {
            int nr = current.first + DIRECTIONS[i].first;
            int nc = current.second + DIRECTIONS[i].second;

            if (!isTraversable(appstate, nr, nc)) {
                continue;
            }
            
            int tentativeG = gCost[current.first][current.second] + costtomove; // distance from Start node to current node. cheapest way to get from start to current node. 
            if (tentativeG < gCost[nr][nc]) { // if the new path to neighbor is cheaper than any previous one
                gCost[nr][nc] = tentativeG; // update the cost to reach this neighbor
                searchState.cameFrom[nr][nc] = current; // update the path to reach this neighbor
                int hCost = manhattanDistance({nr, nc}, appstate.end); // heuristic cost to end node
                int fCost = tentativeG + hCost; // fCost = gCost + hCost
                
                pq.push({fCost, {nr, nc}}); // push the neighbor with its fCost as priority
            }
        }
    }

    return {{}, searchState.explorationOrder}; // no path found
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

void printTerminalGridAndStats(Grid& copy, SearchResult& result) {
// void printTerminalGridAndStats(Grid& copy, const std::vector<Position>& path, const std::vector<Position>& explorationOrder) {
    if (!result.path.empty()) {
        for (const Position& position : result.explorationOrder) {
            if (copy[position.first][position.second] != CellType::Start && copy[position.first][position.second] != CellType::End) {
                copy[position.first][position.second] = CellType::Explored;
            }
        }
        for (const Position& position : result.path) {
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
        std::cout << "Explored: " << result.explorationOrder.size() << " nodes.\n";
        std::cout << "Path Length: " << result.path.size() << ".\n";
        
    } else {
        std::cout << "No path found.\n";
    }
}

void runAlgorithm(AppState& appState, const Algorithm& algo) {
    if (appState.start.first == -1 || appState.end.first == -1) return;

    switch (algo) {
        case Algorithm::DFS:
            appState.result = depthFirstSearch(appState);
            break;
        case Algorithm::BFS:
            appState.result = breadthFirstSearch(appState);
            break;
        case Algorithm::AStar:
            appState.result = aStarSearch(appState);
            break;
        default:
            break;
    }
}

std::string algorithmName(const Algorithm& algo) {
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
void clearSearchResult(SearchResult& result) {
    result.path.clear();
    result.explorationOrder.clear();
}

void resetAnimationStates(AnimationState& animationState) {
    animationState.animatedGrid = std::vector<std::vector<bool>>(ROWS, std::vector<bool>(COLS, false));
    animationState.animatedPath = std::vector<bool>(ROWS * COLS, false);
    animationState.animationStep = 0;
    animationState.clock.restart();
}

void resetSearchState(SearchResult& result,
                 AnimationState& animationState) {

    clearSearchResult(result);
    resetAnimationStates(animationState);
}

bool validateGrid(AppState& appState) {
    if (appState.grid.empty() || appState.grid[0].empty()) {
        std::cerr << "Invalid or empty grid.\n";
        return false;
    }
    
    appState.start = findCellPosition(appState.grid, CellType::Start);
    appState.end = findCellPosition(appState.grid, CellType::End);
    
    if (appState.start.first == -1 || appState.end.first == -1) {
        std::cerr << "Start or End not found.\n";
        return false;
    }

    if (std::count_if(appState.grid.begin(), appState.grid.end(), [](const std::vector<CellType>& row) {
        return std::count(row.begin(), row.end(), CellType::Start) > 1;
    }) > 0 || std::count_if(appState.grid.begin(), appState.grid.end(), [](const std::vector<CellType>& row) {
        return std::count(row.begin(), row.end(), CellType::End) > 1;
    }) > 0) {
        std::cerr << "More than one Start or End found.\n";
        return false;
    }

    return true;
}

int askForAlgorithmChoice() {
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
        return askForAlgorithmChoice(); // recursively ask again
    }

    return algorithmChoice;
}

bool handleAlgorithmChoice(AppState& appState, int algorithmChoice) {
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
    runAlgorithm(appState, currentAlgo);
    return true;
}

void runTerminalMode(AppState& appState) {
    appState.grid = readFromFile("grids/biggrid.txt");
    
    if (!validateGrid(appState)) {
        return;
    }

    int algorithmChoice {askForAlgorithmChoice()};
    
    if (!handleAlgorithmChoice(appState, algorithmChoice)) {
        return;
    }
    
    Grid copy = appState.grid;
    printTerminalGridAndStats(copy, appState.result);
}

void resetGrid(Grid& grid) {
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            grid[r][c] = CellType::Open;
        }
    }
}

void handleKeyboardEvent(AppState& appState, AnimationState& animationState, const std::optional<sf::Event>& event, sf::RenderWindow& window) {
    if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
        if ((keyPressed->code == sf::Keyboard::Key::Num1 ||
                keyPressed->code == sf::Keyboard::Key::Num2 ||
                keyPressed->code == sf::Keyboard::Key::Num3) &&
            (appState.start.first == -1 || appState.end.first == -1)) {

            std::cout << "Set start and end first!\n";
            return;
        }

        switch (keyPressed->code) {
            case sf::Keyboard::Key::Escape:
                window.close();
                break;
            case sf::Keyboard::Key::Num1:
                std::cout << "Using " << algorithmName(Algorithm::DFS) << ".\n";
                currentAlgo = Algorithm::DFS;
                appState.gridChanged = true;
                appState.statsRequested = true;
                break;
            case sf::Keyboard::Key::Num2:
                std::cout << "Using " << algorithmName(Algorithm::BFS) << ".\n";
                currentAlgo = Algorithm::BFS;
                appState.gridChanged = true;
                appState.statsRequested = true;
                break;
            case sf::Keyboard::Key::Num3:
                std::cout << "Using " << algorithmName(Algorithm::AStar) << ".\n";
                currentAlgo = Algorithm::AStar;
                appState.gridChanged = true;
                appState.statsRequested = true;
                break;
            case sf::Keyboard::Key::C:
                std::cout << "Clearing the entire grid.\n";
                appState.gridChanged = true;
                currentAlgo = Algorithm::None;
                appState.start = {-1, -1};
                appState.end = {-1, -1};
                
                appState.statsRequested = false;

                resetGrid(appState.grid);
                resetSearchState(appState.result, animationState);
                break;
            case sf::Keyboard::Key::LShift:
                break;
            default:
                std::cout << "Unknown key pressed.\n";
                break;
        }
    }
}

void updateCellType(AppState& appState, AnimationState& animationState, int row, int col, CellType requestedCellType) {
    switch(requestedCellType) {
        case CellType::Wall:
            if (appState.grid[row][col] == CellType::Start || appState.grid[row][col] == CellType::End) {
                std::cout << "Cannot set Wall on Start or End.\n";
                return;
            }

            if (appState.grid[row][col] == CellType::Wall) {
                appState.grid[row][col] = CellType::Open;
            } else {
                appState.grid[row][col] = CellType::Wall;
            }

            appState.statsRequested = false;
            break;
        case CellType::Start:
            if (appState.grid[row][col] == CellType::End) {
                std::cout << "Cannot set Start on End.\n";
                return;
            }
             for (auto& r : appState.grid)
                for (auto& c : r)
                    if (c == CellType::Start) c = CellType::Open;
            
            appState.grid[row][col] = CellType::Start;
            appState.statsRequested = true;
            appState.start = {row, col};
            break;
        case CellType::End:
            if (appState.grid[row][col] == CellType::Start) {
                std::cout << "Cannot set End on Start.\n";
                return;
            }
            for (auto& r : appState.grid)
                for (auto& c : r)
                    if (c == CellType::End) c = CellType::Open;

            appState.grid[row][col] = CellType::End;
            appState.statsRequested = true;
            appState.end = {row, col};
            break;
        default:
            std::cout << "Unknown cell type.\n";
            break;
    }

    appState.gridChanged = true;
    resetSearchState(appState.result, animationState);
}

void handleMouseEvent(AppState& appState, AnimationState& animationState, const std::optional<sf::Event>& event) {
    if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
        int col = mousePressed->position.x / CELL_SIZE;
        int row = mousePressed->position.y / CELL_SIZE;

        if (row >= 0 && row < ROWS && col >= 0 && col < COLS) {
            if (mousePressed->button == sf::Mouse::Button::Left) {
                updateCellType(appState, animationState, row, col, CellType::Wall);
            }
            else if (mousePressed->button == sf::Mouse::Button::Right &&
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
                updateCellType(appState, animationState, row, col, CellType::Start);
            }
            else if (mousePressed->button == sf::Mouse::Button::Right) {
                updateCellType(appState, animationState, row, col, CellType::End);
            }
            else { 
                std::cout << "Unknown mouse button pressed.\n";
            }
        }
    }
}

void printPerformanceStats(const SearchResult& result, const Algorithm& currentAlgo, double executionTime) {
    std::cout << "\n--- Performance Stats ---\n";
    std::cout << "Algorithm: " << algorithmName(currentAlgo) << "\n";
    std::cout << "Execution Time: " << executionTime << " ms\n";
    std::cout << "Total Nodes Explored: " << result.explorationOrder.size() << "\n";
    std::cout << "Path Length: " << result.path.size() << "\n";
}

double measureExecutionTime(std::function<void()> func) {
    auto start = std::chrono::steady_clock::now();
    func();
    auto end = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}


void displayPerformanceStats(AppState& appState,
                                const Algorithm& currentAlgo, double executionTime) {

    if (appState.statsRequested && currentAlgo != Algorithm::None) {
        printPerformanceStats(appState.result, currentAlgo, executionTime);
    }
}

AnimationPhase getAnimationPhase(const AnimationState& animationState, const AppState& appState) {
    const int explorationSize = static_cast<int>(appState.result.explorationOrder.size());
    const int totalSteps = explorationSize + static_cast<int>(appState.result.path.size());
    if (animationState.animationStep < explorationSize) {
        return AnimationPhase::Exploring;
    }
    if (animationState.animationStep < totalSteps) {
        return AnimationPhase::DrawingPath;
    }
    return AnimationPhase::Finished;
}

void updateAnimation(AnimationState& animationState, const AppState& appState) {
    // check if its time to update first before doing anything else
    if (animationState.clock.getElapsedTime().asMilliseconds() <= ANIMATION_INTERVAL_MS) {
        return;
    }

    switch (getAnimationPhase(animationState, appState)) {
        case AnimationPhase::Exploring: {
            const auto position = appState.result.explorationOrder[animationState.animationStep];
            animationState.animatedGrid[position.first][position.second] = true;
            break;
        }   

        case AnimationPhase::DrawingPath: {
            const int explorationSize = static_cast<int>(appState.result.explorationOrder.size());
            const int pathIndex = animationState.animationStep - explorationSize;
            const auto position = appState.result.path[pathIndex];
            animationState.animatedPath[position.first * COLS + position.second] = true;
            break;
        }

        case AnimationPhase::Finished:
            return;
    }
    ++animationState.animationStep;
    animationState.clock.restart();
}

void eventGUI(sf::RenderWindow& window, AppState& appState, AnimationState& animationState, const Algorithm& currentAlgo) {
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }

        handleKeyboardEvent(appState, animationState, event, window);
        handleMouseEvent(appState, animationState, event);
    }

    if (appState.gridChanged) {
        double executionTime = measureExecutionTime([&]() {
            runAlgorithm(appState, currentAlgo);
        });
        
        // std::cout << appState.result.totalNodesExplored << " vs "
        //   << appState.result.explorationOrder.size() << "\n";
          
        displayPerformanceStats(appState, currentAlgo, executionTime);

        resetAnimationStates(animationState);
        appState.gridChanged = false;
    }

    updateAnimation(animationState, appState);
}



void setCellColor(sf::RectangleShape& cell, CellType cellType, const AnimationState& animationState, int row, int col) {
    switch (cellType) {
        case CellType::Open:  cell.setFillColor(sf::Color::White); break;
        case CellType::Wall:  cell.setFillColor(sf::Color::Black); break;
        case CellType::Path:  cell.setFillColor(sf::Color::Yellow); break;
        case CellType::Explored:  cell.setFillColor(sf::Color::Cyan); break;
        case CellType::Start: cell.setFillColor(sf::Color::Green); break;
        case CellType::End:   cell.setFillColor(sf::Color::Blue); break;
    }

    if (animationState.animatedGrid[row][col] && cellType != CellType::Start && cellType != CellType::End) {
        cell.setFillColor(sf::Color::Cyan);
    }
    
    if (animationState.animatedPath[row * COLS + col] && cellType != CellType::Start && cellType != CellType::End) {
        cell.setFillColor(sf::Color::Red);
    }
}

void renderGUI(sf::RenderWindow& window, const AppState& appState, const AnimationState& animationState) {
    for (int row {0}; row < ROWS; ++row) {
        for (int col {0}; col < COLS; ++col) {

            sf::RectangleShape cell({CELL_SIZE, CELL_SIZE});
            cell.setPosition({col * CELL_SIZE * 1.f, row * CELL_SIZE * 1.f});

            setCellColor(cell, appState.grid[row][col], animationState, row, col);

            cell.setOutlineThickness(1);
            cell.setOutlineColor(sf::Color(200, 200, 200));

            window.draw(cell);
        }
    }
}

void runGuiMode(AppState& appState,
               sf::RenderWindow& window) {
    
    AnimationState animationState;

    while (window.isOpen()) {

        // --- EVENT ---
        eventGUI(window, appState, animationState, currentAlgo);

        // --- RENDER ---
        window.clear(sf::Color::White);

        renderGUI(window, appState, animationState);

        window.display();
    }
}

void runApp(AppState& appState) {
    int choice {};
    std::cout << "Choose mode: 1 for Terminal, 2 for GUI: ";
    std::cin >> choice;
    if (choice == 1) {
        runTerminalMode(appState);
    } else {
        sf::RenderWindow window(
            sf::VideoMode({COLS * CELL_SIZE, ROWS * CELL_SIZE}),
            "Pathfinding Visualizer"
        );
        runGuiMode(appState, window);
    }
}


int main() {

    AppState appState;
    appState.grid = Grid(ROWS, std::vector<CellType>(COLS, CellType::Open));

    runApp(appState);
    return 0;
}
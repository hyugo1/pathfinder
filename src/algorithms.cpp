#include "algorithms.hpp"

#include "grid.hpp"
#include <queue>
#include <stack>
#include <array>
#include <algorithm>
#include <limits>
#include <functional>
#include <ios>

SearchState initializeSearchState(const Grid& grid) {
    return {
        std::vector<std::vector<Position>>(grid.size(), std::vector<Position>(grid[0].size(), {-1, -1})),
        {}
    };
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


SearchResult breadthFirstSearch(const Grid& grid, Position start, Position end) {
    SearchState searchState = initializeSearchState(grid);
    std::vector<std::vector<bool>> visited = createVisitedGrid(grid.size(), grid[0].size());

    std::queue<Position> q;
    q.push(start);
    visited[start.first][start.second] = true;

    while (!q.empty()) {
        Position current = q.front();
        searchState.explorationOrder.push_back(current);
        q.pop();

        if (current == end) {
            return {reconstructPath(searchState.cameFrom, end), searchState.explorationOrder};
        }

        for (const Position& direction : DIRECTIONS) {
            int nr = current.first + direction.first;
            int nc = current.second + direction.second;

            if (isTraversable(grid, nr, nc) && !visited[nr][nc]) {
                visited[nr][nc] = true;
                // to get from this place, we came from the current
                searchState.cameFrom[nr][nc] = current;
                q.push({nr, nc});
            }
        }
    }

    return {{}, searchState.explorationOrder}; // no path found
}

//Your DFS marks cells visited when pushing them, not when popping. It still finds a valid path and is correct, but the exploration order and the path won’t match a textbook DFS.
SearchResult depthFirstSearch(const Grid& grid, Position start, Position end) {
    SearchState searchState = initializeSearchState(grid);
    std::vector<std::vector<bool>> visited = createVisitedGrid(grid.size(), grid[0].size());

    std::stack<Position> st;
    st.push(start);

    while (!st.empty()) {
        Position current = st.top();
        st.pop();
        if (visited[current.first][current.second]) {
            continue;
        }
        visited[current.first][current.second] = true;
        searchState.explorationOrder.push_back(current);
        if (current == end) {
            return {reconstructPath(searchState.cameFrom, end), searchState.explorationOrder};
        }

        // backtrack in reverse order to maintain the original order of exploration
        for (int i = DIRECTIONS.size() - 1; i >= 0; --i) {
            const Position& direction = DIRECTIONS[i];
            int nr = current.first + direction.first;
            int nc = current.second + direction.second;

            if (isTraversable(grid, nr, nc) && !visited[nr][nc]) {
                searchState.cameFrom[nr][nc] = current;
                st.push({nr, nc});
            }
        }
    }

    return {{}, searchState.explorationOrder}; // no path found
}

int manhattanDistance(const Position& current, const Position& goal) {
    return std::abs(goal.first - current.first) + std::abs(goal.second - current.second); // Manhattan distance
}

// Out of all the places I could go next, which one seems closest to the goal while also not being too expensive so far?
SearchResult aStarSearch(const Grid& grid, Position start, Position end) {
    // * g(n) → how far I’ve actually moved from start
    // * h(n) → guess to the goal (Manhattan distance)
    // * f(n) = g + h → “how promising this path looks”
    int rowCount = grid.size();
    int colCount = grid[0].size();
    SearchState searchState = initializeSearchState(grid);
    std::vector<std::vector<int>> gCost(rowCount, std::vector<int>(colCount, std::numeric_limits<int>::max())); // cost from start to this node
    std::priority_queue<std::pair<int, Position>, std::vector<std::pair<int, Position>>, std::greater<>> pq;//std::greater makes it a minheap.default is maxheap.
    
    int hStart = manhattanDistance(start, end);
    pq.push({hStart, start});

    gCost[start.first][start.second] = 0;

    while (!pq.empty()) {
        auto [f, current] = pq.top();
        pq.pop();
        
        int hCost = manhattanDistance(current, end);
        if (f > gCost[current.first][current.second] + hCost) {
            continue; // skip outdated entry
        }
        searchState.explorationOrder.push_back(current);
        
        if (current == end) {
            return {reconstructPath(searchState.cameFrom, end), searchState.explorationOrder};
        }
        
        int costtomove = 1;
        for (int i = 0; i < 4; ++i) {
            int nr = current.first + DIRECTIONS[i].first;
            int nc = current.second + DIRECTIONS[i].second;

            if (!isTraversable(grid, nr, nc)) {
                continue;
            }
            
            int tentativeG = gCost[current.first][current.second] + costtomove; // distance from Start node to current node. cheapest way to get from start to current node. 
            if (tentativeG < gCost[nr][nc]) { // if the new path to neighbor is cheaper than any previous one
                gCost[nr][nc] = tentativeG; // update the cost to reach this neighbor
                searchState.cameFrom[nr][nc] = current; // update the path to reach this neighbor
                int hCost = manhattanDistance({nr, nc}, end); // heuristic cost to end node
                int fCost = tentativeG + hCost; // fCost = gCost + hCost
                
                pq.push({fCost, {nr, nc}}); // push the neighbor with its fCost as priority
            }
        }
    }

    return {{}, searchState.explorationOrder}; // no path found
}

// BFS minimizes the number of steps, whereas Dijkstra minimizes the total movement cost.
SearchResult djikstra(const Grid& grid, Position start, Position end) {
    // * g(n) → how far I’ve actually moved from start
    int rowCount = grid.size();
    int colCount = grid[0].size();
    SearchState searchState = initializeSearchState(grid);
    std::vector<std::vector<int>> gCost(rowCount, std::vector<int>(colCount, std::numeric_limits<int>::max())); // cost from start to this node
    std::priority_queue<std::pair<int, Position>, std::vector<std::pair<int, Position>>, std::greater<>> pq;//std::greater makes it a minheap.default is maxheap.

    gCost[start.first][start.second] = 0;
    pq.push({0, start});

    while (!pq.empty()) {
        auto [g, current] = pq.top();
        pq.pop();
        
        if (g > gCost[current.first][current.second]) {
            continue; // skip outdated entry
        }
        searchState.explorationOrder.push_back(current);
        
        if (current == end) {
            return {reconstructPath(searchState.cameFrom, end), searchState.explorationOrder};
        }
        
        int costtomove = 1;
        for (int i = 0; i < 4; ++i) {
            int nr = current.first + DIRECTIONS[i].first;
            int nc = current.second + DIRECTIONS[i].second;

            if (!isTraversable(grid, nr, nc)) {
                continue;
            }
            
            int tentativeG = gCost[current.first][current.second] + costtomove;
            if (tentativeG < gCost[nr][nc]) {
                gCost[nr][nc] = tentativeG;
                searchState.cameFrom[nr][nc] = current;
                
                pq.push({tentativeG, {nr, nc}});
            }
        }
    }

    return {{}, searchState.explorationOrder}; // no path found
}

SearchResult runSearch(const Grid& grid, Position start, Position end, Algorithm algo) {
    switch (algo) {
        case Algorithm::DFS:   return depthFirstSearch(grid, start, end);
        case Algorithm::BFS:   return breadthFirstSearch(grid, start, end);
        case Algorithm::AStar: return aStarSearch(grid, start, end);
        case Algorithm::Djikstra: return djikstra(grid, start, end);
        default:               return {};
    }
}

void clearSearchResult(SearchResult& result) {
    result.path.clear();
    result.explorationOrder.clear();
}
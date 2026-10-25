// appstate.cpp

#include "appstate.hpp"
#include <iostream>

void resetAnimationStates(AnimationState& animationState) {
    animationState.animatedGrid = std::vector<std::vector<bool>>(ROWS, std::vector<bool>(COLS, false));
    animationState.animatedPath = std::vector<bool>(ROWS * COLS, false);
    animationState.animationStep = 0;
    animationState.clock = std::chrono::steady_clock::now();
}

void resetSearchState(SearchResult& result, AnimationState& animationState) {
    clearSearchResult(result);
    resetAnimationStates(animationState);
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

double measureExecutionTime(std::function<void()> func) {
    auto start = std::chrono::steady_clock::now();
    func();
    auto end = std::chrono::steady_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

void displayPerformanceStats(bool statsRequested, const Algorithm& chosenAlgo, const SearchResult& result, double executionTime) {
    if (statsRequested && chosenAlgo != Algorithm::None) {
        printPerformanceStats(result, chosenAlgo, executionTime);
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
    auto end = std::chrono::steady_clock::now();
    double elapsed = std::chrono::duration<double, std::milli>(
        end - animationState.clock
    ).count();

    if (elapsed < ANIMATION_INTERVAL_MS) {
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
    animationState.clock = std::chrono::steady_clock::now();
}

//appstate.hpp

#pragma once

#include "grid.hpp"
#include "algorithms.hpp"
#include <chrono>
#include <functional>


constexpr int ANIMATION_INTERVAL_MS = 1;
constexpr int CELL_SIZE = 20; // pixels per grid cell

enum class AnimationPhase { Exploring, DrawingPath, Finished };

struct AppState {
    Grid grid;
    Position start {-1, -1};
    Position end {-1, -1};
    SearchResult result;
    Algorithm chosenAlgo {Algorithm::None};
    bool gridChanged {false};
    bool statsRequested {false};
    int latestVersion {0};
};

struct AnimationState {
    std::vector<std::vector<bool>> animatedGrid = std::vector<std::vector<bool>>(ROWS, std::vector<bool>(COLS, false));
    std::vector<bool> animatedPath = std::vector<bool>(ROWS * COLS, false);
    int animationStep {0};
    std::chrono::steady_clock::time_point clock = std::chrono::steady_clock::now();
};


void resetAnimationStates(AnimationState& animationState);
void resetSearchState(SearchResult& result, AnimationState& animationState);
void updateCellType(AppState& appState, AnimationState& animationState, int row, int col, CellType requestedCellType);
double measureExecutionTime(std::function<void()> func);
void printPerformanceStats(const SearchResult& result, const Algorithm& chosenAlgo, double executionTime);
void displayPerformanceStats(bool statsRequested, const Algorithm& chosenAlgo, const SearchResult& result, double executionTime);
AnimationPhase getAnimationPhase(const AnimationState& animationState, const AppState& appState);
void updateAnimation(AnimationState& animationState, const AppState& appState);
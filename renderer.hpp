#pragma once

#include "grid.hpp"
#include "algorithms.hpp"
#include "appstate.hpp"
#include "SearchWorker.hpp"
#include <SFML/Graphics.hpp>

void handleKeyboardEvent(AppState& appState, AnimationState& animationState, const std::optional<sf::Event>& event, sf::RenderWindow& window);
void handleMouseEvent(AppState& appState, AnimationState& animationState, const std::optional<sf::Event>& event);
void processWorkerResult(AppState& appState, AnimationState& animationState, SearchWorker& worker);
void processGridChanges(AppState& appState, AnimationState& animationState, bool threadedMode
, SearchWorker& worker);
void processGUIFrame(sf::RenderWindow& window, AppState& appState, AnimationState
& animationState, bool threadedMode, SearchWorker& worker);
void renderGUI(sf::RenderWindow& window, const AppState& appState, const AnimationState& animationState);
void runGuiMode(AppState& appState, sf::RenderWindow& window, bool threadedMode);
void setCellColor(sf::RectangleShape& cell, CellType cellType, const AnimationState& animationState, int row, int col);

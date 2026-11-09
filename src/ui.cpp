#include "ui.hpp"
#include "searchworker.hpp"
#include "grid.hpp"
 
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>

UI::UI() {
    buttonList.emplace_back(sf::FloatRect({COLS * CELL_SIZE + PANEL_SPACE, 10.f}, {BUTTON_WIDTH, BUTTON_HEIGHT}), "Clear", UiAction::Clear);
    buttonList.emplace_back(sf::FloatRect({COLS * CELL_SIZE + PANEL_SPACE, 60.f}, {BUTTON_WIDTH, BUTTON_HEIGHT}), "Generate", UiAction::Generate);
    buttonList.emplace_back(sf::FloatRect({COLS * CELL_SIZE + PANEL_SPACE, 110.f}, {BUTTON_WIDTH, BUTTON_HEIGHT}), "DFS", UiAction::SelectDFS);
    buttonList.emplace_back(sf::FloatRect({COLS * CELL_SIZE + PANEL_SPACE, 160.f}, {BUTTON_WIDTH, BUTTON_HEIGHT}), "BFS", UiAction::SelectBFS);
    buttonList.emplace_back(sf::FloatRect({COLS * CELL_SIZE + PANEL_SPACE, 210.f}, {BUTTON_WIDTH, BUTTON_HEIGHT}), "A*", UiAction::SelectAStar);
    buttonList.emplace_back(sf::FloatRect({COLS * CELL_SIZE + PANEL_SPACE, 260.f}, {BUTTON_WIDTH, BUTTON_HEIGHT}), "Dijkstra", UiAction::SelectDijkstra);
    buttonList.emplace_back(sf::FloatRect({COLS * CELL_SIZE + PANEL_SPACE, 310.f}, {BUTTON_WIDTH, BUTTON_HEIGHT}), "Toggle Delay", UiAction::ToggleDelay);
    buttonList.emplace_back(sf::FloatRect({COLS * CELL_SIZE + PANEL_SPACE, 360.f}, {BUTTON_WIDTH, BUTTON_HEIGHT}), "Toggle Seed Lock", UiAction::ToggleSeedLock);
}

void UI::draw(sf::RenderWindow& window) {
    for (const Button& b : buttonList) {
        sf::RectangleShape box(b.rect.size);
        box.setPosition(b.rect.position);
        box.setFillColor(sf::Color(200, 200, 200));
        window.draw(box);
    }
}

UiAction UI::handleClick(sf::Vector2i pos) {
    for (const Button& b : buttonList) {
        if (b.rect.contains(static_cast<sf::Vector2f>(pos))) {
            return b.action;
        }
    }
    return UiAction::None;
}
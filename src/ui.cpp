#include "ui.hpp"
#include "searchworker.hpp"
#include "grid.hpp"
 
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>

UI::UI() {
    buttonList.emplace_back(sf::FloatRect({COLS * CELL_SIZE + 10.f, 10.f}, {100.f, 40.f}), "Clear", UiAction::Clear);
    buttonList.emplace_back(sf::FloatRect({COLS * CELL_SIZE + 10.f, 60.f}, {100.f, 40.f}), "Generate", UiAction::Generate);
    buttonList.emplace_back(sf::FloatRect({COLS * CELL_SIZE + 10.f, 110.f}, {100.f, 40.f}), "DFS", UiAction::SelectDFS);
    buttonList.emplace_back(sf::FloatRect({COLS * CELL_SIZE + 10.f, 160.f}, {100.f, 40.f}), "BFS", UiAction::SelectBFS);
    buttonList.emplace_back(sf::FloatRect({COLS * CELL_SIZE + 10.f, 210.f}, {100.f, 40.f}), "A*", UiAction::SelectAStar);
    buttonList.emplace_back(sf::FloatRect({COLS * CELL_SIZE + 10.f, 260.f}, {100.f, 40.f}), "Dijkstra", UiAction::SelectDijkstra);
    buttonList.emplace_back(sf::FloatRect({COLS * CELL_SIZE + 10.f, 310.f}, {100.f, 40.f}), "Toggle Delay", UiAction::ToggleDelay);
    buttonList.emplace_back(sf::FloatRect({COLS * CELL_SIZE + 10.f, 360.f}, {100.f, 40.f}), "Toggle Seed Lock", UiAction::ToggleSeedLock);
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
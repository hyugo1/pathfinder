#include "ui.hpp"
#include "searchworker.hpp"
#include "grid.hpp"
 
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <array>

UI::UI() {
    buttonList.emplace_back(sf::FloatRect({COLS * CELL_SIZE + PANEL_SPACE, PANEL_SPACE}, {BUTTON_WIDTH, BUTTON_HEIGHT}), "Clear", UiAction::Clear, sf::Color(240, 250, 130));
    buttonList.emplace_back(sf::FloatRect({COLS * CELL_SIZE + PANEL_SPACE, PANEL_SPACE + BUTTON_SPACE}, {BUTTON_WIDTH, BUTTON_HEIGHT}), "Generate", UiAction::Generate, sf::Color(235, 91,  91));
    buttonList.emplace_back(sf::FloatRect({COLS * CELL_SIZE + PANEL_SPACE, PANEL_SPACE + 2 * BUTTON_SPACE}, {BUTTON_WIDTH, BUTTON_HEIGHT}), "DFS", UiAction::SelectDFS, sf::Color(86,  240, 114));
    buttonList.emplace_back(sf::FloatRect({COLS * CELL_SIZE + PANEL_SPACE, PANEL_SPACE + 3 * BUTTON_SPACE}, {BUTTON_WIDTH, BUTTON_HEIGHT}), "BFS", UiAction::SelectBFS, sf::Color(86,  240, 114));
    buttonList.emplace_back(sf::FloatRect({COLS * CELL_SIZE + PANEL_SPACE, PANEL_SPACE + 4 * BUTTON_SPACE}, {BUTTON_WIDTH, BUTTON_HEIGHT}), "A*", UiAction::SelectAStar, sf::Color(86,  240, 114));
    buttonList.emplace_back(sf::FloatRect({COLS * CELL_SIZE + PANEL_SPACE, PANEL_SPACE + 5 * BUTTON_SPACE}, {BUTTON_WIDTH, BUTTON_HEIGHT}), "Dijkstra", UiAction::SelectDijkstra, sf::Color(86,  240, 114));
    buttonList.emplace_back(sf::FloatRect({COLS * CELL_SIZE + PANEL_SPACE, PANEL_SPACE + 6 * BUTTON_SPACE}, {BUTTON_WIDTH, BUTTON_HEIGHT}), "Delay", UiAction::ToggleDelay, sf::Color(240, 86,  230));
    buttonList.emplace_back(sf::FloatRect({COLS * CELL_SIZE + PANEL_SPACE, PANEL_SPACE + 7 * BUTTON_SPACE}, {BUTTON_WIDTH, BUTTON_HEIGHT}), "Seed Lock", UiAction::ToggleSeedLock, sf::Color(240, 86,  230));


    if (!font.openFromFile("assets/ChrustyRock.ttf")) {
        std::cout << "Custom font failed. Using Roboto instead.\n";
        if (!font.openFromFile("assets/Roboto-Regular.ttf")) {
            throw std::runtime_error("Failed to load both fonts!");
        }
    }
}

void UI::draw(sf::RenderWindow& window) {
    for (const Button& b : buttonList) {
        sf::RectangleShape box(b.rect.size);
        box.setPosition(b.rect.position);
        box.setFillColor(b.color);

        sf::Text label(font, b.text, 15); 
        label.setFillColor(sf::Color::Black);
        
        // Center the label inside b.rect
        sf::FloatRect bounds = label.getLocalBounds();
        label.setOrigin(bounds.position + bounds.size / 2.f);
        label.setPosition(b.rect.getCenter());

        window.draw(box);
        window.draw(label);
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
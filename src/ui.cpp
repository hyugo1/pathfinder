#include "ui.hpp"
#include "searchworker.hpp"
#include "grid.hpp"
 
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <array>

UI::UI() {
    buttonList.emplace_back(sf::FloatRect({COLS * CELL_SIZE + PANEL_SPACE, 10.f}, {BUTTON_WIDTH, BUTTON_HEIGHT}), "Clear", UiAction::Clear);
    buttonList.emplace_back(sf::FloatRect({COLS * CELL_SIZE + PANEL_SPACE, 60.f}, {BUTTON_WIDTH, BUTTON_HEIGHT}), "Generate", UiAction::Generate);
    buttonList.emplace_back(sf::FloatRect({COLS * CELL_SIZE + PANEL_SPACE, 110.f}, {BUTTON_WIDTH, BUTTON_HEIGHT}), "DFS", UiAction::SelectDFS);
    buttonList.emplace_back(sf::FloatRect({COLS * CELL_SIZE + PANEL_SPACE, 160.f}, {BUTTON_WIDTH, BUTTON_HEIGHT}), "BFS", UiAction::SelectBFS);
    buttonList.emplace_back(sf::FloatRect({COLS * CELL_SIZE + PANEL_SPACE, 210.f}, {BUTTON_WIDTH, BUTTON_HEIGHT}), "A*", UiAction::SelectAStar);
    buttonList.emplace_back(sf::FloatRect({COLS * CELL_SIZE + PANEL_SPACE, 260.f}, {BUTTON_WIDTH, BUTTON_HEIGHT}), "Dijkstra", UiAction::SelectDijkstra);
    buttonList.emplace_back(sf::FloatRect({COLS * CELL_SIZE + PANEL_SPACE, 310.f}, {BUTTON_WIDTH, BUTTON_HEIGHT}), "Delay", UiAction::ToggleDelay);
    buttonList.emplace_back(sf::FloatRect({COLS * CELL_SIZE + PANEL_SPACE, 360.f}, {BUTTON_WIDTH, BUTTON_HEIGHT}), "Seed Lock", UiAction::ToggleSeedLock);


    if (!font.openFromFile("assets/ChrustyRock.ttf")) {
        throw std::runtime_error("Failed to load specific font");
    }
}

void UI::draw(sf::RenderWindow& window) {
    static const std::array<UiAction, 9> actions = {UiAction::None, UiAction::Generate, UiAction::Clear, UiAction::SelectDFS, UiAction::SelectBFS, UiAction::SelectAStar, UiAction::SelectDijkstra, UiAction::ToggleDelay, UiAction::ToggleSeedLock};

    for (const Button& b : buttonList) {
        sf::RectangleShape box(b.rect.size);
        box.setPosition(b.rect.position);

        for (int i = 0; i < actions.size(); ++i) {
            if (b.action == actions[i]) {
                box.setFillColor(buttonColors[i]);
                break;
            }
        }

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
#include "ui.hpp"
#include "searchworker.hpp"
#include "grid.hpp"
#include "console.hpp"
#include "algorithms.hpp"
 
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <random>
#include <sstream>
#include <array>
#include <stdexcept>

UI::UI() {
    const float x = COLS * CELL_SIZE + PANEL_SPACE;
    auto addButton = [&](int row, std::string text, UiAction action, sf::Color color) {
        buttonList.emplace_back(
            sf::FloatRect({x, PANEL_SPACE + row * BUTTON_SPACE}, {BUTTON_WIDTH, BUTTON_HEIGHT}), text, action, color);
    };

    addButton(0, "Clear", UiAction::Clear, sf::Color(240, 250, 130));
    addButton(1, "Generate", UiAction::Generate, sf::Color(235, 91, 91));
    addButton(2, "DFS", UiAction::SelectDFS, sf::Color(86, 240, 114));
    addButton(3, "BFS", UiAction::SelectBFS, sf::Color(86, 240, 114));
    addButton(4, "A Star", UiAction::SelectAStar, sf::Color(86, 240, 114));
    addButton(5, "Dijkstra", UiAction::SelectDijkstra, sf::Color(86, 240, 114));
    addButton(6, "Delay Off", UiAction::ToggleDelay, sf::Color(240, 86, 230));
    // addButton(7, "Seed Lock", UiAction::ToggleSeedLock, sf::Color(240, 86,  230));


    auto addTextBox = [&](int row, std::string text, StatType s, sf::Color color) {
        textboxList.emplace_back(
            sf::FloatRect({x, PANEL_SPACE + row * TEXTBOX_SPACE}, {TEXTBOX_WIDTH, TEXTBOX_HEIGHT}), text, s, color);
    };

    addTextBox(10, "Algorithm: ", StatType::AlgorithmName, sf::Color(180, 208, 242));
    addTextBox(11, "Nodes Explored: ", StatType::TotalNodesExplored, sf::Color(180, 208, 242));
    addTextBox(12, "Total Cost: ", StatType::TotalCost, sf::Color(180, 208, 242));
    addTextBox(13, "Path Length: ", StatType::PathLength, sf::Color(180, 208, 242));

    if (!font.openFromFile("assets/OpenSans-Regular.ttf")) {
        throw std::runtime_error("Failed to load both fonts!");
    }
}

void UI::draw(sf::RenderWindow& window, const AppState& appState) {
    for (const Button& b : buttonList) {
        sf::RectangleShape box(b.rect.size);
        box.setPosition(b.rect.position);
        sf::Color color = b.color;
        std::string labelText = b.text;
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        
        if (b.rect.contains(static_cast<sf::Vector2f>(mousePos))) {
            //make color lighter
            color.r = std::min(255, color.r + 40);
            color.g = std::min(255, color.g + 40);
            color.b = std::min(255, color.b + 40);
        }
        
        if (THREAD_SLEEP_SECONDS == 2 && b.action == UiAction::ToggleDelay) {
            color.r = 152;
            color.g = 86;
            color.b = 230;
            labelText = "Delay On";
        }

        box.setFillColor(color);
        
        sf::Text label(font, labelText, 15); 
        label.setFillColor(sf::Color::Black);
        
        // Center the label inside b.rect
        sf::FloatRect bounds = label.getLocalBounds();
        label.setOrigin(bounds.position + bounds.size / 2.f);
        label.setPosition(b.rect.getCenter());

        window.draw(box);
        window.draw(label);
    }

    for (const TextBox& t : textboxList) {
        sf::RectangleShape box(t.rect.size);
        box.setPosition(t.rect.position);
        sf::Color color = t.color;
        std::string labelText = t.text;
        box.setFillColor(color);
        
        switch(t.statType) {
            case StatType::AlgorithmName:
                labelText = "Algorithm: " + algorithmName(appState.chosenAlgo);
                break;
            case StatType::TotalNodesExplored:
                labelText = "Nodes Explored: " + std::to_string(appState.result.explorationOrder.size());
                break;
            case StatType::TotalCost:
                labelText = "Total Cost: " + std::to_string(appState.result.totalCost);
                break;
            case StatType::PathLength:
                labelText = "Path Length: " + std::to_string(appState.result.path.size());
                break;
        }

        sf::Text label(font, labelText, 15); 
        label.setFillColor(sf::Color::Black);

        // Center the label inside b.rect
        sf::FloatRect bounds = label.getLocalBounds();
        label.setOrigin(bounds.position + bounds.size / 2.f);
        label.setPosition(t.rect.getCenter());

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
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

    auto addInputBox = [&](int row, std::string placeholder, std::string inputtext, InputLabel i, sf::Color color) {
        inputboxList.emplace_back(
            sf::FloatRect({x, PANEL_SPACE + row * TEXTBOX_SPACE}, {TEXTBOX_WIDTH, TEXTBOX_HEIGHT}), placeholder, inputtext, i, color);
    };

    addInputBox(16, "Seed: ", "0", InputLabel::Seed, sf::Color(180, 208, 242));
    addInputBox(17, "Wall Density: ", "0", InputLabel::WallDensity, sf::Color(180, 208, 242));
    addInputBox(18, "Mud Blob Density: ", "0", InputLabel::MudBlobDensity, sf::Color(180, 208, 242));
    addInputBox(19, "Water Blob Density: ", "0", InputLabel::WaterBlobDensity, sf::Color(180, 208, 242));

    if (!font.openFromFile("assets/OpenSans-Regular.ttf")) {
        throw std::runtime_error("Failed to load both fonts!");
    }
}

void UI::drawButtons(sf::RenderWindow& window, const AppState& appState) {
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
}

void UI::drawTextBoxes(sf::RenderWindow& window, const AppState& appState) {
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


void UI::drawInputBoxes(sf::RenderWindow& window, const AppState& appState) {
for (const InputBox& i : inputboxList) {
        sf::RectangleShape box(i.rect.size);
        box.setPosition(i.rect.position);
        sf::Color color = i.color;
        std::string placeholder = i.placeholder;
        std::string inputText = i.inputtext;
        InputLabel inputLabel = i.label;
        box.setFillColor(color);


        switch(inputLabel) {
             case InputLabel::Seed:
                placeholder = "Seed: " + i.inputtext;
                break;
            case InputLabel::WallDensity:
                placeholder = "Wall Density: " + i.inputtext;
                break;
            case InputLabel::MudBlobDensity:
                placeholder = "Mud Blob Density: " + i.inputtext;
                break;
            case InputLabel::WaterBlobDensity:
                placeholder = "Water Blob Density: " + i.inputtext;
                break;
            case InputLabel::None:
                placeholder = "NONE" + i.inputtext;
                break;
            default:
                placeholder = "ERROR";
                break;
        }


        sf::Text label(font, placeholder, 15);
        label.setFillColor(sf::Color::Black);

        // Center the label inside b.rect
        sf::FloatRect bounds = label.getLocalBounds();
        label.setOrigin(bounds.position + bounds.size / 2.f);
        label.setPosition(i.rect.getCenter());

        window.draw(box);
        window.draw(label);
    }   
}


void UI::draw(sf::RenderWindow& window, const AppState& appState) {
    drawButtons(window, appState);
    drawTextBoxes(window, appState);
    drawInputBoxes(window, appState);
}

UiAction UI::handleClick(sf::Vector2i pos) {
    for (const Button& b : buttonList) {
        if (b.rect.contains(static_cast<sf::Vector2f>(pos))) {
            return b.action;
        }
    }
    return UiAction::None;
}


std::ostream& operator<<(std::ostream& os, InputLabel input) {
    switch (input) {
        case InputLabel::None:  os << "NONE"; break;
        case InputLabel::Seed:  os << "seed"; break;
        case InputLabel::WallDensity:   os << "Wall density"; break;
        case InputLabel::WaterBlobDensity: os << "Water Blob Density"; break;
        case InputLabel::MudBlobDensity: os << "Mud Blob Density"; break;
        default:              os << '?'; break;
    }
    return os;
}

InputLabel UI::handleInputBoxClick(sf::Vector2i pos) {
    for (const InputBox& i : inputboxList) {
        if (i.rect.contains(static_cast<sf::Vector2f>(pos))) {
            focusedInput = i.label;            
            std::cout << focusedInput << '\n';
            return focusedInput;
        }
    }
    focusedInput = InputLabel::None;
    return focusedInput;
}


void UI::handleTyping(char32_t c) {
    for (InputBox& i : inputboxList) {
        if (focusedInput == i.label) {
            std::cout << static_cast<char>(c);
            i.inputtext += static_cast<char>(c);
        }
    }
}

void UI::handleDelete() {
    for (InputBox& i : inputboxList) {
        if (focusedInput == i.label) {
            if (!i.inputtext.empty()) {
                i.inputtext.pop_back();
            }
        }
    }
}


void UI::handleEnter() {
    for (InputBox& i : inputboxList) {
        if (focusedInput == i.label) {
            focusedInput = InputLabel::None;
        }
    }
}
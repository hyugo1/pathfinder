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
#include <string>
#include <limits>
#include <stdexcept>
#include <cmath>

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

    addInputBox(16, "Seed: ", "", InputLabel::Seed, sf::Color(180, 208, 242));
    addInputBox(17, "Wall Density: ", "", InputLabel::WallDensity, sf::Color(180, 208, 242));
    addInputBox(18, "Mud Blob Density: ", "", InputLabel::MudBlobDensity, sf::Color(180, 208, 242));
    addInputBox(19, "Water Blob Density: ", "", InputLabel::WaterBlobDensity, sf::Color(180, 208, 242));

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

std::ostream& operator<<(std::ostream& os, const GenerationSettings& settings) {
    os << "Seed: " << settings.seed.value_or(0) << '\n'
       << "Wall density: " << settings.wallDensity.value_or(0.0) << '\n'
       << "Water Count: " << settings.waterCount.value_or(0.0) << '\n'
       << "Mud count: " << settings.mudCount.value_or(0.0);
    return os;
}

void UI::handleEnter(GenerationSettings& generationState) {
    for (InputBox& i : inputboxList) {
        if (focusedInput == i.label) {
            if (validateInput(focusedInput, i.inputtext)) {
                switch(focusedInput) {
                    case InputLabel::Seed:
                        generationState.seed = static_cast<unsigned int>(std::stoul(i.inputtext));
                        break;

                    case InputLabel::WallDensity:
                        generationState.wallDensity = std::stod(i.inputtext);
                        break;

                    case InputLabel::MudBlobDensity:
                        generationState.mudCount = std::stod(i.inputtext);
                        break;

                    case InputLabel::WaterBlobDensity:
                        generationState.waterCount = std::stod(i.inputtext);
                        break;
                    
                    case InputLabel::None:
                        // std::cout << "its none\n";
                        break;
                    default:
                        break;
                        
                }
            } else {
                std::cout << "Please correct your input.\n";
            }
        }
    }
}

bool UI::validateInput(InputLabel inputlabel, const std::string& inputstring) {
    switch (inputlabel) {
        case (InputLabel::WallDensity): {
            try {
                std::size_t pos;
                double input = std::stod(inputstring, &pos);
                if (pos != inputstring.size() || input < 0.0 || input > 1.0 || !std::isfinite(input)) {
                    throw std::invalid_argument("Invalid density");
                }
                std::cout << "Using " << InputLabel::WallDensity << ": " << input << '\n';
                return true;

            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid input: " << e.what() << ". Please enter a number between 0.0 and 1.0.\n";
                return false;
                
            } catch (const std::out_of_range& e) {
                std::cerr << "Density value is out of range.\n";
                return false;
            }
        }

        case (InputLabel::Seed): {
            try {
                if (inputstring.find_first_not_of("0123456789") != std::string::npos) {
                    throw std::invalid_argument("Seed must contain digits only");
                }

                std::size_t pos = 0;
                unsigned long input = std::stoul(inputstring, &pos);

                if (pos != inputstring.size()) {
                    throw std::invalid_argument("Invalid seed");
                }

                if (input > std::numeric_limits<unsigned int>::max()) {
                    throw std::out_of_range("Seed exceeds maximum value");
                }

                std::cout << "Using " << InputLabel::Seed << ": "
                        << input << '\n';
                return true;

            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid input: " << e.what() << ". Please enter a number between 0 and 4294967295.\n";
                return false;
                
            } catch (const std::out_of_range& e) {
                std::cerr << "Seed value is out of range.\n";
                return false;
            }
        }

        case InputLabel::WaterBlobDensity:
        case InputLabel::MudBlobDensity: 
            try {
                std::size_t pos;
                if (inputstring.find('-') != std::string::npos) {
                    throw std::invalid_argument("Negative numbers not allowed");
                }
                
                double input = std::stod(inputstring, &pos);
                if (pos != inputstring.size() || input < 0.0 || input > 100.0 || !std::isfinite(input)) {
                    throw std::invalid_argument("Invalid density");
                }
                std::cout << "Using " << InputLabel::WaterBlobDensity << ": " << input << '\n';
                std::cout << "Using " << InputLabel::MudBlobDensity << ": " << input << '\n';
                return true;

            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid input: " << e.what() << ". Please enter a number between 0 and 100.\n";
                return false;
                
            } catch (const std::out_of_range& e) {
                std::cerr << e.what() << "Density value is out of range.\n";
                return false;
            }
        case InputLabel::None:
            std::cout << "None selected";
            return false;
    }

    return true;
}
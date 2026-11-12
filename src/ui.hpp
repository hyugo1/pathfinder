#pragma once
 
#include <SFML/Graphics.hpp>
#include <optional>
#include <string>
#include <vector>
#include "algorithms.hpp"
#include "appstate.hpp"

constexpr int PANEL_WIDTH = 200;

constexpr int PANEL_SPACE = 10;

constexpr float BUTTON_WIDTH = 180.0f;
constexpr float BUTTON_HEIGHT = 40.0f;
constexpr float BUTTON_SPACE = 50.0f;

constexpr float TEXTBOX_WIDTH = 180.0f;
constexpr float TEXTBOX_HEIGHT = 40.0f;
constexpr float TEXTBOX_SPACE = 50.0f;

enum class UiAction {
    None,
    Generate,
    Clear,
    SelectDFS,
    SelectBFS,
    SelectAStar,
    SelectDijkstra,
    ToggleDelay,
    ToggleSeedLock,
};

enum class StatType {
    AlgorithmName,
    TotalNodesExplored,
    TotalCost,
    PathLength
};

class UI {
    private: 
        struct Button {
            sf::FloatRect rect;
            std::string text;
            UiAction action;
            sf::Color color;

            Button (sf::FloatRect r, std::string t, UiAction a, sf::Color c) : rect(r), text(t), action(a), color(c) {}
        };

        std::vector<Button> buttonList;

        //labels need an sf::Font and sf::Text
        sf::Font font;

        struct TextBox {
            sf::FloatRect rect;
            std::string text;
            StatType statType;
            sf::Color color;

            TextBox (sf::FloatRect r, std::string t, StatType s, sf::Color c) : rect(r), text(t), statType(s), color(c) {}
        };

        std::vector<TextBox> textboxList;

    public:
        UI();
        void draw(sf::RenderWindow& window, const AppState& appState);
        UiAction handleClick(sf::Vector2i pos);
};
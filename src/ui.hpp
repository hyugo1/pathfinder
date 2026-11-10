#pragma once
 
#include <SFML/Graphics.hpp>
#include <optional>
#include <string>
#include <vector>
#include "algorithms.hpp"
#include "appstate.hpp"

constexpr int PANEL_WIDTH = 140;
constexpr int PANEL_HEIGHT = 100;

constexpr int PANEL_SPACE = 10;

constexpr double BUTTON_WIDTH = 120.0f;
constexpr double BUTTON_HEIGHT = 40.0f;
constexpr double BUTTON_SPACE = 50.0f;

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

    public:
        UI();
        void draw(sf::RenderWindow& window);
        UiAction handleClick(sf::Vector2i pos);
};
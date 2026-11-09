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

enum class UiAction {
    None,
    Generate,
    Clear,
    SelectDFS,
    SelectBFS,
    SelectAStar,
    SelectDijkstra,
    ToggleDelay,
    ToggleSeedLock
};

class UI {
    private: 
        struct Button {
            sf::FloatRect rect;
            std::string text;
            UiAction action;

            Button (sf::FloatRect r, std::string t, UiAction a) : rect(r), text(t), action(a) {}
        };

        std::vector<Button> buttonList;

        std::vector<sf::Color> buttonColors = {
            sf::Color(200, 200, 200), // None
            sf::Color(240, 250, 130), // Clear
            sf::Color(235, 91,  91),  // Generate
            sf::Color(86,  240, 114), // DFS
            sf::Color(86,  240, 114), // BFS
            sf::Color(86,  240, 114), // A*
            sf::Color(86,  240, 114), // Dijkstra
            sf::Color(240, 86,  230), // Toggle Delay
            sf::Color(240, 86,  230)  // Toggle Seed Lock
        };

        //labels need an sf::Font and sf::Text
        sf::Font font;

    public:
        UI();
        void draw(sf::RenderWindow& window);
        UiAction handleClick(sf::Vector2i pos);
};
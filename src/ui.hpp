#pragma once
 
#include <SFML/Graphics.hpp>
#include <optional>
#include <string>
#include <vector>
#include "algorithms.hpp"
#include "appstate.hpp"

constexpr int PANEL_WIDTH = 200;
constexpr int PANEL_HEIGHT = 200;

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
            std::string label;
            UiAction action;

            Button (sf::FloatRect r, std::string l, UiAction a) : rect(r), label(l), action(a) {}
        };

        std::vector<Button> buttonList;

    public:
        void draw(sf::RenderWindow& window) {
            sf::RectangleShape box({100.f, 40.f});
            box.setPosition({COLS * CELL_SIZE + 10.f, 10.f});
            box.setFillColor(sf::Color(200, 200, 200));
            window.draw(box);
        }

};
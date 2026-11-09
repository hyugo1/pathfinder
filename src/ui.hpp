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
        UI();
        void draw(sf::RenderWindow& window);
        UiAction handleClick(sf::Vector2i pos);
};
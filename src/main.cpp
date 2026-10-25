//main.cpp
#include <iostream>

#include "console.hpp"
#include "terminal.hpp"
#include "renderer.hpp"
#include "appstate.hpp"
#include "grid.hpp"

void runApp(AppState& appState) {
    int choice = askForModeChoice();
    if (choice == 1) {
        runTerminalMode(appState);
    } else {
        bool threadedMode = chooseThreadedMode();
        sf::RenderWindow window(
            sf::VideoMode({COLS * CELL_SIZE, ROWS * CELL_SIZE}),
            "Pathfinding Visualizer"
        );
        runGuiMode(appState, window, threadedMode);
    }
}

int main() {

    AppState appState;
    appState.grid = Grid(ROWS, std::vector<CellType>(COLS, CellType::Open));

    runApp(appState);
    return 0;
}
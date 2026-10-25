#include "appstate.hpp"
#include "searchworker.hpp"
#include "algorithms.hpp"
#include "grid.hpp"
#include "console.hpp"
#include "renderer.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>


void handleKeyboardEvent(AppState& appState, AnimationState& animationState, const std::optional<sf::Event>& event, sf::RenderWindow& window) {
    if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
        if ((keyPressed->code == sf::Keyboard::Key::Num1 ||
                keyPressed->code == sf::Keyboard::Key::Num2 ||
                keyPressed->code == sf::Keyboard::Key::Num3) &&
            (appState.start.first == -1 || appState.end.first == -1)) {

            std::cout << "Set start and end first!\n";
            return;
        }

        switch (keyPressed->code) {
            case sf::Keyboard::Key::Escape:
                window.close();
                break;
            case sf::Keyboard::Key::Num1:
                std::cout << "Using " << algorithmName(Algorithm::DFS) << ".\n";
                appState.chosenAlgo = Algorithm::DFS;
                appState.gridChanged = true;
                appState.statsRequested = true;
                break;
            case sf::Keyboard::Key::Num2:
                std::cout << "Using " << algorithmName(Algorithm::BFS) << ".\n";
                appState.chosenAlgo = Algorithm::BFS;
                appState.gridChanged = true;
                appState.statsRequested = true;
                break;
            case sf::Keyboard::Key::Num3:
                std::cout << "Using " << algorithmName(Algorithm::AStar) << ".\n";
                appState.chosenAlgo = Algorithm::AStar;
                appState.gridChanged = true;
                appState.statsRequested = true;
                break;
            case sf::Keyboard::Key::C:
                std::cout << "Clearing the entire grid.\n";
                appState.gridChanged = true;
                appState.chosenAlgo = Algorithm::None;
                appState.start = {-1, -1};
                appState.end = {-1, -1};
                
                appState.statsRequested = false;

                resetGrid(appState.grid);
                resetSearchState(appState.result, animationState);
                ++appState.latestVersion;  
                break;
            case sf::Keyboard::Key::D:
                if (THREAD_SLEEP_SECONDS == 2) {
                    std::cout << "Removing delay.\n";
                    THREAD_SLEEP_SECONDS = 0;
                } else {
                    std::cout << "Adding delay of 2 second to make threading more visible.\n";
                    THREAD_SLEEP_SECONDS = 2;
                }
                break;
            case sf::Keyboard::Key::LShift:
                break;
            default:
                std::cout << "Unknown key pressed.\n";
                break;
        }
    }
}

void handleMouseEvent(AppState& appState, AnimationState& animationState, const std::optional<sf::Event>& event) {
    if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>()) {
        int col = mousePressed->position.x / CELL_SIZE;
        int row = mousePressed->position.y / CELL_SIZE;

        if (row >= 0 && row < ROWS && col >= 0 && col < COLS) {
            if (mousePressed->button == sf::Mouse::Button::Left) {
                updateCellType(appState, animationState, row, col, CellType::Wall);
            }
            else if (mousePressed->button == sf::Mouse::Button::Right &&
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
                updateCellType(appState, animationState, row, col, CellType::Start);
            }
            else if (mousePressed->button == sf::Mouse::Button::Right) {
                updateCellType(appState, animationState, row, col, CellType::End);
            }
            else { 
                std::cout << "Unknown mouse button pressed.\n";
            }
        }
    }
}

void processWorkerResult(AppState& appState, AnimationState& animationState, SearchWorker& worker) {
    if (const std::optional<Response> response = worker.poll()) {
        if (response->version == appState.latestVersion) {
            appState.result = response->result;
            displayPerformanceStats(appState.statsRequested, appState.chosenAlgo, appState.result, response->executionTimeMs);
            resetAnimationStates(animationState);
        }
    }
}

void processGridChanges(AppState& appState, AnimationState& animationState, bool threadedMode, SearchWorker& worker) {
    if (appState.gridChanged) {
        if (threadedMode) {
            if (appState.start.first != -1 && appState.end.first != -1) {
               worker.submit(Request{appState.grid, appState.start, appState.end, appState.chosenAlgo, ++appState.latestVersion});
            }
        } else {
            if (appState.start.first != -1 && appState.end.first != -1) {
                std::this_thread::sleep_for(std::chrono::seconds(THREAD_SLEEP_SECONDS));
                double executionTime = measureExecutionTime([&]() {
                    appState.result = runSearch(appState.grid, appState.start, appState.end, appState.chosenAlgo);
                });
            
                displayPerformanceStats(appState.statsRequested, appState.chosenAlgo, appState.result, executionTime);
                resetAnimationStates(animationState);
            }
        }
        appState.gridChanged = false;
    }
}

void processGUIFrame(sf::RenderWindow& window, AppState& appState, AnimationState& animationState, bool threadedMode, SearchWorker& worker) {
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }

        handleKeyboardEvent(appState, animationState, event, window);
        handleMouseEvent(appState, animationState, event);
    }

    processGridChanges(appState, animationState, threadedMode, worker);
    
    if (threadedMode) {
        processWorkerResult(appState, animationState, worker);
    }

    updateAnimation(animationState, appState);
}

void renderGUI(sf::RenderWindow& window, const AppState& appState, const AnimationState& animationState) {
    for (int row {0}; row < ROWS; ++row) {
        for (int col {0}; col < COLS; ++col) {

            sf::RectangleShape cell({CELL_SIZE, CELL_SIZE});
            cell.setPosition({col * CELL_SIZE * 1.f, row * CELL_SIZE * 1.f});

            setCellColor(cell, appState.grid[row][col], animationState, row, col);

            cell.setOutlineThickness(1);
            cell.setOutlineColor(sf::Color(200, 200, 200));

            window.draw(cell);
        }
    }
}

void runGuiMode(AppState& appState, sf::RenderWindow& window, bool threadedMode) {
    
    AnimationState animationState;
    SearchWorker worker;

    while (window.isOpen()) {
        // --- EVENT ---
        processGUIFrame(window, appState, animationState, threadedMode, worker);

        // --- RENDER ---
        window.clear(sf::Color::White);

        renderGUI(window, appState, animationState);

        window.display();
    }
}


void setCellColor(sf::RectangleShape& cell, CellType cellType, const AnimationState& animationState, int row, int col) {
    switch (cellType) {
        case CellType::Open:  cell.setFillColor(sf::Color::White); break;
        case CellType::Wall:  cell.setFillColor(sf::Color::Black); break;
        case CellType::Path:  cell.setFillColor(sf::Color::Yellow); break;
        case CellType::Explored:  cell.setFillColor(sf::Color::Cyan); break;
        case CellType::Start: cell.setFillColor(sf::Color::Green); break;
        case CellType::End:   cell.setFillColor(sf::Color::Blue); break;
    }

    if (animationState.animatedGrid[row][col] && cellType != CellType::Start && cellType != CellType::End) {
        cell.setFillColor(sf::Color::Cyan);
    }
    
    if (animationState.animatedPath[row * COLS + col] && cellType != CellType::Start && cellType != CellType::End) {
        cell.setFillColor(sf::Color::Red);
    }
}
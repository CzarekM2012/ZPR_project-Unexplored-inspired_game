/*!
 \mainpage Project description

 \section features Features:
 - local PvP arena
 - top-down view
 - static camera, limited interface
 - simple, polygon-based character and object models (and matching hitboxes)
 - melee weapons and shields
 - items provide players with custom actions like lunge or throw
 - requires controllers

 @file main.cpp Main file that creates window, objects and threads
 @file GameController.cpp
 @file InputHandler.cpp
 @file PhysicalObject.cpp
 @file Renderer.cpp
*/
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

#include "GameController.h"
#include "InputHandler.h"
#include "Renderer.h"

using namespace std::chrono_literals;
const int ACTION_Q_SIZE = 10;

int main() {
    std::cout << "Started" << std::endl;

    sf::RenderWindow window(sf::VideoMode(960, 540), "ZPR Game");
    Renderer renderer(&window);

    std::shared_ptr<moodycamel::ReaderWriterQueue<Action> > action_q(new moodycamel::ReaderWriterQueue<Action>(ACTION_Q_SIZE));
    InputHandler inputHandler(action_q);
    GameController gameController(action_q);

    gameController.prepareGame();
    std::thread gameLogicThread(&GameController::run, &gameController);

    while (window.isOpen()) {
        sf::Event event;

        if (window.pollEvent(event)) {
            inputHandler.handleEvent(event);
            if (event.type == sf::Event::Closed) {  // Event doesn't tell by itself if something had happened, need to check pollEvent return
                window.close();
            }
        }
        inputHandler.handleStates();
        renderer.render(gameController.getDrawablesCopy());
    }
    std::cout << "Window closed" << std::endl;

    GameController::stop = true;
    gameLogicThread.join();

    std::cout << "Finished" << std::endl;
    return 0;
}
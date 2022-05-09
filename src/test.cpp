#include <chrono>
#include <iostream>
#include <thread>
#include <vector>


#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

#include "../header/GameController.h"
#include "../header/InputHandler.h"
#include "../header/Renderer.h"


using namespace std::chrono_literals;
const float ACTION_Q_SIZE = 10;

int main() {
    std::cout << "Started" << std::endl;

    // sf::RenderWindow window(sf::VideoMode(1920, 1080), "ZPR Game");
    sf::RenderWindow window(sf::VideoMode(960, 540), "ZPR Game");
    Renderer renderer(&window);

    std::shared_ptr<moodycamel::ReaderWriterQueue<Action> > action_q(
        new moodycamel::ReaderWriterQueue<Action>(ACTION_Q_SIZE));
    InputHandler inputHandler(action_q);
    GameController gameController(action_q);

    std::thread gameLogicThread(
        &GameController::run,
        &gameController);  // ~Use a GameController function on gameController
                           // object
    while (window.isOpen()) {
        sf::Event event;

        if (window.pollEvent(event) &&
            event.type == sf::Event::Closed) {  // Event doesn't by itself tell
                                                // if something has happened (if
                                                // nothing happens can return 0)
            window.close();
        }

        inputHandler.handleInput(
            event);  // This should be called even if no new event has appeared
                     // (for e.g. checking if keyboard keys are still pressed)
        renderer.render(gameController.getStateCopy());
        std::this_thread::sleep_for(10ms);
    }
    std::cout << "Window closed" << std::endl;

    GameController::stop = true;
    gameLogicThread.join();

    std::cout << "Finished" << std::endl;
    return 0;
}
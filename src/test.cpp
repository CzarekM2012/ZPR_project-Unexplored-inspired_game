#include <iostream>
#include <vector>
#include <thread>

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

#include "../header/InputHandler.h"
#include "../header/GameController.h"

//TODO: Split Renderer, InputHandler and GameController into separate classes

sf::CircleShape shape(100.f);

// This is set by InputHandler in every pass, and then checked by GameController each tick
// contrary to Actions, that are sent into message queue only once, when they appear

InputHandler inputHandler;
GameController gameController;

// void run() {

//     while(!stop) {
//         sf::Vector2f * playerPos = &(state[0]); //TODO: Check pointer syntax

//         playerPos->x += inputHandler.inputStateTab[0][0] * STEP;
//         playerPos->y += inputHandler.inputStateTab[0][1] * STEP;

//         using namespace std::chrono_literals;
//         std::this_thread::sleep_for(1ms);
//         //std::cout << state[0].x << " " << state[0].y << " " << inputStateTab[0][0] << " " << inputStateTab[0][1] << std::endl;
//     }
// }

void render(sf::RenderWindow &window) {

    State state = gameController.getStateCopy();

    if(state.getObjectCount() == 0)
        return;

    shape.setPosition(sf::Vector2f(state.get(0)->x, state.get(0)->y));

    window.clear();
    window.draw(shape);
    window.display();
}

int main()
{
    std::cout << "Started" << std::endl;
    // Renderer setup
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "ZPR Game");
    shape.setFillColor(sf::Color::Green);

    std::thread gameLogicThread(&GameController::run, gameController); // ~Use a GameController function on gameController object
    while (window.isOpen())
    {
        sf::Event event;
        window.pollEvent(event);
        if (event.type == sf::Event::Closed)
            window.close();

        inputHandler.handleInput(event);
        render(window);
    }
    std::cout << "Window closed" << std::endl;
    
    //gameController.signalStop();
    GameController::stop = true;
    gameLogicThread.join();

    std::cout << "Finished" << std::endl;
    return 0;
}
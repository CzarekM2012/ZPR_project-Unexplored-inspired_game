#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

#include <box2d/box2d.h>
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>


#include "../header/InputHandler.h"
#include "../header/GameController.h"
#include "../header/Renderer.h"

using namespace std::chrono_literals;

InputHandler inputHandler;
GameController gameController;
Renderer renderer;

int main()
{
    std::cout << "Started" << std::endl;

    //sf::RenderWindow window(sf::VideoMode(1920, 1080), "ZPR Game");
    sf::RenderWindow window(sf::VideoMode(960, 540), "ZPR Game");
    Renderer renderer(&window);    

    std::thread gameLogicThread(&GameController::run, &gameController); // ~Use a GameController function on gameController object
    while (window.isOpen()) {
        sf::Event event;
        window.pollEvent(event);
        if (event.type == sf::Event::Closed)
            window.close();

        inputHandler.handleInput(event);
        renderer.render(gameController.getStateCopy());
        std::this_thread::sleep_for(10ms);
    }
    std::cout << "Window closed" << std::endl;
    
    GameController::stop = true;
    gameLogicThread.join();

    std::cout << "Finished" << std::endl;
    return 0;
}
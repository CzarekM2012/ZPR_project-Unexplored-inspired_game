#include <catch2/catch.hpp>
#include <memory>
#include "GameController.h"
#include "InputHandler.h"
#include "Renderer.h"

#include <iostream>
#include <thread>

using namespace std::chrono_literals;

unsigned int Factorial(unsigned int number) {
    return number <= 1 ? number : Factorial(number - 1) * number;
}

TEST_CASE("Factorials are computed", "[factorial]") {
    REQUIRE(Factorial(1) == 1);
    REQUIRE(Factorial(2) == 2);
    REQUIRE(Factorial(3) == 6);
    REQUIRE(Factorial(10) == 3628800);
}

TEST_CASE("GameController and InputHandler creation/destruction", "[game]") {
    std::shared_ptr<moodycamel::ReaderWriterQueue<Action> > action_q(new moodycamel::ReaderWriterQueue<Action>(10));
    InputHandler inputHandler(action_q);
    GameController gameController(action_q);
}

TEST_CASE("GameController thread launch", "[game]") {
    std::shared_ptr<moodycamel::ReaderWriterQueue<Action> > action_q(new moodycamel::ReaderWriterQueue<Action>(10));
    InputHandler inputHandler(action_q);
    GameController gameController(action_q);
    gameController.prepareGame();
    std::thread gameLogicThread(&GameController::run, &gameController);
    std::this_thread::sleep_for(1000ms);
    GameController::stop = true;
    gameLogicThread.join();
}

TEST_CASE("SFML Window and Renderer creation", "[game]") {
    sf::RenderWindow window(sf::VideoMode(960, 540), "ZPR Game");
    Renderer renderer(&window);
    window.close();
}
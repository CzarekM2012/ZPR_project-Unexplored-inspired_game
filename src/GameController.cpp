#include <thread>
#include <chrono>
#include <iostream>
#include <memory>

#include <box2d/box2d.h>

#include "../header/GameController.h"
#include "../header/InputHandler.h"

using namespace std::chrono_literals;

bool GameController::stop;

GameController::GameController() {

    world = new b2World(b2Vec2(0, 0));

    b2PolygonShape * shape = new b2PolygonShape();

    b2Vec2 pentagon[] = {b2Vec2(-5, -5), b2Vec2(-5, 5), b2Vec2(0, 7), b2Vec2(5, 5), b2Vec2(5, -5)}; 
    shape->Set(pentagon, 5);
    state.add(std::shared_ptr<PhysicalObject>(new PhysicalObject(world, 10, 10, true, shape, sf::Color::Green)));

    shape->SetAsBox(5, 5);
    state.add(std::shared_ptr<PhysicalObject>(new PhysicalObject(world, 80, 38, true, shape, sf::Color::Yellow)));
    state.add(std::shared_ptr<PhysicalObject>(new PhysicalObject(world, 74, 50, true, shape, sf::Color::Yellow)));
    state.add(std::shared_ptr<PhysicalObject>(new PhysicalObject(world, 82, 63, true, shape, sf::Color::Yellow)));

    // Walls
    shape->SetAsBox(100, 2);
    state.add(std::shared_ptr<PhysicalObject>(new PhysicalObject(world, 100, 0, false, shape, sf::Color(20, 20, 20))));
    state.add(std::shared_ptr<PhysicalObject>(new PhysicalObject(world, 100, 107.5, false, shape, sf::Color(20, 20, 20))));
    shape->SetAsBox(2, 60);
    state.add(std::shared_ptr<PhysicalObject>(new PhysicalObject(world, 0, 50, false, shape, sf::Color(20, 20, 20))));
    state.add(std::shared_ptr<PhysicalObject>(new PhysicalObject(world, 191.5, 50, false, shape, sf::Color(20, 20, 20))));
    shape->SetAsBox(2, 20);
    state.add(std::shared_ptr<PhysicalObject>(new PhysicalObject(world, 95, 50, false, shape, sf::Color(20, 20, 20))));

    stop = false;
}

void GameController::run() {
    while(!stop) {

        if(state.getObjectCount() == 0) {
            std::this_thread::sleep_for(1ms);
            std::cout << "State is Empty!" << std::endl;
            continue;
        }

        std::shared_ptr<PhysicalObject> player = state.get(0);
        player->getBodyPtr()->ApplyLinearImpulseToCenter(b2Vec2(InputHandler::inputStateTab[0][0] * STEP, InputHandler::inputStateTab[0][1] * STEP), true);
        world->Step(1, 8, 3);
        std::this_thread::sleep_for(1ms); //TODO: Add time control
    }
}
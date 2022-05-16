#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include <box2d/box2d.h>

#include "GameController.h"
#include "ObjectClasses.h"

using namespace std::chrono_literals;

bool GameController::stop;

#define UNUSED(x) (void)(x)  ///< For now to disable "unused parameter" error

GameController::GameController(std::shared_ptr<moodycamel::ReaderWriterQueue<Action> > q)
    : action_q(q) {
    world = new b2World(b2Vec2(0, 0));

    state.add(std::shared_ptr<PhysicalObject>(new Player()))
        ->createPhysicalObject(world, 10, 10);

    state.add(std::shared_ptr<PhysicalObject>(new Box()))
        ->createPhysicalObject(world, 80, 38);
    state.add(std::shared_ptr<PhysicalObject>(new Box()))
        ->createPhysicalObject(world, 74, 50);
    state.add(std::shared_ptr<PhysicalObject>(new Box()))
        ->createPhysicalObject(world, 82, 63);

    // Add some walls
    std::shared_ptr<Wall> wall;
    wall = std::shared_ptr<Wall>(new Wall());
    wall->setSize(200, 4);
    state.add(wall);
    wall->createPhysicalObject(world, 100, 0);

    wall = std::shared_ptr<Wall>(new Wall());
    wall->setSize(200, 4);
    state.add(wall);
    wall->createPhysicalObject(world, 100, 107.5);

    wall = std::shared_ptr<Wall>(new Wall());
    wall->setSize(200, 4);
    state.add(wall);
    wall->createPhysicalObject(world, 0, 50, 90);

    wall = std::shared_ptr<Wall>(new Wall());
    wall->setSize(200, 4);
    state.add(wall);
    wall->createPhysicalObject(world, 191.5, 50, 90);

    wall = std::shared_ptr<Wall>(new Wall());
    wall->setSize(40, 4);
    state.add(wall);
    wall->createPhysicalObject(world, 95, 50, 90);

    // Test some stuff
    state.add(std::shared_ptr<PhysicalObject>(new Sword()))
        ->createPhysicalObject(world, 10, 30);
    state.getLast()->setCollision(false);
    state.add(std::shared_ptr<PhysicalObject>(new Shield()))
        ->createPhysicalObject(world, 16, 30);
    state.getLast()->setCollision(false);
    // std::dynamic_pointer_cast<Box>(state.getLast())->setSize(20, 10);

    // b2PolygonShape * shape = new b2PolygonShape();

    // b2Vec2 pentagon[] = {b2Vec2(-5, -5), b2Vec2(-5, 5), b2Vec2(0, 7),
    // b2Vec2(5, 5), b2Vec2(5, -5)}; shape->Set(pentagon, 5);
    // state.add(std::shared_ptr<PhysicalObject>(new PhysicalObject(world, 10,
    // 10, true, shape, sf::Color::Green)));

    // shape->SetAsBox(5, 5);
    // state.add(std::shared_ptr<PhysicalObject>(new PhysicalObject(world, 80,
    // 38, true, shape, sf::Color::Yellow)));
    // state.add(std::shared_ptr<PhysicalObject>(new PhysicalObject(world, 74,
    // 50, true, shape, sf::Color::Yellow)));
    // state.add(std::shared_ptr<PhysicalObject>(new PhysicalObject(world, 82,
    // 63, true, shape, sf::Color::Yellow)));

    // // Walls
    // shape->SetAsBox(100, 2);
    // state.add(std::shared_ptr<PhysicalObject>(new PhysicalObject(world, 100,
    // 0, false, shape, sf::Color(20, 20, 20))));
    // state.add(std::shared_ptr<PhysicalObject>(new PhysicalObject(world, 100,
    // 107.5, false, shape, sf::Color(20, 20, 20)))); shape->SetAsBox(2, 60);
    // state.add(std::shared_ptr<PhysicalObject>(new PhysicalObject(world, 0,
    // 50, false, shape, sf::Color(20, 20, 20))));
    // state.add(std::shared_ptr<PhysicalObject>(new PhysicalObject(world,
    // 191.5, 50, false, shape, sf::Color(20, 20, 20)))); shape->SetAsBox(2,
    // 20); state.add(std::shared_ptr<PhysicalObject>(new PhysicalObject(world,
    // 95, 50, false, shape, sf::Color(20, 20, 20))));

    stop = false;
}

void GameController::run() {
    while (!stop) {
        if (state.getObjectCount() == 0) {
            std::this_thread::sleep_for(1ms);
            std::cout << "State is Empty!" << std::endl;
            continue;
        }

        // (player movement direction and look angles)
        for (int i = 0; i < 1;
             ++i)  // TODO: Change '1' to ~'playerCount' when supported
            processPlayerInputStates(i);

        Action action;
        if (action_q->try_dequeue(action)) {
            processAction(action);
        }

        // Process Physics
        world->Step(1, 8, 3);

        // Wait for the next tick
        std::this_thread::sleep_for(1ms);  // TODO: Add time control
    }
}

void GameController::processPlayerInputStates(int playerId) {
    UNUSED(playerId);

    std::shared_ptr<PhysicalObject> player = state.get(0);
    auto bodyPtr = player->getBodyPtr();
    bodyPtr->ApplyLinearImpulseToCenter(
        b2Vec2(InputHandler::inputStateTab[0][InputHandler::INPUT_MOVE_X] *
                   FORCE_MOVE,
               InputHandler::inputStateTab[0][InputHandler::INPUT_MOVE_Y] *
                   FORCE_MOVE),
        true);

    float angleDeg = player->getAngleDeg();
    float lookAngle =
        InputHandler::inputStateTab[0][InputHandler::INPUT_LOOK_ANGLE];
    float diff = abs(angleDeg - lookAngle);

    int force = FORCE_LOOK;
    if (diff > 180.0f) {
        diff = 360.0f - diff;
        force = -force;
    }
    force *= diff / 180.0f;

    if (force >= 0)
        force += FORCE_LOOK_MIN;
    else
        force -= FORCE_LOOK_MIN;

    if (angleDeg > lookAngle)
        force = -force;
    if (diff > LOOK_ACC_DEG / 2)
        bodyPtr->ApplyAngularImpulse(force, true);
    // else
    //     std:: cout << "No rotation force needed" << std::endl;
    // std:: cout << "Angle: " << player->getAngleDeg() << " Requested: " <<
    // InputHandler::inputStateTab[0][InputHandler::INPUT_LOOK_ANGLE] << "
    // Force: " << force << std::endl;
}

void GameController::processAction(const Action& action) {
    std::shared_ptr<Player> player;
    if (action.getPlayerId() >= 0)
        player = std::dynamic_pointer_cast<Player>(state.get(action.getPlayerId()));

    auto playerBody = player->getBodyPtr();
    auto playerPos = playerBody->GetPosition();

    std::vector<std::shared_ptr<PhysicalObject> > foundObjects;
    switch (action.getType()) {
        case Action::TYPE_DEBUG:
            std::cout << "Received Debug Action!" << std::endl;
            break;

        case Action::TYPE_PICK_LEFT:
            std::cout << "Received Pickup Action!" << std::endl;

            // TODO: Replace loop with something like:
            // std::copy_if(state.objects.begin(), state.objects.end(), foundObjects.begin(),
            //    [&player](std::shared_ptr<PhysicalObject> object){return (object->getBodyPtr()->GetPosition() - player->getBodyPtr()->GetPosition()).Length() < 10 && std::dynamic_pointer_cast<Item>(object);}); //TODO: change '10' to sth like "PICKUP_RANGE"
            // player->equipLeftHand(std::dynamic_pointer_cast<Item>(*foundObjects.begin()));

            for (auto object_it : state.objects) {
                auto body = object_it->getBodyPtr();
                auto pos = body->GetPosition();

                if (PICKUP_RANGE * PICKUP_RANGE < (pos.x - playerPos.x) * (pos.x - playerPos.x) + (pos.y - playerPos.y) * (pos.y - playerPos.y))
                    continue;

                if (auto item = std::dynamic_pointer_cast<Item>(object_it)) {
                    std::cout << "Found " << typeid(*object_it).name() << " at (" << pos.x << " " << pos.y << ")" << std::endl;
                    player->equipLeftHand(item);
                    break;
                }
            }
            break;

        default:
            break;
    }
}
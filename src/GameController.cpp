#include <algorithm>
#include <iostream>
#include <memory>
#include <thread>

#include <box2d/box2d.h>

#include "GameController.h"

using namespace std::chrono_literals;

/// A simple listener to capture Box2D collisions. Calls both colliding objects' onContact() function
class MyContactListener : public b2ContactListener {
    void BeginContact(b2Contact* contact) {
        auto data = contact->GetFixtureA()->GetUserData();
        PhysicalObject *objectA = nullptr, *objectB = nullptr;
        objectA = reinterpret_cast<PhysicalObject*>(data.pointer);

        data = contact->GetFixtureB()->GetUserData();
        objectB = reinterpret_cast<PhysicalObject*>(data.pointer);
        if (objectA && objectB) {
            objectA->onContact(objectB);
            objectB->onContact(objectA);
        }
    }

    void EndContact(b2Contact* /*contact*/) {}
};

volatile bool GameController::stop;
MyContactListener listener;

GameController::GameController(std::shared_ptr<moodycamel::ReaderWriterQueue<Action> > q)
    : action_q(q) {}

void GameController::prepareGame() {
    const std::lock_guard<std::mutex> guard(drawableCopyMutex);

    world = new b2World(b2Vec2(0, 0));
    world->SetContactListener(&listener);

    std::array<std::tuple<int, sf::Color, b2Vec2>, InputHandler::PLAYER_COUNT_MAX> players_parameters = {
        std::make_tuple(1, sf::Color::Red, b2Vec2(10, 10)),
        std::make_tuple(2, sf::Color::Green, b2Vec2(170, 10)),
        std::make_tuple(3, sf::Color::Blue, b2Vec2(10, 90)),
        std::make_tuple(4, sf::Color::Magenta, b2Vec2(170, 90))};
    std::transform(players_parameters.begin(), players_parameters.end(), players.begin(), [&](const auto& params) {
        auto player = new Player();
        player->setPrimaryColor(std::get<1>(params));
        state.add(player);
        player->createBody(world, std::get<2>(params));
        player->setCollisionGroup(std::get<0>(params));
        return player;
    });

    std::array<std::tuple<b2Vec2>, 3> boxes_parameters = {
        std::make_tuple(b2Vec2(80, 38)),
        std::make_tuple(b2Vec2(74, 50)),
        std::make_tuple(b2Vec2(82, 63))};
    std::for_each(boxes_parameters.begin(), boxes_parameters.end(), [&](auto& params) {
        auto box = new Box();
        state.add(box);
        box->createBody(world, std::get<0>(params));
    });

    // Add some walls
    std::array<std::tuple<float, float, b2Vec2, Angle>, 5> walls_parameters = {
        std::make_tuple(200, 4, b2Vec2(100, 0), Angle()),
        std::make_tuple(200, 4, b2Vec2(100, 107.5), Angle()),
        std::make_tuple(200, 4, b2Vec2(1, 50), Angle(90)),
        std::make_tuple(200, 4, b2Vec2(191.5, 50), Angle(90)),
        std::make_tuple(40, 4, b2Vec2(95, 50), Angle(90))};
    std::for_each(walls_parameters.begin(), walls_parameters.end(), [&](auto& params) {
        auto wall = new Wall();
        state.add(wall);
        wall->setSize(std::get<0>(params), std::get<1>(params));
        wall->createBody(world, std::get<2>(params), std::get<3>(params));
    });

    // Test some stuff
    std::array<std::tuple<PhysicalObject*, b2Vec2>, 9> equimpents_parameters = {
        std::make_tuple(new Sword(), b2Vec2(10, 10)),
        std::make_tuple(new Sword(), b2Vec2(170, 10)),
        std::make_tuple(new Sword(), b2Vec2(10, 90)),
        std::make_tuple(new Sword(), b2Vec2(170, 90)),

        std::make_tuple(new Shield(), b2Vec2(10, 10)),
        std::make_tuple(new Shield(), b2Vec2(170, 10)),
        std::make_tuple(new Shield(), b2Vec2(10, 90)),
        std::make_tuple(new Shield(), b2Vec2(170, 90)),

        std::make_tuple(new Axe(), b2Vec2(10, 10))};
    std::for_each(equimpents_parameters.begin(), equimpents_parameters.end(), [&](auto& params) {
        auto object = std::get<0>(params);
        state.add(object);
        object->createBody(world, std::get<1>(params));
        object->setCollision(false);
    });

    players[0]->equip(dynamic_cast<Item*>(std::get<0>(equimpents_parameters[0])), Player::EqSlot::LEFT_HAND);
    players[0]->equip(dynamic_cast<Item*>(std::get<0>(equimpents_parameters[4])), Player::EqSlot::RIGHT_HAND);

    players[1]->equip(dynamic_cast<Item*>(std::get<0>(equimpents_parameters[1])), Player::EqSlot::LEFT_HAND);
    players[1]->equip(dynamic_cast<Item*>(std::get<0>(equimpents_parameters[5])), Player::EqSlot::RIGHT_HAND);

    players[2]->equip(dynamic_cast<Item*>(std::get<0>(equimpents_parameters[2])), Player::EqSlot::LEFT_HAND);
    players[2]->equip(dynamic_cast<Item*>(std::get<0>(equimpents_parameters[6])), Player::EqSlot::RIGHT_HAND);

    players[3]->equip(dynamic_cast<Item*>(std::get<0>(equimpents_parameters[3])), Player::EqSlot::LEFT_HAND);
    players[3]->equip(dynamic_cast<Item*>(std::get<0>(equimpents_parameters[7])), Player::EqSlot::RIGHT_HAND);

    stop = false;
}

void GameController::run() {
    auto lastTime = std::chrono::steady_clock::now();
    // int fps = 0;
    // auto lastSecond = std::chrono::steady_clock::now();

    while (!stop) {
        if (state.getObjectCount() == 0) {
            std::this_thread::sleep_for(1ms);
            std::cout << "State is Empty!" << std::endl;
            continue;
        }

        // (player movement direction and look angles)
        for (unsigned int i = 0; i < players.size(); ++i)
            processPlayerInputStates(i);

        Action action;
        if (action_q->try_dequeue(action)) {
            processAction(action);
        }

        // Process Physics
        world->Step(1, 8, 3);

        drawableCopyMutex.lock();
        std::vector<PhysicalObject*> toRemove;
        for (auto&& object : state.objects) {
            if (object->toDestroy) {
                std::cout << "Adding object to remove list" << std::endl;
                toRemove.push_back(object.get());
            }
        }

        for (auto object : toRemove) {
            std::cout << "Destroying object" << std::endl;
            object->destroyBody();
            std::cout << "Removing object from Physical object list" << std::endl;
            state.remove(object);

            int alive = 0;
            for (int i = 0; i < InputHandler::PLAYER_COUNT_MAX; ++i) {
                if (players[i] == object)
                    players[i] = nullptr;

                if (players[i])
                    ++alive;
            }

            if (alive <= 1) {
                drawableCopyMutex.unlock();
                restartGame();
                drawableCopyMutex.lock();
            }
        }

        for (auto&& object : state.objects) {
            object->synchronize();
        }

        // std::this_thread::sleep_for(10ms); // For synchronization testing

        drawableCopyMutex.unlock();

        // Wait for the next tick
        // std::cout << "Duration = " << TIME_STEP.count() << ", Time to wait = " << (TIME_STEP - std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastTime)).count() << " Time spent: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastTime).count() << std::endl;
        // std::this_thread::sleep_for(TIME_STEP - std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastTime));
        while (TIME_STEP > std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - lastTime))  // TODO: Replace this with some form of non-active waiting
            ;
        lastTime = std::chrono::steady_clock::now();
        // fps += 1;

        // if (std::chrono::duration_cast<std::chrono::seconds>(lastTime - lastSecond).count() >= 1) {
        //     lastSecond = std::chrono::steady_clock::now();
        //     std::cout << "FPS: " << fps << std::endl;
        //     fps = 0;
        // }
    }
}

void GameController::processPlayerInputStates(const int playerId) {
    auto player = players.at(playerId);
    if (!player)
        return;

    auto playerInput = InputHandler::inputStateTab.at(playerId);
    auto bodyPtr = player->getBodyPtr();

    auto movementVector = std::get<InputHandler::INPUT_MOVEMENT>(playerInput);
    movementVector *= FORCE_MOVE;  // b2vec2 has no * operator
    bodyPtr->ApplyLinearImpulseToCenter(movementVector, true);

    Angle currentAngle = player->getAngle();
    Angle targetAngle = std::get<InputHandler::INPUT_LOOK_ANGLE>(playerInput);
    float diff = (targetAngle - currentAngle).get();

    auto force = FORCE_LOOK * diff / Angle::HALF_ANGLE_DEG;
    if (force >= 0)
        force += FORCE_LOOK_MIN;
    else
        force -= FORCE_LOOK_MIN;

    if (abs(diff) > LOOK_ACC_DEG)
        bodyPtr->ApplyAngularImpulse(force, true);

    player->moveItems();
    player->tickItemTimers();

    // else
    //     std:: cout << "No rotation force needed" << std::endl;
    // std:: cout << "Angle: " << player->getAngleDeg() << " Requested: " <<
    // InputHandler::inputStateTab[0][InputHandler::INPUT_LOOK_ANGLE] << "
    // Force: " << force << std::endl;
}

void GameController::processAction(const Action& action) {
    Player* player = nullptr;
    if (action.playerId >= 0 && action.playerId < InputHandler::PLAYER_COUNT_MAX)
        player = players[action.playerId];

    // Only debug actions allowed without a valid player
    if (!player && action.type != Action::Type::DEBUG && action.type != Action::Type::RESTART)
        return;

    std::cout << "Action from player " << action.playerId << std::endl;

    Item* foundItem = nullptr;
    switch (action.type) {
        case Action::Type::DEBUG:
            std::cout << "Received DEBUG Action!" << std::endl;
            drawableCopyMutex.lock();
            for (int i = 0; i < 4; ++i) {
                auto box = new Box();
                state.add(box);
                box->createBody(world, b2Vec2(80, 10));
                box->damage(99);
            }
            drawableCopyMutex.unlock();
            break;

        case Action::Type::RESTART:
            std::cout << "Received RESTART Action!" << std::endl;
            restartGame();
            break;

        case Action::Type::PICK_LEFT:
            std::cout << "Received PICKUP_LEFT Action!" << std::endl;

            foundItem = getFirstPickableItem(player);
            if (foundItem != nullptr)
                player->equip(foundItem, Player::EqSlot::LEFT_HAND);
            break;

        case Action::Type::DROP_LEFT:
            std::cout << "Received DROP_LEFT Action!" << std::endl;
            player->drop(Player::EqSlot::LEFT_HAND);
            break;

        case Action::Type::PICK_RIGHT:
            std::cout << "Received PICKUP_RIGHT Action!" << std::endl;

            foundItem = getFirstPickableItem(player);
            if (foundItem != nullptr)
                player->equip(foundItem, Player::EqSlot::RIGHT_HAND);
            break;

        case Action::Type::DROP_RIGHT:
            std::cout << "Received DROP_RIGHT Action!" << std::endl;
            player->drop(Player::EqSlot::RIGHT_HAND);
            break;

        case Action::Type::ACT_PREP_LEFT:
            std::cout << "Received ACT_PREP_LEFT Action!" << std::endl;
            player->prepareItem(Player::EqSlot::LEFT_HAND);
            break;

        case Action::Type::ACT_PREP_RIGHT:
            std::cout << "Received ACT_PREP_RIGHT Action!" << std::endl;
            player->prepareItem(Player::EqSlot::RIGHT_HAND);
            break;

        case Action::Type::ACT_LEFT:
            std::cout << "Received ACT_LEFT Action!" << std::endl;
            player->triggerAction(Player::EqSlot::LEFT_HAND);
            break;

        case Action::Type::ACT_RIGHT:
            std::cout << "Received ACT_RIGHT Action!" << std::endl;
            player->triggerAction(Player::EqSlot::RIGHT_HAND);
            break;

        default:
            break;
    }
}

void GameController::restartGame() {
    std::cout << "Restarting..." << std::endl;
    drawableCopyMutex.lock();
    state.objects.clear();
    drawableCopyMutex.unlock();
    players.fill(nullptr);
    delete world;
    prepareGame();
    std::cout << "Restarting finished" << std::endl;
}

Item* GameController::getFirstPickableItem(Player* player) const {
    // TODO: Replace loop with something like:
    // std::copy_if(state.objects.begin(), state.objects.end(), foundObjects.begin(),
    //    [&player](std::shared_ptr<PhysicalObject> object){return (object->getBodyPtr()->GetPosition() - player->getBodyPtr()->GetPosition()).Length() < 10 && std::dynamic_pointer_cast<Item>(object);});
    // player->equipLeftHand(std::dynamic_pointer_cast<Item>(*foundObjects.begin()));

    auto playerBody = player->getBodyPtr();
    auto playerPos = playerBody->GetPosition();

    for (auto&& object_it : state.objects) {
        auto body = object_it->getBodyPtr();
        auto pos = body->GetPosition();

        if (PICKUP_RANGE * PICKUP_RANGE < (pos.x - playerPos.x) * (pos.x - playerPos.x) + (pos.y - playerPos.y) * (pos.y - playerPos.y))
            continue;

        if (auto item = dynamic_cast<Item*>(object_it.get())) {
            if (item->getOwner() == nullptr) {
                std::cout << "Found " << typeid(*object_it).name() << " at (" << pos.x << " " << pos.y << ")" << std::endl;
                return item;
            }
        }
    }
    return nullptr;
}

std::vector<sf::ConvexShape> GameController::getDrawablesCopy() {
    std::vector<sf::ConvexShape> drawables;
    if (!drawableCopyMutex.try_lock()) {
        // std::cout << "Lock failed!" << std::endl;
        return drawables;
    }

    for (auto&& objectPtr : state.objects) {
        auto toAdd = objectPtr->getViews();
        drawables.insert(drawables.end(), toAdd.begin(), toAdd.end());
    }

    drawableCopyMutex.unlock();
    return drawables;
}
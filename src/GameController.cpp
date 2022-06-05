#include <math.h>
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

    // void EndContact(b2Contact* /*contact*/) {}
};

volatile bool GameController::stop;
MyContactListener listener;

GameController::GameController(std::shared_ptr<moodycamel::ReaderWriterQueue<Action> > q)
    : action_q(q) {}

GameController::~GameController() {
    if (world)
        delete world;
}

void GameController::prepareGame() {
    const std::lock_guard<std::mutex> guard(drawableCopyMutex);

    world = new b2World(b2Vec2(0, 0));
    world->SetContactListener(&listener);

    std::array<std::tuple<int, sf::Color, b2Vec2, Angle>, InputHandler::PLAYER_COUNT_MAX> players_parameters = {
        std::make_tuple(1, sf::Color::Red, b2Vec2(20, 15), Angle(-45)),
        std::make_tuple(2, sf::Color::Green, b2Vec2(170, 15), Angle(45)),
        std::make_tuple(3, sf::Color::Blue, b2Vec2(20, 85), Angle(-135)),
        std::make_tuple(4, sf::Color::Magenta, b2Vec2(170, 85), Angle(135))};
    std::transform(players_parameters.begin(), players_parameters.end(), players.begin(), [&](const auto& params) {
        auto player = new Player();
        player->setPrimaryColor(std::get<1>(params));
        state.add(player);
        player->createBody(world, std::get<2>(params), std::get<3>(params));
        player->setCollisionGroup(std::get<0>(params));

        // Starting equipment
        auto mainWeapon = new Sword();
        state.add(mainWeapon);
        mainWeapon->createBody(world, std::get<2>(params), std::get<3>(params));
        mainWeapon->setCollision(false);

        auto secondWeapon = new Shield();
        state.add(secondWeapon);
        secondWeapon->createBody(world, std::get<2>(params), std::get<3>(params));
        secondWeapon->setCollision(false);

        player->equip(mainWeapon, Player::EqSlotId::LEFT_HAND);
        player->equip(secondWeapon, Player::EqSlotId::RIGHT_HAND);
        return player;
    });

    // Add more equipment
    std::array<std::tuple<PhysicalObject*, b2Vec2>, 4> equimpents_parameters = {
        std::make_tuple(new Axe(), b2Vec2(86, 47)),
        std::make_tuple(new Axe(), b2Vec2(104, 47)),
        std::make_tuple(new Dagger(), b2Vec2(20, 45)),
        std::make_tuple(new Dagger(), b2Vec2(170, 45))};
    std::for_each(equimpents_parameters.begin(), equimpents_parameters.end(), [&](auto& params) {
        auto object = std::get<0>(params);
        state.add(object);
        object->createBody(world, std::get<1>(params));
        object->setCollision(false);
    });

    // Add boxes
    std::array<std::tuple<b2Vec2>, 8> boxes_parameters = {
        std::make_tuple(b2Vec2(80, 38)),
        std::make_tuple(b2Vec2(74, 50)),
        std::make_tuple(b2Vec2(82, 63)),
        std::make_tuple(b2Vec2(110, 38)),
        std::make_tuple(b2Vec2(116, 50)),
        std::make_tuple(b2Vec2(108, 63)),
        std::make_tuple(b2Vec2(20, 50)),
        std::make_tuple(b2Vec2(170, 50))};
    std::for_each(boxes_parameters.begin(), boxes_parameters.end(), [&](auto& params) {
        auto box = new Box();
        state.add(box);
        box->createBody(world, std::get<0>(params));
    });

    // Add walls
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

    stop = false;
}

void GameController::run() {
    auto lastTime = std::chrono::steady_clock::now();
    // int fps = 0;
    // auto lastSecond = std::chrono::steady_clock::now();

    while (!stop) {
        if (state.getObjectCount() == 0) {
            std::this_thread::sleep_for(1ms);

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
                toRemove.push_back(object.get());
            }
        }

        for (auto object : toRemove) {
            const auto playerIter = std::find(players.begin(), players.end(), object);
            if (playerIter != players.end()) {  // object was found in players
                auto& player = *playerIter;
                player->dropAll();
                player = nullptr;
            }

            const auto alive = std::count_if(players.begin(), players.end(), [](const Player* player) {
                return player != nullptr;
            });

            object->destroyBody();

            state.remove(object);

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

        // std::this_thread::sleep_for(TIME_STEP - std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastTime));
        while (TIME_STEP > std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - lastTime))  // Non-active waiting methods we tried weren't precise enough to avoid animation stuttering
            ;
        lastTime = std::chrono::steady_clock::now();
        // fps += 1;

        // if (std::chrono::duration_cast<std::chrono::seconds>(lastTime - lastSecond).count() >= 1) {
        //     lastSecond = std::chrono::steady_clock::now();

        //     fps = 0;
        // }
    }
}

void GameController::processPlayerInputStates(const int playerId) {
    const auto player = players.at(playerId);
    if (!player)
        return;

    const auto playerInput = InputHandler::inputStateTab.at(playerId);
    const auto bodyPtr = player->getBodyPtr();

    auto movementVector = std::get<InputHandler::INPUT_MOVEMENT>(playerInput);
    movementVector *= FORCE_MOVE;  // b2vec2 has no * operator
    bodyPtr->ApplyLinearImpulseToCenter(movementVector, true);

    const Angle currentAngle = player->getAngle();
    const Angle targetAngle = std::get<InputHandler::INPUT_LOOK_ANGLE>(playerInput);
    const float diff = (targetAngle - currentAngle).get();

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

    switch (action.type) {
        case Action::Type::DEBUG: {
            const std::lock_guard<std::mutex> drawCopyLock(drawableCopyMutex);
            for (int i = 0; i < 4; ++i) {
                auto box = new Box();
                state.add(box);
                box->createBody(world, b2Vec2(80, 10));
                box->damage(99);
            }
            break;
        }

        case Action::Type::RESTART:

            restartGame();
            break;

        case Action::Type::PICK_LEFT: {
            const auto foundItem = getFirstPickableItem(player);
            if (foundItem != nullptr)
                player->equip(foundItem, Player::EqSlotId::LEFT_HAND);
            break;
        }

        case Action::Type::DROP_LEFT:

            player->drop(Player::EqSlotId::LEFT_HAND);
            break;

        case Action::Type::PICK_RIGHT: {
            const auto foundItem = getFirstPickableItem(player);
            if (foundItem != nullptr)
                player->equip(foundItem, Player::EqSlotId::RIGHT_HAND);
            break;
        }

        case Action::Type::DROP_RIGHT:

            player->drop(Player::EqSlotId::RIGHT_HAND);
            break;

        case Action::Type::ACT_PREP_LEFT:

            player->prepareItem(Player::EqSlotId::LEFT_HAND);
            break;

        case Action::Type::ACT_PREP_RIGHT:

            player->prepareItem(Player::EqSlotId::RIGHT_HAND);
            break;

        case Action::Type::ACT_LEFT:

            player->triggerAction(Player::EqSlotId::LEFT_HAND);
            break;

        case Action::Type::ACT_RIGHT:

            player->triggerAction(Player::EqSlotId::RIGHT_HAND);
            break;

        case Action::Type::SWITCH_HANDS:

            player->switchHands();
            break;

        default:
            break;
    }
}

void GameController::restartGame() {
    std::cout << "Restarting..." << std::endl;
    {
        std::lock_guard<std::mutex> drawCopyLock(drawableCopyMutex);
        state.objects.clear();
    }
    players.fill(nullptr);
    delete world;
    prepareGame();
    std::cout << "Restarting finished" << std::endl;
}

Item* GameController::getFirstPickableItem(Player* player) const {
    std::vector<PhysicalObject*> itemPointers;
    std::transform(state.objects.begin(), state.objects.end(), std::back_inserter(itemPointers), [&](const auto& pointer) {
        return pointer.get();
    });  // Unpack std::unique_pointers

    std::vector<PhysicalObject*> ownerlessItems;
    std::copy_if(itemPointers.begin(), itemPointers.end(), std::back_inserter(ownerlessItems), [&](const auto& objectPointer) {
        auto itemPointer = dynamic_cast<Item*>(objectPointer);
        return (itemPointer && itemPointer->getOwner() == nullptr);
    });  // filter out non-Item pointers and Item pointers with owner

    auto playerBody = player->getBodyPtr();
    auto playerPos = playerBody->GetPosition();

    auto pointerIter = std::min_element(ownerlessItems.begin(), ownerlessItems.end(), [&](const auto& lower, const auto& higher) {
        auto lowerPos = lower->getBodyPtr()->GetPosition(), higherPos = higher->getBodyPtr()->GetPosition();
        return (playerPos - lowerPos).LengthSquared() < (playerPos - higherPos).LengthSquared();
    });  // find item with lowest distance from player

    if (pointerIter != ownerlessItems.end()) {
        auto itemPointer = (*pointerIter);
        auto itemPos = itemPointer->getBodyPtr()->GetPosition();
        if (pow(PICKUP_RANGE, 2) >= (itemPos - playerPos).LengthSquared())
            return dynamic_cast<Item*>(itemPointer);
    }
    return nullptr;
}

std::vector<sf::ConvexShape> GameController::getDrawablesCopy() {
    std::vector<sf::ConvexShape> drawables;
    if (!drawableCopyMutex.try_lock()) {
        return drawables;
    }

    for (auto&& objectPtr : state.objects) {
        auto toAdd = objectPtr->getViews();
        drawables.insert(drawables.end(), toAdd.begin(), toAdd.end());
    }

    drawableCopyMutex.unlock();
    return drawables;
}
#include <thread>
#include <chrono>

#include <iostream>

#include "../header/GameController.h"
#include "../header/InputHandler.h"

using namespace std::chrono_literals;

bool GameController::stop;

GameController::GameController() {
    state.add(new PhysicalObject(1, 1, 1));
    stop = false;
};

void GameController::run() {
    while(!stop) {

        if(state.getObjectCount() == 0) {
            std::this_thread::sleep_for(1ms);
            //std::cout << "State is Empty!" << std::endl;
            continue;
        }

        PhysicalObject * playerPos = state.get(0); //TODO: Check pointer syntax

        playerPos->x += InputHandler::inputStateTab[0][0] * STEP;
        playerPos->y += InputHandler::inputStateTab[0][1] * STEP;

        std::this_thread::sleep_for(1ms);
        //std::cout << state.get(0)->x << " " << state.get(0)->y << " " << InputHandler::inputStateTab[0][0] << " " << InputHandler::inputStateTab[0][1] << std::endl;
    }
}
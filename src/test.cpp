#include<iostream>
#include<vector>
#include <chrono>
#include<thread>

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

//TODO: Split Renderer, InputHandler and GameController into separate classes

sf::CircleShape shape(100.f);
bool stop = false;

const float INPUT_SCALE = 1;
const float STEP = 1;
const int PLAYER_COUNT_MAX = 4;
const int STATE_CONTROLS_PER_PLAYER = 3; // Position (x, y) and angle

// This is set by InputHandler in every pass, and then checked by GameController each tick
// contrary to Actions, that are sent into message queue only once, when they appear
float inputStateTab[PLAYER_COUNT_MAX][STATE_CONTROLS_PER_PLAYER];

std::vector<sf::Vector2f> state;

// TODO: Changing variables one-by-one can cause a race condition, check implications
void handleInput(sf::Event event) {
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        inputStateTab[0][1] = -INPUT_SCALE;
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        inputStateTab[0][1] = INPUT_SCALE;
    else
        inputStateTab[0][1] = 0;

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        inputStateTab[0][0] = -INPUT_SCALE;

    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        inputStateTab[0][0] = INPUT_SCALE;
    else
        inputStateTab[0][0] = 0;

    
    // switch(event.type) {
    //     case sf::Event::KeyPressed:
            
    //         break;
    // }
}

void run() {

    while(!stop) {
        sf::Vector2f * playerPos = &(state[0]); //TODO: Check pointer syntax

        playerPos->x += inputStateTab[0][0] * STEP;
        playerPos->y += inputStateTab[0][1] * STEP;

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1ms);
        //std::cout << state[0].x << " " << state[0].y << " " << inputStateTab[0][0] << " " << inputStateTab[0][1] << std::endl;
    }
}

void render(sf::RenderWindow &window) {

    if(state.empty())
        return;
    
    shape.setPosition(state[0]);

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

    // GameController setup
    state.push_back(shape.getPosition());

    std::thread gameLogicThread(run);
    while (window.isOpen())
    {
        sf::Event event;
        window.pollEvent(event);
        if (event.type == sf::Event::Closed)
            window.close();

        handleInput(event);
        render(window);
    }
    std::cout << "Window closed" << std::endl;
    
    stop = true;
    gameLogicThread.join();

    std::cout << "Finished" << std::endl;
    return 0;
}
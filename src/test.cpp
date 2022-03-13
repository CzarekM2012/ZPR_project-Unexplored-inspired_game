#include<iostream>

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

float STEP = 0.1;
sf::Vector2f position;

void handleInput(sf::Event event) {
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        position.y -= STEP;
    
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        position.x -= STEP;

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        position.y += STEP;

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        position.x += STEP;

    
    // switch(event.type) {
    //     case sf::Event::KeyPressed:
            
    //         break;
    // }
}

int main()
{
    std::cout << "Started" << std::endl;

    sf::RenderWindow window(sf::VideoMode(1920, 1080), "ZPR Game");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    position = shape.getPosition();

    while (window.isOpen())
    {
        //while (window.pollEvent(event))
        //{
        sf::Event event;
        window.pollEvent(event);
        if (event.type == sf::Event::Closed)
            window.close();
        //}

        handleInput(event);

        //position.x += 0.01;
        //position.y += 0.01;
        shape.setPosition(position);

        window.clear();
        window.draw(shape);
        window.display();
    }

    std::cout << "Finished" << std::endl;
    return 0;
}
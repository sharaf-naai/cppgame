#pragma once
#include <SFML/Graphics.hpp>

class Entity {
public:
    sf::RectangleShape shape;

    virtual ~Entity() = default;

    virtual void draw(sf::RenderWindow& window) const {
        window.draw(shape);
    }

    sf::FloatRect getBounds() const { return shape.getGlobalBounds(); }
    sf::Vector2f getPosition() const { return shape.getPosition(); }
    sf::Vector2f getSize() const { return shape.getSize(); }
};
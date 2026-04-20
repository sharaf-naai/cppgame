#pragma once
#include "Entity.h"

// --- BULLET ---
class Bullet : public Entity {
public:
    sf::Vector2f velocity;
    bool isShotgun;
    float originX;

    Bullet(float startX, float startY, sf::Vector2f vel, bool shotgun = false, sf::Color color = sf::Color::Yellow)
        : velocity(vel), isShotgun(shotgun), originX(startX) {
        shape.setSize({10.f, 10.f});
        shape.setFillColor(color);
        shape.setPosition({startX, startY});
    }
    void update(float dt) { shape.move(velocity * dt); }
};

// --- ENEMY ---
struct Enemy {
    sf::RectangleShape shape;
    float speed;
    bool isFlying;
    float velocityY;

    // Added the 'flying' boolean to the constructor
    Enemy(float x, float y, float speed, bool flying) : speed(speed), isFlying(flying), velocityY(0.f) {
        shape.setPosition({x, y});
        shape.setFillColor(sf::Color::White); // Remove the red tint so textures show perfectly
    }

    // Upgraded update function to accept platforms for collision!
    void update(float dt, const std::vector<sf::RectangleShape>& platforms) {
        shape.move({-speed * dt, 0.f}); // Always move left

        if (!isFlying) {
            // Apply Gravity for Ground Enemies (Skeleton & Spider)
            velocityY += 1500.f * dt;
            shape.move({0.f, velocityY * dt});

            // Ground Collision: Stop falling if touching a platform
            for (const auto& plat : platforms) {
                if (shape.getGlobalBounds().findIntersection(plat.getGlobalBounds())) {
                    // THE FIX: Increased tolerance to 30.f to be super safe against falling through
                    if (velocityY >= 0.f && shape.getPosition().y + shape.getSize().y - (velocityY * dt) <= plat.getPosition().y + 30.f) {
                        shape.setPosition({shape.getPosition().x, plat.getPosition().y - shape.getSize().y});
                        velocityY = 0.f;
                    }
                }
            }
        }
    }

    sf::FloatRect getBounds() const { return shape.getGlobalBounds(); }
    sf::Vector2f getPosition() const { return shape.getPosition(); }
    sf::Vector2f getSize() const { return shape.getSize(); }
    void draw(sf::RenderWindow& window) const { window.draw(shape); }
};

// --- COIN ---
struct Coin {
    sf::CircleShape shape;
    Coin(float x, float y) {
        shape.setRadius(10.f);
        shape.setFillColor(sf::Color::Yellow);
        shape.setPosition({x, y});
    }
};

// --- PICKUPS ---
struct Pickup {
    bool active = false;
};

struct GunPickup : public Pickup {
    sf::RectangleShape shape;
    GunPickup() {
        shape.setSize({25.f, 15.f});
        shape.setFillColor(sf::Color(139, 69, 19));
    }
};

struct ShotgunPickup : public Pickup {
    sf::RectangleShape shape;
    ShotgunPickup() {
        shape.setSize({35.f, 12.f});
        shape.setFillColor(sf::Color(100, 100, 100));
    }
};

struct HealthPotion : public Pickup {
    sf::RectangleShape shape;
    float cooldownTimer = 0.f;
    HealthPotion() {
        shape.setSize({15.f, 20.f});
        shape.setFillColor(sf::Color::Green);
    }
};

struct ShieldPickup : public Pickup {
    sf::RectangleShape shape;
    float cooldownTimer = 0.f;
    ShieldPickup() {
        shape.setSize({15.f, 20.f});
        shape.setFillColor(sf::Color::Green);
    }
};
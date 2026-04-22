#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "WorldObjects.h"

class Boss : public Entity {
private:
    int health;
    int maxHealth;
    float speed;
    float attackCooldown;
    float hitTimer;
    bool facingRight;

    const sf::Texture* idleTex;
    const sf::Texture* walkTex;
    const sf::Texture* attackTex;
    const sf::Texture* hitTex;
    const sf::Texture* deadTex;
    const sf::Texture* bulletTex;

    int currentFrame;
    float frameTimer;
    bool isAttacking;
    bool hasFired;

public:
    Boss(float x, float y);
    void setTextures(const sf::Texture& idle, const sf::Texture& walk, const sf::Texture& atk, const sf::Texture& hit, const sf::Texture& dead, const sf::Texture& bullet);
    void update(float dt, std::vector<Bullet>& bossBullets, float floorY, sf::Vector2f playerCenter);
    void takeDamage(int amount);

    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    sf::FloatRect getBounds() const;

    bool isDeadAnimFinished() const;
    void draw(sf::RenderWindow& window);
};

#pragma once
#include "Entity.h"
#include "WorldObjects.h"
#include <vector>
#include <cmath>

// Abstract Base Class
class Boss : public Entity {
protected:
    int health;
    int maxHealth;
    float speed;
    float shootTimer;
    float stateTimer;
    int state;
    int dirX;
    int dirY;
    int shotsFired;

public:
    Boss(int hp, float spd, float startX, float startY);
    virtual ~Boss() = default;

    // Pure Virtual Function forces derived classes to implement their own logic
    virtual void update(float dt, std::vector<Bullet>& bossBullets, float floorY, sf::Vector2f playerCenter) = 0;

    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    void takeDamage(int amount) { health -= amount; }
};

// Level 1 Boss Implementation
class Level1Boss : public Boss {
public:
    Level1Boss(float startX, float startY);
    void update(float dt, std::vector<Bullet>& bossBullets, float floorY, sf::Vector2f playerCenter) override;
};

// Level 2 Cloud Boss Implementation
class CloudBoss : public Boss {
public:
    CloudBoss(float startX, float startY);
    void update(float dt, std::vector<Bullet>& bossBullets, float floorY, sf::Vector2f playerCenter) override;
};
#include "Boss.h"

// Base Boss Constructor
Boss::Boss(int hp, float spd, float startX, float startY)
    : health(hp), maxHealth(hp), speed(spd), shootTimer(0.f), stateTimer(0.f), state(0), dirX(1), dirY(1), shotsFired(0) {
    shape.setPosition({startX, startY});
}

// Level 1 Boss
Level1Boss::Level1Boss(float startX, float startY) : Boss(40, 200.f, startX, startY) {
    shape.setSize({80.f, 120.f});
    shape.setFillColor(sf::Color::Magenta);
}

void Level1Boss::update(float dt, std::vector<Bullet>& bossBullets, float floorY, sf::Vector2f playerCenter) {
    if (state == 1) { 
        stateTimer -= dt;
        if (stateTimer <= 0.f) {
            state = 0; shotsFired = 0;
            dirX = (shape.getPosition().x > 400.f) ? -1 : 1; 
            float newX = (dirX == -1) ? 20.f : 700.f;
            shape.setPosition({newX, shape.getPosition().y});
        }
        return;
    }

    shape.move({0.f, speed * dirY * dt});
    if (shape.getPosition().y <= 50.f) dirY = 1;
    else if (shape.getPosition().y + shape.getSize().y >= floorY) dirY = -1;

    shootTimer += dt;
    if (shootTimer >= 1.2f) {
        shootTimer = 0.f; shotsFired++;
        sf::Vector2f bossCenter = {shape.getPosition().x + shape.getSize().x/2.f, shape.getPosition().y + shape.getSize().y / 2.f};
        float angleToPlayer = std::atan2(playerCenter.y - bossCenter.y, playerCenter.x - bossCenter.x);
        float spreads[3] = {-0.1f, 0.0f, 0.1f};
        for (int i = 0; i < 3; ++i) {
            float angle = angleToPlayer + spreads[i];
            bossBullets.emplace_back(bossCenter.x, bossCenter.y, sf::Vector2f(std::cos(angle) * 400.f, std::sin(angle) * 400.f), false, sf::Color(255, 100, 0));
        }
        if (shotsFired >= 6) {
            state = 1; stateTimer = 4.0f;
        }
    }
}

// Level 2 Cloud Boss
CloudBoss::CloudBoss(float startX, float startY) : Boss(60, 350.f, startX, startY) {
    shape.setSize({120.f, 50.f});
    shape.setFillColor(sf::Color(200, 200, 200)); 
}

void CloudBoss::update(float dt, std::vector<Bullet>& bossBullets, float floorY, sf::Vector2f playerCenter) {
    stateTimer += dt;
    shootTimer += dt;

    if (state == 0) {
        shape.move({speed * dirX * dt, 0.f});
        if (shape.getPosition().x <= 0) dirX = 1;
        if (shape.getPosition().x + shape.getSize().x >= 800.f) dirX = -1;

        if (shootTimer >= 0.2f) { 
            shootTimer = 0.f;
            float dropX = shape.getPosition().x + (std::rand() % static_cast<int>(shape.getSize().x));
            bossBullets.emplace_back(dropX, shape.getPosition().y + shape.getSize().y, sf::Vector2f(0.f, 300.f), false, sf::Color::Cyan);
        }

        if (stateTimer >= 5.0f) { 
            state = 1; stateTimer = 0.f;
            shape.setFillColor(sf::Color(80, 80, 100)); 
        }
    } else {
        float dx = playerCenter.x - (shape.getPosition().x + shape.getSize().x / 2.f);
        if (dx > 10.f) shape.move({120.f * dt, 0.f}); 
        if (dx < -10.f) shape.move({-120.f * dt, 0.f});

        if (shootTimer >= 0.08f) { 
            shootTimer = 0.f;
            float dropX = shape.getPosition().x + (std::rand() % static_cast<int>(shape.getSize().x));
            bossBullets.emplace_back(dropX, shape.getPosition().y + shape.getSize().y, sf::Vector2f(0.f, 550.f), false, sf::Color::Blue);
        }

        if (stateTimer >= 3.5f) { 
            state = 0; stateTimer = 0.f;
            shape.setFillColor(sf::Color(200, 200, 200));
            dirX = (std::rand() % 2 == 0) ? 1 : -1; 
        }
    }
}
#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Entity.h"

class Bullet : public Entity {
public:
    sf::Vector2f velocity;
    bool isShotgun;
    float originX;

    int totalFrames = 1;
    int currentFrame = 0;
    float frameTimer = 0.f;

    Bullet(float startX, float startY, sf::Vector2f vel, bool shotgun = false, sf::Color color = sf::Color::White)
        : velocity(vel), isShotgun(shotgun) {
        shape.setPosition({startX, startY});
        shape.setFillColor(color);
    }

    void update(float dt) {
        shape.move({velocity.x * dt, velocity.y * dt});

        if (shape.getTexture() && totalFrames > 1) {
            frameTimer += dt;
            if (frameTimer >= 0.08f) {
                frameTimer -= 0.08f;
                currentFrame = (currentFrame + 1) % totalFrames;
                int fW = shape.getTexture()->getSize().x / totalFrames;
                int fH = shape.getTexture()->getSize().y;
                shape.setTextureRect(sf::IntRect({currentFrame * fW, 0}, {fW, fH}));
            }
        }
    }
};

struct Coin {
    sf::RectangleShape shape;
    float frameTimer = 0.f;
    int currentFrame = 0;
    int totalFrames = 12;
    float frameDuration = 0.08f;

    Coin(float x, float y) {
        shape.setPosition({x, y});
        shape.setSize({30.f, 30.f});
    }

    void update(float dt) {
        if (shape.getTexture() && totalFrames > 1) {
            frameTimer += dt;
            if (frameTimer >= frameDuration) {
                frameTimer -= frameDuration;
                currentFrame = (currentFrame + 1) % totalFrames;
                int fWidth = shape.getTexture()->getSize().x / totalFrames;
                int fHeight = shape.getTexture()->getSize().y;
                shape.setTextureRect(sf::IntRect({currentFrame * fWidth, 0}, {fWidth, fHeight}));
            }
        }
    }
};

struct GunPickup     { sf::RectangleShape shape; bool active = false; };
struct ShotgunPickup { sf::RectangleShape shape; bool active = false; };
struct HealthPotion  { sf::RectangleShape shape; bool active = false; float cooldownTimer = 0.f; };
struct ShieldPickup  { sf::RectangleShape shape; bool active = false; float cooldownTimer = 0.f; };

class Enemy {
public:
    sf::RectangleShape shape;
    float speed;
    bool isFlying;

    const sf::Texture* currentSheet = nullptr;
    const sf::Texture* runSheet = nullptr;
    const sf::Texture* dieSheet = nullptr;

    int runFrames = 1;
    int dieFrames = 1;
    int totalFrames = 1;
    int currentFrame = 0;
    float frameTimer = 0.f;
    float frameDuration = 0.08f;
    bool isDying = false;
    bool isDead = false;

    Enemy(float x, float y, float spd, bool flying) : speed(spd), isFlying(flying) { shape.setPosition({x, y}); }

    void setAnimation(const sf::Texture* run, int rFrames, const sf::Texture* die, int dFrames) {
        runSheet = run; runFrames = rFrames;
        dieSheet = die; dieFrames = dFrames;
        currentSheet = runSheet; totalFrames = runFrames;
        shape.setTexture(currentSheet);

        if (currentSheet) {
            int fWidth = currentSheet->getSize().x / totalFrames;
            int fHeight = currentSheet->getSize().y;
            shape.setTextureRect(sf::IntRect({0, 0}, {fWidth, fHeight}));
        }
    }

    void triggerDeath() {
        if (!isDying) {
            isDying = true; currentFrame = 0; frameTimer = 0.f;
            currentSheet = dieSheet; totalFrames = dieFrames;
            shape.setTexture(currentSheet);

            if (currentSheet) {
                int fWidth = currentSheet->getSize().x / totalFrames;
                int fHeight = currentSheet->getSize().y;
                shape.setTextureRect(sf::IntRect({0, 0}, {fWidth, fHeight}));
            }
        }
    }

    void update(float dt, const std::vector<sf::RectangleShape>& platforms) {
        if (!isDying) shape.move({-speed * dt, 0.f});

        if (currentSheet) {
            frameTimer += dt;
            if (frameTimer >= frameDuration) {
                frameTimer -= frameDuration;
                currentFrame++;
                if (currentFrame >= totalFrames) {
                    if (isDying) { isDead = true; currentFrame = totalFrames - 1; }
                    else currentFrame = 0;
                }

                int fWidth = currentSheet->getSize().x / totalFrames;
                int fHeight = currentSheet->getSize().y;
                shape.setTextureRect(sf::IntRect({currentFrame * fWidth, 0}, {fWidth, fHeight}));
            }
        }
    }

    sf::FloatRect getBounds() const {
        sf::FloatRect visual = shape.getGlobalBounds();
        float hitW = visual.size.x * 0.35f;
        float hitH = visual.size.y * 0.5f;
        float hitX = visual.position.x + (visual.size.x - hitW) / 2.f;
        float hitY = isFlying
            ? visual.position.y + (visual.size.y - hitH) / 2.f
            : visual.position.y + visual.size.y - hitH - (visual.size.y * 0.15f);
        return sf::FloatRect({hitX, hitY}, {hitW, hitH});
    }

    sf::Vector2f getPosition() const { return shape.getPosition(); }
    sf::Vector2f getSize() const { return shape.getSize(); }
    void draw(sf::RenderWindow& window) const { window.draw(shape); }
};

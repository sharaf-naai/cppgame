#include "Boss.h"
#include <cmath>

Boss::Boss(float x, float y) {
    shape.setSize({875.f, 875.f});
    shape.setOrigin({437.5f, 875.f});
    shape.setPosition({x, y});

    health = maxHealth = 60;
    speed = 80.f;
    attackCooldown = 2.0f;
    hitTimer = 0.f;
    facingRight = false;
    currentFrame = 0;
    frameTimer = 0.f;
    isAttacking = false;
    hasFired = false;
}

void Boss::setTextures(const sf::Texture& idle, const sf::Texture& walk, const sf::Texture& atk, const sf::Texture& hit, const sf::Texture& dead, const sf::Texture& bullet) {
    idleTex = &idle; walkTex = &walk; attackTex = &atk;
    hitTex = &hit; deadTex = &dead; bulletTex = &bullet;
    shape.setTexture(idleTex);
}

sf::FloatRect Boss::getBounds() const {
    sf::Vector2f pos = shape.getPosition();
    float hitW = 325.f;
    float hitH = 225.f;
    float actualFloorY = pos.y - 175.f;
    return sf::FloatRect({pos.x - hitW / 2.f, actualFloorY - hitH}, {hitW, hitH});
}

void Boss::takeDamage(int amount) {
    if (health > 0) {
        health -= amount;
        hitTimer = 0.3f;
        isAttacking = false;
    }
}

bool Boss::isDeadAnimFinished() const {
    return health <= 0 && currentFrame >= 2;
}

void Boss::update(float dt, std::vector<Bullet>& bossBullets, float floorY, sf::Vector2f playerCenter) {

    shape.setPosition({shape.getPosition().x, floorY + 175.f});

    const sf::Texture* targetSheet = idleTex;
    int targetFrames = 6;
    float animSpeed = 0.1f;

    if (health <= 0) {
        targetSheet = deadTex; targetFrames = 3; animSpeed = 0.15f;
    } else if (hitTimer > 0.f) {
        hitTimer -= dt;
        targetSheet = hitTex; targetFrames = 3; animSpeed = 0.1f;
    } else if (isAttacking) {
        targetSheet = attackTex; targetFrames = 5; animSpeed = 0.1f;

        if (currentFrame == 3 && !hasFired) {
            float dir = facingRight ? 1.f : -1.f;

            // Adjust the spawn position slightly lower to match the mouth
            sf::Vector2f bPos = shape.getPosition() + sf::Vector2f(dir * 150.f, -220.f);

            // --- NEW TARGETING MATH ---
            // Calculate the exact line from the boss's mouth to the player's chest!
            sf::Vector2f direction = playerCenter - bPos;
            float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

            sf::Vector2f bVel;
            if (distance != 0) {
                // Normalize the vector and multiply by speed
                bVel = (direction / distance) * 700.f;
            } else {
                bVel = sf::Vector2f(dir * 700.f, 0.f); // Fallback
            }
            // ---------------------------

            Bullet b(bPos.x, bPos.y, bVel, false, sf::Color::White);

            b.shape.setSize({120.f, 120.f});
            b.shape.setOrigin({60.f, 60.f});
            b.shape.setTexture(bulletTex);
            b.totalFrames = 4;
            b.originX = bPos.x;

            bossBullets.push_back(b);
            hasFired = true;
        }
    } else {
        attackCooldown -= dt;
        float dist = playerCenter.x - shape.getPosition().x;
        facingRight = dist > 0;

        if (std::abs(dist) > 350.f) {
            shape.move({(facingRight ? speed : -speed) * dt, 0.f});
            targetSheet = walkTex; targetFrames = 8; animSpeed = 0.08f;
        } else {
            targetSheet = idleTex; targetFrames = 6; animSpeed = 0.1f;
            if (attackCooldown <= 0.f) {
                isAttacking = true;
                hasFired = false;
                currentFrame = 0;
            }
        }
    }

    if (shape.getTexture() != targetSheet) {
        shape.setTexture(targetSheet);
        currentFrame = 0;
        frameTimer = 0.f;
    }

    if (targetSheet == deadTex && currentFrame == targetFrames - 1) {
        // Stay dead
    } else {
        frameTimer += dt;
        if (frameTimer >= animSpeed) {
            frameTimer -= animSpeed;
            currentFrame++;
            if (currentFrame >= targetFrames) {
                if (targetSheet == deadTex) currentFrame = targetFrames - 1;
                else {
                    currentFrame = 0;
                    if (isAttacking) {
                        isAttacking = false;
                        attackCooldown = 2.0f;
                    }
                }
            }
        }
    }

    if (targetSheet && targetFrames > 0) {
        int fW = targetSheet->getSize().x / targetFrames;
        int fH = targetSheet->getSize().y;
        shape.setTextureRect(sf::IntRect({currentFrame * fW, 0}, {fW, fH}));
    }
}

void Boss::draw(sf::RenderWindow& window) {
    if (facingRight) shape.setScale({-1.f, 1.f});
    else shape.setScale({1.f, 1.f});

    if (hitTimer > 0.f) shape.setFillColor(sf::Color(255, 100, 100));
    else shape.setFillColor(sf::Color::White);

    window.draw(shape);
}
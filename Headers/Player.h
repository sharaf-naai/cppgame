#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "WorldObjects.h"

class Player {
private:
    sf::RectangleShape shape;
    sf::Vector2f velocity;

    int health = 16;
    int maxHealth = 16;

    int coins = 0;
    int ammo = 0;
    int clipAmmo = 0;
    bool hasWeapon = false;
    bool hasShotgun = false;

    float shieldTimer = 0.f;
    float invincibilityTimer = 0.f;
    float shootCooldown = 0.f;
    float rechargingTimer = 0.f;
    bool facingRight = true;

    const sf::Texture* idleTex = nullptr;
    const sf::Texture* runTex = nullptr;
    const sf::Texture* jumpTex = nullptr;
    const sf::Texture* shotTex = nullptr;
    const sf::Texture* hurtTex = nullptr;
    const sf::Texture* deadTex = nullptr;
    const sf::Texture* rechargeTex = nullptr;

    int currentFrame = 0;
    float frameTimer = 0.f;
    bool onGround = false;
    float shootingAnimTimer = 0.f;

    void updateAnimation(float dt);

public:
    Player(float x, float y);

    void setAnimationTextures(const sf::Texture& idle, const sf::Texture& run, const sf::Texture& jump, const sf::Texture& shot, const sf::Texture& hurt, const sf::Texture& dead, const sf::Texture& recharge);

    void update(float dt, const std::vector<sf::RectangleShape>& platforms, std::vector<Bullet>& bullets);
    void draw(sf::RenderWindow& window);

    sf::FloatRect getBounds() const;
    sf::Vector2f getCenter() const;
    float getFloorY() const;

    int getHealth() const { return health; }
    int getCoins() const { return coins; }
    int getAmmo() const { return ammo; }
    int getClipAmmo() const { return clipAmmo; }
    bool getHasWeapon() const { return hasWeapon; }
    bool getHasShotgun() const { return hasShotgun; }
    bool isDeadAnimFinished() const;

    void heal(int amount);
    void takeDamage(int amount);
    void activateShield(float duration);
    void resetCoins() { coins = 0; }
    void addCoin() { coins++; }
    void pickUpGun();
    void pickUpShotgun();
    void triggerRecharge();
    void triggerBossMode(int level);
    void resetPosition(float x, float y);
};
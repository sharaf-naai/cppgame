#pragma once
#include "Entity.h"
#include "WorldObjects.h"
#include <vector>

class Player : public Entity {
private:
    sf::Texture texture; // MUST BE ABOVE SPRITE
    sf::Sprite sprite;   // MUST BE BELOW TEXTURE
    sf::Vector2f velocity;

    float speed, jumpForce, gravity, floorY;
    bool isGrounded, isInvincible, facingRight, hasGun, hasShotgun, isReloading, wasXPressed, isShielded;
    int health, ammo, clipAmmo, coins;
    float iFrameTimer, reloadTimer, shieldTimeLeft;

public:
    Player(float startX, float startY);

    void update(float dt, const std::vector<sf::RectangleShape>& platforms, std::vector<Bullet>& bullets);
    void draw(sf::RenderWindow& window) const override;

    void takeDamage(int amount);
    void heal(int amount);
    void forceKill();
    void addCoin();
    void resetCoins();
    void pickUpGun();
    void pickUpShotgun();
    void triggerBossMode(int level);
    void activateShield(float duration = 3.0f);
    void resetPosition(float x, float y);

    // Getters
    float getFloorY() const { return floorY; }
    sf::Vector2f getCenter() const;
    int getHealth() const { return health; }
    bool getHasWeapon() const { return hasGun || hasShotgun; }
    bool getHasShotgun() const { return hasShotgun; }
    int getAmmo() const { return ammo; }
    int getClipAmmo() const { return clipAmmo; }
    bool getIsReloading() const { return isReloading; }
    int getCoins() const { return coins; }
};
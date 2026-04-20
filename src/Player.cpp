#include "Player.h"
#include <iostream>
#include <algorithm>

Player::Player(float startX, float startY)
    : texture(), sprite(texture), velocity({0.f, 0.f}), speed(300.f),
      jumpForce(-900.f), // <--- Change this to -900.f
      gravity(1500.f), floorY(startY),
      isGrounded(false), isInvincible(false), facingRight(true), hasGun(false), hasShotgun(false), isReloading(false),
      wasXPressed(false), isShielded(false), health(10), ammo(0), clipAmmo(0), coins(0),
      iFrameTimer(0.f), reloadTimer(0.f), shieldTimeLeft(0.f)
{
    shape.setSize({60.f, 60.f});

    // 1. Load the image using the absolute path that we KNOW works!
    if (!texture.loadFromFile("C:/Users/chara/Desktop/cmake-sfml-project-master/cmake-sfml-project-master/src/player.png")) {
        std::cerr << "WARNING: Could not load player.png!\n";
        shape.setFillColor(sf::Color::Magenta);
        sprite.setColor(sf::Color::Transparent);
    } else {
        shape.setFillColor(sf::Color::Transparent);

        // FIX: The 'true' argument forces the sprite to update its rectangle to 31x31!
        sprite.setTexture(texture, true);
        sprite.setColor(sf::Color::White);

        float texWidth = static_cast<float>(texture.getSize().x);
        float texHeight = static_cast<float>(texture.getSize().y);

        if (texWidth > 0.f && texHeight > 0.f) {
            sprite.setScale({40.f / texWidth, 60.f / texHeight});
            sprite.setOrigin({0.f, 0.f});
        }
    }

    resetPosition(startX, startY);
}

void Player::update(float dt, const std::vector<sf::RectangleShape>& platforms, std::vector<Bullet>& bullets) {

    // Flashing logic for both the sprite AND the failsafe shape
    if (isInvincible && !isShielded) {
        iFrameTimer -= dt;
        if (iFrameTimer <= 0.f) {
            isInvincible = false;
            sprite.setColor(sf::Color::White);
            if (shape.getFillColor() != sf::Color::Transparent) shape.setFillColor(sf::Color::Magenta);
        } else {
            int cycle = static_cast<int>(iFrameTimer / 0.2f);
            if (cycle % 2 == 0) {
                sprite.setColor(sf::Color(255, 255, 255, 100)); // Faded
                if (shape.getFillColor() != sf::Color::Transparent) shape.setFillColor(sf::Color(255, 0, 255, 100));
            } else {
                sprite.setColor(sf::Color::White); // Solid
                if (shape.getFillColor() != sf::Color::Transparent) shape.setFillColor(sf::Color::Magenta);
            }
        }
    }

    if (isShielded) {
        shieldTimeLeft -= dt;
        if (shieldTimeLeft <= 0.f) isShielded = false;
    }

    velocity.x = 0.f;

    // Calculate base scale dynamically for flipping the sprite
    float baseScaleX = 1.f;
    float baseScaleY = 1.f;
    if (texture.getSize().x > 0 && texture.getSize().y > 0) {
        baseScaleX = 40.f / static_cast<float>(texture.getSize().x);
        baseScaleY = 60.f / static_cast<float>(texture.getSize().y);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
        velocity.x -= speed;
        facingRight = false;

        // FLIP SPRITE LEFT
        sprite.setScale({-baseScaleX, baseScaleY});
        sprite.setOrigin({static_cast<float>(texture.getSize().x), 0.f});
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
        velocity.x += speed;
        facingRight = true;

        // FACE SPRITE RIGHT
        sprite.setScale({baseScaleX, baseScaleY});
        sprite.setOrigin({0.f, 0.f});
    }

    if (isReloading) {
        reloadTimer -= dt;
        if (reloadTimer <= 0.f) {
            isReloading = false; clipAmmo = std::min((hasShotgun ? 3 : 5), ammo);
        }
    }

    bool isXPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::X);
    if (isXPressed && !wasXPressed && (hasGun || hasShotgun) && !isReloading && clipAmmo > 0) {
        clipAmmo--; ammo--;
        bool isUpPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up);
        sf::Vector2f bVel = isUpPressed ? sf::Vector2f(0.f, -800.f) : sf::Vector2f(facingRight ? 800.f : -800.f, 0.f);
        float spawnX = isUpPressed ? shape.getPosition().x + shape.getSize().x / 2.f - 5.f : (facingRight ? shape.getPosition().x + shape.getSize().x : shape.getPosition().x - 10.f);
        float spawnY = isUpPressed ? shape.getPosition().y - 10.f : shape.getPosition().y + 30.f;

        bullets.emplace_back(spawnX, spawnY, bVel, hasShotgun, hasShotgun ? sf::Color(255, 165, 0) : sf::Color::Yellow);

        if (clipAmmo == 0 && ammo > 0) { isReloading = true; reloadTimer = 1.5f; }
        else if (ammo <= 0) { hasGun = false; hasShotgun = false; }
    }
    wasXPressed = isXPressed;

    shape.move({velocity.x * dt, 0.f});

    // Around line 105 in Player.cpp:
    if (shape.getPosition().x < 0.f) shape.setPosition({0.f, shape.getPosition().y});
    else if (shape.getPosition().x + shape.getSize().x > 1280.f) shape.setPosition({1280.f - shape.getSize().x, shape.getPosition().y});

    for (const auto& platform : platforms) {
        if (shape.getGlobalBounds().findIntersection(platform.getGlobalBounds())) {
            if (velocity.x > 0.f) shape.setPosition({platform.getPosition().x - shape.getSize().x, shape.getPosition().y});
            else if (velocity.x < 0.f) shape.setPosition({platform.getPosition().x + platform.getSize().x, shape.getPosition().y});
            velocity.x = 0.f;
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && isGrounded) { velocity.y = jumpForce; isGrounded = false; }

    velocity.y += gravity * dt;
    shape.move({0.f, velocity.y * dt});
    isGrounded = false;

    for (const auto& platform : platforms) {
        if (shape.getGlobalBounds().findIntersection(platform.getGlobalBounds())) {
            if (velocity.y > 0.f) {
                shape.setPosition({shape.getPosition().x, platform.getPosition().y - shape.getSize().y});
                isGrounded = true; velocity.y = 0.f;
            } else if (velocity.y < 0.f) {
                shape.setPosition({shape.getPosition().x, platform.getPosition().y + platform.getSize().y});
                velocity.y = 0.f;
            }
        }
    }

    if (shape.getPosition().y + shape.getSize().y >= floorY) {
        shape.setPosition({shape.getPosition().x, floorY - shape.getSize().y});
        velocity.y = 0.f; isGrounded = true;
    }

    // SNAP THE SPRITE TO THE HITBOX
    sprite.setPosition(shape.getPosition());
}

void Player::draw(sf::RenderWindow& window) const {
    window.draw(shape);
    window.draw(sprite); // Draw the actual image!

    if (isShielded) {
        sf::CircleShape shieldAura(45.f);
        shieldAura.setFillColor(sf::Color(0, 255, 255, 80));
        shieldAura.setOutlineColor(sf::Color::Cyan);
        shieldAura.setOutlineThickness(2.f);
        shieldAura.setPosition({shape.getPosition().x + shape.getSize().x/2.f - 45.f,
                                shape.getPosition().y + shape.getSize().y/2.f - 45.f});
        window.draw(shieldAura);
    }
}

void Player::takeDamage(int amount) {
    if (isShielded) return;
    if (!isInvincible) {
        health -= amount;
        if (health < 0) health = 0;
        isInvincible = true; iFrameTimer = 1.2f;
    }
}

void Player::heal(int amount) { health = std::min(10, health + amount); }
void Player::forceKill() { health = 0; }
void Player::addCoin() { coins++; }
void Player::resetCoins() { coins = 0; }
void Player::pickUpGun() { hasGun = true; hasShotgun = false; ammo = 15; clipAmmo = std::min(5, ammo); isReloading = false; }
void Player::pickUpShotgun() { hasShotgun = true; hasGun = false; ammo = 15; clipAmmo = std::min(3, ammo); isReloading = false; }

void Player::triggerBossMode(int level) {
    if (level == 1) {
        hasGun = true; hasShotgun = false; ammo = 30; clipAmmo = std::min(5, ammo); isReloading = false;
    } else if (level == 2) {
        hasGun = false; hasShotgun = true; ammo = 30; clipAmmo = std::min(3, ammo); isReloading = false;
        activateShield(8.0f);
    }
}

void Player::activateShield(float duration) { isShielded = true; shieldTimeLeft = duration; }

void Player::resetPosition(float x, float y) {
    shape.setPosition({x, y - shape.getSize().y});
    velocity = {0.f, 0.f};
    health = 10; isInvincible = false; hasGun = false; hasShotgun = false;
    ammo = 0; clipAmmo = 0; isReloading = false; coins = 0; isShielded = false;
    facingRight = true;

    // Ensure sprite opacity is reset
    sprite.setColor(sf::Color::White);
}

sf::Vector2f Player::getCenter() const {
    return {shape.getPosition().x + shape.getSize().x / 2.f, shape.getPosition().y + shape.getSize().y / 2.f};
}
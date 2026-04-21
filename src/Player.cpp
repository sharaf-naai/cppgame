#include "Player.h"
#include <cmath>
#include <iostream>

Player::Player(float x, float y) {
    shape.setSize({150.f, 150.f});
    shape.setOrigin({75.f, 75.f});
    shape.setPosition({x, y});
}

void Player::setAnimationTextures(const sf::Texture& idle, const sf::Texture& run, const sf::Texture& jump, const sf::Texture& shot, const sf::Texture& hurt, const sf::Texture& dead, const sf::Texture& recharge) {
    idleTex = &idle; runTex = &run; jumpTex = &jump;
    shotTex = &shot; hurtTex = &hurt; deadTex = &dead; rechargeTex = &recharge;
    shape.setTexture(idleTex);
}

sf::FloatRect Player::getBounds() const {
    sf::Vector2f pos = shape.getPosition();
    float hitWidth = 40.f;
    float hitHeight = 70.f;

    float hitX = pos.x - (hitWidth / 2.f);
    float hitY = pos.y + 75.f - hitHeight - 5.f;

    return sf::FloatRect({hitX, hitY}, {hitWidth, hitHeight});
}

sf::Vector2f Player::getCenter() const {
    sf::FloatRect b = getBounds();
    return {b.position.x + b.size.x / 2.f, b.position.y + b.size.y / 2.f};
}

float Player::getFloorY() const { return getBounds().position.y + getBounds().size.y; }

bool Player::isDeadAnimFinished() const {
    if (!deadTex) return false;
    int totalFrames = deadTex->getSize().x / deadTex->getSize().y;
    if (totalFrames <= 0) totalFrames = 1;
    return health <= 0 && currentFrame == totalFrames - 1;
}

void Player::heal(int amount) { health = std::min(maxHealth, health + amount); }

void Player::takeDamage(int amount) {
    if (invincibilityTimer <= 0.f && shieldTimer <= 0.f && health > 0) {
        health -= amount;
        invincibilityTimer = 1.0f;
    }
}

void Player::activateShield(float duration) { shieldTimer = duration; }

void Player::triggerRecharge() {
    int maxClip = hasShotgun ? 3 : 6;

    if (hasWeapon && rechargeTex && clipAmmo < maxClip && ammo > 0) {
        int needed = maxClip - clipAmmo;
        int reloadAmount = std::min(needed, ammo);

        clipAmmo += reloadAmount;
        ammo -= reloadAmount;

        int totalFrames = rechargeTex->getSize().x / rechargeTex->getSize().y;
        rechargingTimer = totalFrames * 0.08f;
    }
}

void Player::pickUpGun() { hasWeapon = true; hasShotgun = false; ammo = 30; clipAmmo = 0; triggerRecharge(); }
void Player::pickUpShotgun() { hasWeapon = true; hasShotgun = true; ammo = 15; clipAmmo = 0; triggerRecharge(); }
void Player::triggerBossMode(int level) { coins = 0; }
void Player::resetPosition(float x, float y) {
    shape.setPosition({x, y}); velocity = {0.f, 0.f}; health = maxHealth; hasWeapon = false; currentFrame = 0;
}

void Player::update(float dt, const std::vector<sf::RectangleShape>& platforms, std::vector<Bullet>& bullets) {
    if (invincibilityTimer > 0.f) invincibilityTimer -= dt;
    if (shieldTimer > 0.f) shieldTimer -= dt;
    if (shootCooldown > 0.f) shootCooldown -= dt;
    if (shootingAnimTimer > 0.f) shootingAnimTimer -= dt;
    if (rechargingTimer > 0.f) rechargingTimer -= dt;

    if (hasWeapon && clipAmmo == 0 && ammo > 0 && rechargingTimer <= 0.f) {
        triggerRecharge();
    }

    velocity.y += 1400.f * dt;

    if (health <= 0) {
        velocity.x = 0.f;
        shape.move({0.f, velocity.y * dt});
        onGround = false;
        sf::FloatRect bounds = getBounds();
        for (const auto& plat : platforms) {
            sf::FloatRect pBounds = plat.getGlobalBounds();
            if (bounds.findIntersection(pBounds)) {
                if (velocity.y > 0) { shape.move({0.f, -(bounds.position.y + bounds.size.y - pBounds.position.y)}); velocity.y = 0.f; onGround = true; }
            }
        }
        updateAnimation(dt);
        return;
    }

    velocity.x = 0.f;
    if (rechargingTimer <= 0.f && shootingAnimTimer <= 0.f) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) { velocity.x = -350.f; facingRight = false; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) { velocity.x = 350.f; facingRight = true; }
        // 20% lower jump height as requested
        if (onGround && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) { velocity.y = -820.f; onGround = false; }
        if (onGround && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::C)) triggerRecharge();
    }

    shape.move({velocity.x * dt, 0.f});
    sf::FloatRect bounds = getBounds();
    for (const auto& plat : platforms) {
        sf::FloatRect pBounds = plat.getGlobalBounds();
        if (bounds.findIntersection(pBounds)) {
            if (velocity.x > 0) shape.move({-(bounds.position.x + bounds.size.x - pBounds.position.x), 0.f});
            else if (velocity.x < 0) shape.move({pBounds.position.x + pBounds.size.x - bounds.position.x, 0.f});
        }
    }

    shape.move({0.f, velocity.y * dt});
    onGround = false;
    bounds = getBounds();
    for (const auto& plat : platforms) {
        sf::FloatRect pBounds = plat.getGlobalBounds();
        if (bounds.findIntersection(pBounds)) {
            if (velocity.y > 0) {
                shape.move({0.f, -(bounds.position.y + bounds.size.y - pBounds.position.y)});
                velocity.y = 0.f;
                onGround = true;
            } else if (velocity.y < 0) {
                shape.move({0.f, pBounds.position.y + pBounds.size.y - bounds.position.y});
                velocity.y = 0.f;
            }
        }
    }

    sf::FloatRect screenBounds = getBounds();
    if (screenBounds.position.x < 0.f) shape.move({-screenBounds.position.x, 0.f});
    if (screenBounds.position.x + screenBounds.size.x > 1280.f) shape.move({1280.f - (screenBounds.position.x + screenBounds.size.x), 0.f});

    if (hasWeapon && clipAmmo > 0 && shootCooldown <= 0.f && rechargingTimer <= 0.f && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::X)) {
        float dir = facingRight ? 1.f : -1.f;
        sf::Vector2f spawnPos = getCenter();

        if (hasShotgun) {
            Bullet b1(spawnPos.x, spawnPos.y, sf::Vector2f(dir * 800.f, -100.f), true); b1.originX = spawnPos.x; bullets.push_back(b1);
            Bullet b2(spawnPos.x, spawnPos.y, sf::Vector2f(dir * 800.f, 0.f), true); b2.originX = spawnPos.x; bullets.push_back(b2);
            Bullet b3(spawnPos.x, spawnPos.y, sf::Vector2f(dir * 800.f, 100.f), true); b3.originX = spawnPos.x; bullets.push_back(b3);
            shootCooldown = 0.8f;
        } else {
            Bullet b(spawnPos.x, spawnPos.y, sf::Vector2f(dir * 1000.f, 0.f), false); b.originX = spawnPos.x; bullets.push_back(b);
            shootCooldown = 0.3f;
        }

        clipAmmo--;
        shootingAnimTimer = 0.2f;
    }

    updateAnimation(dt);
}

void Player::updateAnimation(float dt) {
    const sf::Texture* targetSheet = idleTex;
    float animSpeed = 0.08f;

    if (health <= 0) { targetSheet = deadTex; animSpeed = 0.1f; }
    else if (invincibilityTimer > 0.6f && hurtTex) { targetSheet = hurtTex; animSpeed = 0.1f; }
    else if (rechargingTimer > 0.f && rechargeTex) { targetSheet = rechargeTex; animSpeed = 0.08f; }
    else if (shootingAnimTimer > 0.f && shotTex) { targetSheet = shotTex; animSpeed = 0.05f; }
    else if (!onGround && jumpTex) { targetSheet = jumpTex; animSpeed = 0.08f; }
    else if (std::abs(velocity.x) > 0.1f && runTex) { targetSheet = runTex; animSpeed = 0.08f; }
    else { targetSheet = idleTex; animSpeed = 0.1f; }

    if (shape.getTexture() != targetSheet) {
        shape.setTexture(targetSheet);
        currentFrame = 0;
        frameTimer = 0.f;
    }

    int targetFrames = 1;
    int fH = 150;
    if (targetSheet && targetSheet->getSize().y > 0) {
        targetFrames = targetSheet->getSize().x / targetSheet->getSize().y;
        if (targetFrames <= 0) targetFrames = 1;
        fH = targetSheet->getSize().y;
    }

    if (targetSheet == deadTex && currentFrame == targetFrames - 1) {
        // Do nothing! Let the player lie dead
    } else {
        frameTimer += dt;
        if (frameTimer >= animSpeed) {
            frameTimer -= animSpeed;
            currentFrame++;
            if (currentFrame >= targetFrames) {
                if (targetSheet == deadTex) currentFrame = targetFrames - 1;
                else currentFrame = 0;
            }
        }
    }

    shape.setTextureRect(sf::IntRect({currentFrame * fH, 0}, {fH, fH}));
}

void Player::draw(sf::RenderWindow& window) {
    if (facingRight) shape.setScale({1.f, 1.f});
    else shape.setScale({-1.f, 1.f});

    if (invincibilityTimer > 0.f && static_cast<int>(invincibilityTimer * 10) % 2 == 0) shape.setFillColor(sf::Color(255, 100, 100));
    else shape.setFillColor(sf::Color::White);

    window.draw(shape);

    // --- NEW: CYAN SHIELD BUBBLE ---
    if (shieldTimer > 0.f) {
        sf::CircleShape shieldBubble(80.f);
        shieldBubble.setOrigin({80.f, 80.f}); // Center it exactly over the player
        shieldBubble.setPosition(getCenter());

        // Semi-transparent glowing inside with a solid bright border
        shieldBubble.setFillColor(sf::Color(0, 255, 255, 80));
        shieldBubble.setOutlineColor(sf::Color::Cyan);
        shieldBubble.setOutlineThickness(3.f);

        window.draw(shieldBubble);
    }
}
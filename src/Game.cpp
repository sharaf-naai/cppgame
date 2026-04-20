#include "Game.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath> // Added for atan2 math!

Game::Game()
    : window(sf::VideoMode({1280, 720}), "Forest Platformer - Level 1 Complete!"),
      backTexture(), farTexture(), middleTexture(), platformTexture(),
      coinTex(), shieldTex(), healthTex(), bowTex(), goldenBowTex(), arrowTex(),
      bossTex(), fireballTex(), // ADDED BOSS AND FIREBALL TEX
      ghostTex(), skeletonTex(), spiderTex(),
      playerHeartTex(), bossHeartTex(), ammoTex(),
      backSprite(backTexture), farSprite(farTexture), middleSprite(middleTexture),
      playerHeartSprite(playerHeartTex), bossHeartSprite(bossHeartTex), ammoIconSprite(ammoTex),
      player(50.f, 700.f), gameState(GameState::PLAYING), gunSpawned(false), coinSpawnTimer(0.f), shotgunSpawnTimer(0.f),
      currentScore(0.f), checkpointScore(0.f), currentLevel(1), checkpointLevel(1),
      scoreText(font), stateText(font), coinText(font), ammoText(font)
{
    window.setFramerateLimit(60);
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    std::string path = "C:/Users/chara/Desktop/cmake-sfml-project-master/cmake-sfml-project-master/src/";

    // BACKGROUND LAYERS
    if (!backTexture.loadFromFile(path + "back.png")) std::cerr << "WARNING: Could not load back.png!\n";
    else {
        backTexture.setRepeated(true);
        float scale = 720.f / static_cast<float>(backTexture.getSize().y);
        backSprite.setTexture(backTexture, true);
        backSprite.setScale({scale, scale});
        backSprite.setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(1280.f / scale), static_cast<int>(backTexture.getSize().y)}));
    }

    if (!farTexture.loadFromFile(path + "far.png")) std::cerr << "WARNING: Could not load far.png!\n";
    else {
        farTexture.setRepeated(true);
        float scale = 720.f / static_cast<float>(farTexture.getSize().y);
        farSprite.setTexture(farTexture, true);
        farSprite.setScale({scale, scale});
        farSprite.setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(1280.f / scale), static_cast<int>(farTexture.getSize().y)}));
    }

    if (!middleTexture.loadFromFile(path + "middle.png")) std::cerr << "WARNING: Could not load middle.png!\n";
    else {
        middleTexture.setRepeated(true);
        float scale = 720.f / static_cast<float>(middleTexture.getSize().y);
        middleSprite.setTexture(middleTexture, true);
        middleSprite.setScale({scale, scale});
        middleSprite.setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(1280.f / scale), static_cast<int>(middleTexture.getSize().y)}));
    }

    // PLATFORM LAYER
    if (!platformTexture.loadFromFile(path + "forest.png")) {
        std::cerr << "WARNING: Could not load forest.png for platforms!\n";
    } else {
        platformTexture.setRepeated(true);
    }

    // WORLD ITEMS
    if (!coinTex.loadFromFile(path + "Coins.png")) std::cerr << "WARNING: Could not load Coins.png\n";
    if (!shieldTex.loadFromFile(path + "shield.png")) std::cerr << "WARNING: Could not load shield.png\n";
    if (!healthTex.loadFromFile(path + "health.png")) std::cerr << "WARNING: Could not load health.png\n";

    // PROJECTILES & BOSS
    if (!arrowTex.loadFromFile(path + "Arrow.png")) std::cerr << "WARNING: Could not load Arrow.png\n";
    if (!bossTex.loadFromFile(path + "boss.png")) std::cerr << "WARNING: Could not load boss.png\n";
    if (!fireballTex.loadFromFile(path + "fireball.png")) std::cerr << "WARNING: Could not load fireball.png\n";

    // ENEMY TEXTURES
    if (!ghostTex.loadFromFile(path + "enemy.png")) std::cerr << "WARNING: Could not load enemy.png\n";
    if (!skeletonTex.loadFromFile(path + "skeleton.png")) std::cerr << "WARNING: Could not load skeleton.png\n";
    if (!spiderTex.loadFromFile(path + "spider.png")) std::cerr << "WARNING: Could not load spider.png\n";

    // POTION INITIALIZATION
    if (healthTex.getSize().x > 0) {
        healthPotion.shape.setSize(sf::Vector2f(healthTex.getSize()));
        healthPotion.shape.setScale({3.0f, 3.0f});
        healthPotion.shape.setFillColor(sf::Color::White);
        healthPotion.shape.setOutlineThickness(0.f);
    }

    if (shieldTex.getSize().x > 0) {
        shieldPickup.shape.setSize(sf::Vector2f(shieldTex.getSize()));
        shieldPickup.shape.setScale({3.0f, 3.0f});
        shieldPickup.shape.setFillColor(sf::Color::White);
        shieldPickup.shape.setOutlineThickness(0.f);
    }

    // WEAPON INITIALIZATION
    if (!bowTex.loadFromFile(path + "bow.png")) std::cerr << "WARNING: Could not load bow.png\n";
    if (bowTex.getSize().x > 0) {
        gunPickup.shape.setSize(sf::Vector2f(bowTex.getSize()));
        gunPickup.shape.setScale({4.5f, 4.5f});
        gunPickup.shape.setFillColor(sf::Color::White);
        gunPickup.shape.setOutlineThickness(0.f);
        gunPickup.shape.setTexture(&bowTex);
    }

    if (!goldenBowTex.loadFromFile(path + "goldenbow.png")) std::cerr << "WARNING: Could not load goldenbow.png\n";
    if (goldenBowTex.getSize().x > 0) {
        shotgunPickup.shape.setSize(sf::Vector2f(goldenBowTex.getSize()));
        shotgunPickup.shape.setScale({4.5f, 4.5f});
        shotgunPickup.shape.setFillColor(sf::Color::White);
        shotgunPickup.shape.setOutlineThickness(0.f);
        shotgunPickup.shape.setTexture(&goldenBowTex);
    }

    // UI ITEMS
    if (!ammoTex.loadFromFile(path + "ammo.png")) std::cerr << "WARNING: Could not load ammo.png\n";
    else {
        ammoIconSprite.setTexture(ammoTex, true);
        ammoIconSprite.setScale({2.5f, 2.5f});
    }

    if (!playerHeartTex.loadFromFile(path + "heart.png")) std::cerr << "WARNING: Could not load heart.png\n";
    else {
        playerHeartSprite.setTexture(playerHeartTex, true);
        playerHeartSprite.setScale({2.5f, 2.5f});
    }

    if (!bossHeartTex.loadFromFile(path + "bossheart.png")) std::cerr << "WARNING: Could not load bossheart.png\n";
    else {
        bossHeartSprite.setTexture(bossHeartTex, true);
        bossHeartSprite.setScale({2.5f, 2.5f});
    }

    // FONT & TEXT
    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) std::cerr << "WARNING: Could not load font.\n";

    scoreText.setCharacterSize(24); scoreText.setFillColor(sf::Color::White);
    scoreText.setOutlineColor(sf::Color::Black); scoreText.setOutlineThickness(2.f);

    coinText.setCharacterSize(24); coinText.setFillColor(sf::Color::Yellow);
    coinText.setOutlineColor(sf::Color::Black); coinText.setOutlineThickness(2.f);
    coinText.setPosition({1130.f, 20.f});

    ammoText.setCharacterSize(20); ammoText.setFillColor(sf::Color::White);
    ammoText.setOutlineColor(sf::Color::Black); ammoText.setOutlineThickness(2.f);
    ammoText.setPosition({1080.f, 60.f});

    stateText.setCharacterSize(50); stateText.setFillColor(sf::Color::Red);
    stateText.setOutlineColor(sf::Color::White); stateText.setOutlineThickness(3.f);

    loadLevel(1);
}

void Game::run() {
    sf::Clock dtClock;
    survivalClock.restart();
    enemySpawnClock.restart();
    while (window.isOpen()) {
        float dt = dtClock.restart().asSeconds();
        processEvents();
        if (gameState == GameState::PLAYING || gameState == GameState::BOSS_FIGHT) update(dt);
        render();
    }
}

void Game::loadLevel(int level) {
    currentLevel = level;
    platforms.clear(); enemies.clear(); playerBullets.clear(); bossBullets.clear(); coins.clear();
    boss.reset(); player.resetCoins();
    healthPotion.active = false; healthPotion.cooldownTimer = 0.f;
    shieldPickup.active = false; shieldPickup.cooldownTimer = 20.f;
    shotgunPickup.active = false; shotgunSpawnTimer = 0.f;
    resetGunTimer();

    float platHeight = 50.f;

    if (level == 1) {
        createPlatform(-100.f, 720.f - platHeight, 2000.f, platHeight);
        createPlatform(150.f, 550.f, 200.f, platHeight);
        createPlatform(450.f, 400.f, 250.f, platHeight);
        createPlatform(850.f, 450.f, 180.f, platHeight);
        createPlatform(1050.f, 250.f, 200.f, platHeight);
        createPlatform(300.f, 200.f, 150.f, platHeight);
        player.resetPosition(50.f, 720.f - platHeight);
    } else {
        createPlatform(-100.f, 720.f - platHeight, 2000.f, platHeight);
        player.resetPosition(50.f, 720.f - platHeight);
    }

    survivalClock.restart();
}

void Game::createPlatform(float x, float y, float width, float height) {
    sf::RectangleShape plat({width, height});
    plat.setPosition({x, y});
    plat.setTexture(&platformTexture);
    plat.setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(width), static_cast<int>(height)}));
    platforms.push_back(plat);
}

void Game::resetGunTimer() { gunSpawned = false; gunPickup.active = false; targetGunSpawnTime = 15.f + (std::rand() % 5); }

std::vector<float> Game::getValidSpawnY() const {
    std::vector<float> lanes;
    if (currentLevel == 1) lanes.push_back(player.getFloorY());
    for (const auto& plat : platforms) lanes.push_back(plat.getPosition().y);
    return lanes;
}

void Game::processEvents() {
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) window.close();
        if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {
            if (gameState == GameState::LEVEL_TRANSITION && keyEvent->code == sf::Keyboard::Key::Enter) {
                checkpointLevel = 2; checkpointScore = currentScore;
                loadLevel(2); gameState = GameState::PLAYING;
            }
            else if (gameState == GameState::GAME_OVER && keyEvent->code == sf::Keyboard::Key::R) {
                currentScore = checkpointScore; loadLevel(checkpointLevel); gameState = GameState::PLAYING;
            }
            else if (gameState == GameState::WIN && keyEvent->code == sf::Keyboard::Key::R) {
                checkpointLevel = 1; checkpointScore = 0.f; currentScore = 0.f;
                loadLevel(1); gameState = GameState::PLAYING;
            }
        }
    }
}

void Game::spawnCoin() {
    if (platforms.empty()) return;
    float spawnX, spawnY;
    int pIdx = std::rand() % platforms.size();
    const auto& plat = platforms[pIdx];

    spawnX = plat.getPosition().x + (std::rand() % static_cast<int>(std::max(1.f, plat.getSize().x - 20.f)));

    // ==========================================
    // THE FIX: FORCE COIN TO STAY ON SCREEN
    // ==========================================
    if (spawnX < 50.f) spawnX = 50.f;
    if (spawnX > 1200.f) spawnX = 1200.f;

    spawnY = plat.getPosition().y - 50.f;

    coins.emplace_back(spawnX, spawnY);
    coins.back().shape.setFillColor(sf::Color::White);
    coins.back().shape.setTexture(&coinTex);
    coins.back().shape.setScale({1.5f, 1.5f});
}

void Game::update(float dt) {
    player.update(dt, platforms, playerBullets);
    float survivalTime = survivalClock.getElapsedTime().asSeconds();
    currentScore += 120.f * dt;

    if (player.getHealth() <= 0) {
        gameState = GameState::GAME_OVER;
        stateText.setString("GAME OVER\nScore: " + std::to_string(static_cast<int>(currentScore)) + "\nPress R to Restart Level " + std::to_string(checkpointLevel));
        stateText.setFillColor(sf::Color::Red);
        sf::FloatRect bounds = stateText.getLocalBounds();
        stateText.setPosition({640.f - (bounds.size.x / 2.f), 300.f});
        return;
    }

    if (!healthPotion.active) {
        healthPotion.cooldownTimer -= dt;
        if (healthPotion.cooldownTimer <= 0.f && player.getHealth() <= 4) {
            healthPotion.active = true;
            if (!platforms.empty()) {
                int pIdx = std::rand() % platforms.size();
                float spawnX = platforms[pIdx].getPosition().x + (std::rand() % static_cast<int>(std::max(1.f, platforms[pIdx].getSize().x - 15.f)));

                healthPotion.shape.setPosition({spawnX, platforms[pIdx].getPosition().y - 70.f});
                healthPotion.shape.setFillColor(sf::Color::White);
                healthPotion.shape.setTexture(&healthTex);
                healthPotion.shape.setScale({3.0f, 3.0f});
            }
        }
    } else {
        if (player.getBounds().findIntersection(healthPotion.shape.getGlobalBounds())) {
            player.heal(4); healthPotion.active = false; healthPotion.cooldownTimer = 15.f;
        }
    }

    // ARROW PROJECTILES
    for (auto bIt = playerBullets.begin(); bIt != playerBullets.end(); ) {
        if (bIt->shape.getTexture() == nullptr) {
            bIt->shape.setTexture(&arrowTex);
            bIt->shape.setFillColor(sf::Color::White);
            bIt->shape.setSize({45.f, 15.f});
        }

        bIt->update(dt);
        bool hit = false;
        if (gameState == GameState::PLAYING) {
            for (auto eIt = enemies.begin(); eIt != enemies.end(); ) {
                if (bIt->getBounds().findIntersection(eIt->getBounds())) {
                    eIt = enemies.erase(eIt); hit = true; currentScore += 250.f; break;
                } else ++eIt;
            }
        } else if (gameState == GameState::BOSS_FIGHT && boss) {
            if (bIt->getBounds().findIntersection(boss->getBounds())) {
                int damage = bIt->isShotgun ? std::max(1, 4 - static_cast<int>(std::abs(boss->getPosition().x + boss->getSize().x/2.f - bIt->originX) / 150.f)) : 1;
                boss->takeDamage(damage); currentScore += 100.f * damage; hit = true;

                if (boss->getHealth() <= 0) {
                    currentScore += 5000.f + std::max(0.f, 15000.f - (survivalTime * 150.f));
                    if (currentLevel == 1) {
                        gameState = GameState::LEVEL_TRANSITION;
                        stateText.setString("LEVEL 1 COMPLETE!\nPress ENTER for Level 2");
                        stateText.setFillColor(sf::Color::Yellow);
                    } else {
                        gameState = GameState::WIN;
                        stateText.setString("YOU WIN!\nFinal Score: " + std::to_string(static_cast<int>(currentScore)) + "\nPress R to Restart");
                        stateText.setFillColor(sf::Color::Yellow);
                    }
                    sf::FloatRect bounds = stateText.getLocalBounds();
                    stateText.setPosition({640.f - (bounds.size.x / 2.f), 300.f});
                    return;
                }
            }
        }
        if (hit || bIt->getPosition().x > 1280 || bIt->getPosition().x < 0 || bIt->getPosition().y < 0) bIt = playerBullets.erase(bIt);
        else ++bIt;
    }

    if (gameState == GameState::PLAYING) {
        if (!gunSpawned && survivalTime >= targetGunSpawnTime && !platforms.empty()) {
            gunSpawned = true; gunPickup.active = true;
            int pIdx = std::rand() % platforms.size();
            gunPickup.shape.setPosition({platforms[pIdx].getPosition().x + (platforms[pIdx].getSize().x / 2.f) - 12.5f, platforms[pIdx].getPosition().y - 50.f});
        }
        if (gunPickup.active && player.getBounds().findIntersection(gunPickup.shape.getGlobalBounds())) {
            gunPickup.active = false; player.pickUpGun();
        }

        coinSpawnTimer += dt;
        if (coinSpawnTimer >= 5.0f && coins.empty()) { coinSpawnTimer = 0.f; spawnCoin(); }

        for (auto cIt = coins.begin(); cIt != coins.end(); ) {
            if (player.getBounds().findIntersection(cIt->shape.getGlobalBounds())) {
                cIt = coins.erase(cIt); player.addCoin(); currentScore += 500.f;
                if (player.getCoins() >= 8) {
                    gameState = GameState::BOSS_FIGHT; player.triggerBossMode(currentLevel);
                    enemies.clear(); gunPickup.active = false;

                    if (currentLevel == 1) boss = std::make_unique<Level1Boss>(1100.f, 250.f);
                    else boss = std::make_unique<CloudBoss>(640.f, 50.f);

                    break;
                }
            } else ++cIt;
        }

        float currentSpawnRate = (survivalTime < 10.f) ? 2.5f : std::max(0.8f, 2.0f - (((static_cast<int>(survivalTime) - 10) / 5) * 0.1f));
        if (enemySpawnClock.getElapsedTime().asSeconds() >= currentSpawnRate) {
            enemySpawnClock.restart();
            std::vector<float> spawnLanes = getValidSpawnY();
            if (!spawnLanes.empty()) {
                float selectedLaneY = spawnLanes[std::rand() % spawnLanes.size()];
                bool isOnBottomGround = (selectedLaneY >= 650.f);

                sf::Vector2f playerSize = player.getBounds().size;
                sf::Vector2f enemySize = {60.0f, 60.0f};

                int enemyType; float targetY; bool isFlying;

                if (isOnBottomGround) {
                    isFlying = false; enemyType = (std::rand() % 2) + 1;
                    targetY = selectedLaneY - enemySize.y;
                } else {
                    isFlying = true; enemyType = 0;
                    targetY = selectedLaneY - enemySize.y - 10.f - (std::rand() % 40);
                }

                float speed = 150.f + (((survivalTime < 10.f) ? 0 : (static_cast<int>(survivalTime) - 10) / 5) * 40.f) + (std::rand() % 200);
                enemies.emplace_back(1300.f, targetY, speed, isFlying);

                if (enemyType == 0) enemies.back().shape.setTexture(&ghostTex);
                else if (enemyType == 1) enemies.back().shape.setTexture(&skeletonTex);
                else enemies.back().shape.setTexture(&spiderTex);

                enemies.back().shape.setFillColor(sf::Color::White);
                enemies.back().shape.setSize(enemySize);
            }
        }

        for (auto it = enemies.begin(); it != enemies.end(); ) {
            it->update(dt, platforms);
            if (player.getBounds().findIntersection(it->getBounds())) { player.takeDamage(2); ++it; }
            else if (it->getPosition().x + it->getSize().x < 0) it = enemies.erase(it);
            else ++it;
        }
    }
    else if (gameState == GameState::BOSS_FIGHT && boss) {

        // ==========================================
        // NEW: BOSS TEXTURE & PIVOTING LOGIC
        // ==========================================
        if (boss->shape.getTexture() == nullptr) {
            boss->shape.setTexture(&bossTex);
            boss->shape.setFillColor(sf::Color::White);

            // ---> CHANGE BOSS SIZE HERE <---
            boss->shape.setSize({150.f, 150.f}); // Change these numbers to whatever you want!

            // Move origin to the exact center so scaling -1 flips him in place
            sf::Vector2f bSize = boss->shape.getSize();
            boss->shape.setOrigin({bSize.x / 2.f, bSize.y / 2.f});
            boss->shape.move({bSize.x / 2.f, bSize.y / 2.f});
        }

        // Pivot Boss: If player is to the left, scale is normal (1). If right, scale is flipped (-1).
        if (player.getCenter().x < boss->shape.getPosition().x) {
            boss->shape.setScale({1.f, 1.f});  // Facing Left
        } else {
            boss->shape.setScale({-1.f, 1.f}); // Facing Right
        }
        // ==========================================

        boss->update(dt, bossBullets, player.getFloorY(), player.getCenter());

        if (!shieldPickup.active) {
            shieldPickup.cooldownTimer -= dt;
            if (shieldPickup.cooldownTimer <= 0.f) {
                shieldPickup.active = true;
                if (!platforms.empty()) {
                    int pIdx = std::rand() % platforms.size();
                    shieldPickup.shape.setPosition({platforms[pIdx].getPosition().x + (std::rand() % static_cast<int>(std::max(1.f, platforms[pIdx].getSize().x - 24.f))), platforms[pIdx].getPosition().y - 70.f});
                    shieldPickup.shape.setFillColor(sf::Color::White);
                    shieldPickup.shape.setTexture(&shieldTex);
                    shieldPickup.shape.setScale({3.0f, 3.0f});
                }
            }
        } else {
            if (player.getBounds().findIntersection(shieldPickup.shape.getGlobalBounds())) {
                player.activateShield(3.0f); shieldPickup.active = false; shieldPickup.cooldownTimer = 20.f;
            }
        }

        if (player.getAmmo() == 0 && !player.getHasShotgun() && !platforms.empty()) {
            shotgunSpawnTimer += dt;
            if (shotgunSpawnTimer >= 6.0f && !shotgunPickup.active) {
                shotgunPickup.active = true; int pIdx = std::rand() % platforms.size();
                shotgunPickup.shape.setPosition({platforms[pIdx].getPosition().x + (platforms[pIdx].getSize().x / 2.f) - 17.5f, platforms[pIdx].getPosition().y - 50.f});
            }
        } else shotgunSpawnTimer = 0.f;

        if (shotgunPickup.active && player.getBounds().findIntersection(shotgunPickup.shape.getGlobalBounds())) {
            shotgunPickup.active = false; player.pickUpShotgun();
        }

        for (auto bIt = bossBullets.begin(); bIt != bossBullets.end(); ) {
            // ==========================================
            // NEW: FIREBALL ROTATION LOGIC
            // ==========================================
            if (bIt->shape.getTexture() == nullptr) {
                bIt->shape.setTexture(&fireballTex);
                bIt->shape.setFillColor(sf::Color::White);

                // Set fireball size
                bIt->shape.setSize({40.f, 20.f});

                // Center origin so it rotates correctly on its axis
                bIt->shape.setOrigin({20.f, 10.f});

                // Trig Math: calculate angle based on X/Y velocity
                float angle = std::atan2(bIt->velocity.y, bIt->velocity.x) * 180.f / 3.14159f;

                // Note: Adding 180 degrees assumes the fireball image naturally points left.
                // If it looks like it's flying backwards, change this to: angle + 0.f
                bIt->shape.setRotation(sf::degrees(angle + 180.f)); // WRAPPED IN sf::degrees()
            }
            // ==========================================

            bIt->update(dt);
            if (player.getBounds().findIntersection(bIt->getBounds())) { player.takeDamage(2); bIt = bossBullets.erase(bIt); }
            else if (bIt->getPosition().x < 0 || bIt->getPosition().x > 1280 || bIt->getPosition().y < 0 || bIt->getPosition().y > 720) bIt = bossBullets.erase(bIt);
            else ++bIt;
        }
    }

    scoreText.setString("Score: " + std::to_string(static_cast<int>(currentScore)));
    sf::FloatRect bounds = scoreText.getLocalBounds();
    scoreText.setPosition({640.f - (bounds.size.x / 2.f), 20.f});
    coinText.setString("Coins: " + std::to_string(player.getCoins()) + "/8");
}

void Game::render() {
    window.clear(sf::Color(30, 30, 30));

    window.draw(backSprite);
    window.draw(farSprite);
    window.draw(middleSprite);
    for (const auto& platform : platforms) window.draw(platform);

    for (const auto& enemy : enemies) enemy.draw(window);
    for (const auto& coin : coins) window.draw(coin.shape);
    for (const auto& bullet : playerBullets) bullet.draw(window);
    for (const auto& bullet : bossBullets) bullet.draw(window);

    if (gunPickup.active) window.draw(gunPickup.shape);
    if (shotgunPickup.active) window.draw(shotgunPickup.shape);
    if (healthPotion.active) window.draw(healthPotion.shape);
    if (shieldPickup.active) window.draw(shieldPickup.shape);

    if (gameState == GameState::BOSS_FIGHT && boss) {
        boss->draw(window);

        float bossHealthPercent = boss->getHealth() / static_cast<float>(boss->getMaxHealth());
        int heartsToDraw = static_cast<int>(bossHealthPercent * 10.f);

        for (int i = 0; i < heartsToDraw; ++i) {
            bossHeartSprite.setPosition({20.f + (i * 45.f), 100.f});
            window.draw(bossHeartSprite);
        }
    }

    player.draw(window);
    window.draw(scoreText);
    if (gameState == GameState::PLAYING) window.draw(coinText);

    for (int i = 0; i < player.getHealth(); ++i) {
        playerHeartSprite.setPosition({20.f + (i * 45.f), 60.f});
        window.draw(playerHeartSprite);
    }

    if (player.getHasWeapon()) {
        for (int i = 0; i < player.getClipAmmo(); ++i) {
            ammoIconSprite.setColor(player.getHasShotgun() ? sf::Color(255, 165, 0) : sf::Color::White);
            ammoIconSprite.setPosition({1250.f - (i * 30.f), 60.f});
            window.draw(ammoIconSprite);
        }
    }
    if (gameState == GameState::GAME_OVER || gameState == GameState::WIN || gameState == GameState::LEVEL_TRANSITION) window.draw(stateText);
    window.display();
}
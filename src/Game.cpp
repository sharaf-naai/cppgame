#include "Game.h"
#include <cstdlib>
#include <ctime>
#include <cmath>

Game::Game()
    : window(sf::VideoMode({1280, 720}), "Emberwood Arena"),
      player(50.f, 700.f), gameState(GameState::MENU), targetGunSpawnTime(0.f),
      gunSpawned(false), coinSpawnTimer(0.f), shotgunSpawnTimer(0.f), currentScore(0.f),
      isGamePaused(false), survivalTime(0.f), enemySpawnTimer(0.f), kills(0)
{
    window.setFramerateLimit(60);
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    std::string path = "Assets/";

    audio.playMusic("backmusic.wav", path);
    audio.loadSound("coin",   "coin.wav",          path);
    audio.loadSound("jump",   "jump.wav",           path);
    audio.loadSound("hurt",   "playerdamage.wav",   path);
    audio.loadSound("pickup", "itempickup.wav",     path);
    audio.loadSound("hit",    "monsterhit.wav",     path);
    audio.loadSound("shoot",  "arrowshoot.wav",     path);

    assets.loadFont("pixelfont.ttf", path);

    assets.loadTexture("back",     "back.png",    path);
    assets.loadTexture("far",      "far.png",     path);
    assets.loadTexture("middle",   "middle.png",  path);
    assets.loadTexture("platform", "forest.png",  path);

    assets.loadTexture("coin",     "Coins.png",   path);
    assets.loadTexture("shield",   "shield.png",  path);
    assets.loadTexture("health",   "health.png",  path);
    assets.loadTexture("arrow",    "Arrow.png",   path);
    assets.loadTexture("fireball", "fireball.png",path);

    assets.loadTexture("ghostFly",     "ghostFly.png",       path);
    assets.loadTexture("ghostDie",     "ghostDeath.png",     path);
    assets.loadTexture("skeletonRun",  "skeletonRun.png",    path);
    assets.loadTexture("skeletonDie",  "SkeletonDeath.png",  path);

    assets.loadTexture("bossIdle",   "spiderIdle.png",    path);
    assets.loadTexture("bossWalk",   "spiderwalk.png",    path);
    assets.loadTexture("bossAttack", "spiderAttack.png",  path);
    assets.loadTexture("bossHit",    "spiderHit.png",     path);
    assets.loadTexture("bossDeath",  "spiderDeath.png",   path);
    assets.loadTexture("bossBullet", "BulletSpider.png",  path);

    assets.getTexture("back").setRepeated(true);
    assets.getTexture("far").setRepeated(true);
    assets.getTexture("middle").setRepeated(true);
    assets.getTexture("platform").setRepeated(true);

    assets.loadTexture("playerIdle",    "Idle_2.png",   path);
    assets.loadTexture("playerRun",     "Run.png",      path);
    assets.loadTexture("playerJump",    "Jump.png",     path);
    assets.loadTexture("playerShot",    "Shot.png",     path);
    assets.loadTexture("playerHurt",    "Hurt.png",     path);
    assets.loadTexture("playerDead",    "Dead.png",     path);
    assets.loadTexture("playerRecharge","Recharge.png", path);

    assets.loadTexture("bow",       "bow.png",       path);
    assets.loadTexture("goldenbow", "goldenbow.png", path);
    assets.loadTexture("ammo",      "ammo.png",      path);
    assets.loadTexture("heart",     "heart.png",     path);
    assets.loadTexture("bossheart", "bossheart.png", path);

    level.initBackgrounds(assets.getTexture("back"), assets.getTexture("far"), assets.getTexture("middle"));
    level.initPlatforms(assets.getTexture("platform"));
    ui.init(assets.getFont(), assets.getTexture("heart"), assets.getTexture("bossheart"), assets.getTexture("ammo"));

    auto initPickup = [&](sf::RectangleShape& s, const std::string& tex, sf::Vector2f scale) {
        auto& t = assets.getTexture(tex);
        if (t.getSize().x > 0) {
            s.setSize(sf::Vector2f(t.getSize()));
            s.setScale(scale);
            s.setTexture(&t);
        }
    };
    initPickup(healthPotion.shape,  "health",    {3.f,  3.f });
    initPickup(shieldPickup.shape,  "shield",    {3.f,  3.f });
    initPickup(gunPickup.shape,     "bow",       {.08f, .08f});
    initPickup(shotgunPickup.shape, "goldenbow", {.1f,  .1f });

    player.setAnimationTextures(
        assets.getTexture("playerIdle"), assets.getTexture("playerRun"),  assets.getTexture("playerJump"),
        assets.getTexture("playerShot"), assets.getTexture("playerHurt"), assets.getTexture("playerDead"),
        assets.getTexture("playerRecharge")
    );

    setupLevel();
}

void Game::run() {
    sf::Clock dtClock;
    while (window.isOpen()) {
        float dt = dtClock.restart().asSeconds();
        processEvents();
        if (!isGamePaused && (gameState == GameState::PLAYING || gameState == GameState::BOSS_FIGHT))
            update(dt);
        render();
    }
}

void Game::setupLevel() {
    enemies.clear(); playerBullets.clear(); bossBullets.clear(); coins.clear();
    boss.reset(); player.resetCoins();
    healthPotion.active = false; healthPotion.cooldownTimer = 0.f;
    shieldPickup.active = false; shieldPickup.cooldownTimer = 20.f;
    shotgunPickup.active = false; shotgunSpawnTimer = 0.f;
    survivalTime = 0.f; enemySpawnTimer = 0.f; isGamePaused = false; kills = 0;
    resetGunTimer();

    level.setupStandardLevel();
    player.resetPosition(50.f, 720.f - 50.f);
}

void Game::resetGunTimer() {
    gunSpawned = false; gunPickup.active = false;
    targetGunSpawnTime = 15.f + (std::rand() % 5);
}

void Game::processEvents() {
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) window.close();
        if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {
            if (keyEvent->code == sf::Keyboard::Key::Q) window.close();
            if (keyEvent->code == sf::Keyboard::Key::P) {
                if (gameState == GameState::PLAYING || gameState == GameState::BOSS_FIGHT)
                    isGamePaused = !isGamePaused;
            }
            if (isGamePaused) {
                if (keyEvent->code == sf::Keyboard::Key::R)
                    { currentScore = 0.f; setupLevel(); gameState = GameState::PLAYING; }
                else if (keyEvent->code == sf::Keyboard::Key::M || keyEvent->code == sf::Keyboard::Key::Escape)
                    { isGamePaused = false; gameState = GameState::MENU; }
            } else {
                if (gameState == GameState::MENU) {
                    if (keyEvent->code == sf::Keyboard::Key::S)
                        { currentScore = 0.f; setupLevel(); gameState = GameState::PLAYING; }
                    else if (keyEvent->code == sf::Keyboard::Key::A)
                        gameState = GameState::ABOUT;
                } else if (gameState == GameState::ABOUT) {
                    if (keyEvent->code == sf::Keyboard::Key::B || keyEvent->code == sf::Keyboard::Key::Escape)
                        gameState = GameState::MENU;
                } else if (gameState == GameState::GAME_OVER || gameState == GameState::WIN) {
                    if (keyEvent->code == sf::Keyboard::Key::R)
                        { currentScore = 0.f; setupLevel(); gameState = GameState::PLAYING; }
                }
            }
        }
    }
}

void Game::spawnCoin() {
    auto platforms = level.getPlatforms();
    if (platforms.empty()) return;

    int pIdx = std::rand() % platforms.size();
    float spawnX = platforms[pIdx].getPosition().x + static_cast<float>(std::rand() % static_cast<int>(std::max(1.f, platforms[pIdx].getSize().x - 20.f)));
    spawnX = std::max(50.f, std::min(1200.f, spawnX));
    float spawnY = platforms[pIdx].getPosition().y - 50.f;

    coins.emplace_back(spawnX, spawnY);
    Coin& c = coins.back();
    c.shape.setFillColor(sf::Color::White);
    c.shape.setTexture(&assets.getTexture("coin"));
    c.totalFrames = 12;
    int fW = assets.getTexture("coin").getSize().x / 12;
    int fH = assets.getTexture("coin").getSize().y;
    c.shape.setTextureRect(sf::IntRect({0, 0}, {fW, fH}));
    c.shape.setScale({1.2f, 1.2f});
}

void Game::setupEnemyAnimation(Enemy& enemy, int enemyType) {
    if      (enemyType == 0) enemy.setAnimation(&assets.getTexture("ghostFly"),    5, &assets.getTexture("ghostDie"),    13);
    else if (enemyType == 1) enemy.setAnimation(&assets.getTexture("skeletonRun"), 6, &assets.getTexture("skeletonDie"), 13);
}

void Game::update(float dt) {
    static bool wasSpacePressed = false;
    bool isSpacePressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);
    if (isSpacePressed && !wasSpacePressed && player.getHealth() > 0) audio.playSound("jump");
    wasSpacePressed = isSpacePressed;

    int prevAmmo   = player.getAmmo();
    int prevHealth = player.getHealth();

    player.update(dt, level.getPlatforms(), playerBullets);
    if (player.getAmmo() < prevAmmo) audio.playSound("shoot");

    if (player.getHealth() <= 0) {
        if (player.isDeadAnimFinished()) gameState = GameState::GAME_OVER;
        return;
    }

    survivalTime += dt; enemySpawnTimer += dt; currentScore += 120.f * dt;

    if (!healthPotion.active) {
        healthPotion.cooldownTimer -= dt;
        if (healthPotion.cooldownTimer <= 0.f && player.getHealth() <= 10 && !level.getPlatforms().empty()) {
            healthPotion.active = true;
            int pIdx = std::rand() % level.getPlatforms().size();
            float spawnX = level.getPlatforms()[pIdx].getPosition().x +
                static_cast<float>(std::rand() % static_cast<int>(std::max(1.f, level.getPlatforms()[pIdx].getSize().x - 15.f)));
            healthPotion.shape.setPosition({spawnX, level.getPlatforms()[pIdx].getPosition().y - 70.f});
        }
    } else if (player.getBounds().findIntersection(healthPotion.shape.getGlobalBounds())) {
        player.heal(6); healthPotion.active = false; healthPotion.cooldownTimer = 15.f; audio.playSound("pickup");
    }

    for (auto bIt = playerBullets.begin(); bIt != playerBullets.end(); ) {
        if (bIt->shape.getTexture() == nullptr) {
            bIt->shape.setTexture(&assets.getTexture("arrow"));
            bIt->shape.setSize({45.f, 15.f});
        }
        bIt->update(dt);
        bool hit = false;

        if (gameState == GameState::PLAYING) {
            for (auto& enemy : enemies) {
                if (!enemy.isDying && bIt->getBounds().findIntersection(enemy.getBounds())) {
                    enemy.triggerDeath(); hit = true; currentScore += 250.f; kills++; audio.playSound("hit"); break;
                }
            }
        } else if (gameState == GameState::BOSS_FIGHT && boss) {
            if (bIt->getBounds().findIntersection(boss->getBounds())) {
                int damage = bIt->isShotgun
                    ? std::max(1, 4 - static_cast<int>(std::abs(boss->getPosition().x + boss->getSize().x / 2.f - bIt->originX) / 150.f))
                    : 1;
                boss->takeDamage(damage); currentScore += 100.f * static_cast<float>(damage); hit = true; audio.playSound("hit");

                if (boss->getHealth() <= 0 && boss->isDeadAnimFinished()) {
                    kills++;
                    currentScore += 5000.f + std::max(0.f, 15000.f - (survivalTime * 150.f));
                    gameState = GameState::WIN;
                    return;
                }
            }
        }

        if (hit || bIt->getPosition().x > 1280 || bIt->getPosition().x < 0 || bIt->getPosition().y < 0)
            bIt = playerBullets.erase(bIt);
        else ++bIt;
    }

    if (gameState == GameState::PLAYING) {
        if (!gunSpawned && survivalTime >= targetGunSpawnTime && !level.getPlatforms().empty()) {
            gunSpawned = true; gunPickup.active = true;
            int pIdx = std::rand() % level.getPlatforms().size();
            gunPickup.shape.setPosition({
                level.getPlatforms()[pIdx].getPosition().x + (level.getPlatforms()[pIdx].getSize().x / 2.f) - 12.5f,
                level.getPlatforms()[pIdx].getPosition().y - 50.f
            });
        }
        if (gunPickup.active && player.getBounds().findIntersection(gunPickup.shape.getGlobalBounds())) {
            gunPickup.active = false; player.pickUpGun(); audio.playSound("pickup");
        }

        coinSpawnTimer += dt;
        if (coinSpawnTimer >= 5.0f && coins.empty()) { coinSpawnTimer = 0.f; spawnCoin(); }

        for (auto cIt = coins.begin(); cIt != coins.end(); ) {
            if (player.getBounds().findIntersection(cIt->shape.getGlobalBounds())) {
                cIt = coins.erase(cIt); player.addCoin(); currentScore += 500.f; audio.playSound("coin");

                if (player.getCoins() >= 8) {
                    gameState = GameState::BOSS_FIGHT; player.triggerBossMode(1);
                    enemies.clear(); gunPickup.active = false;
                    level.setupBossArena();
                    boss = std::make_unique<Boss>(1100.f, 720.f - 50.f);
                    boss->setTextures(
                        assets.getTexture("bossIdle"),  assets.getTexture("bossWalk"),
                        assets.getTexture("bossAttack"),assets.getTexture("bossHit"),
                        assets.getTexture("bossDeath"), assets.getTexture("bossBullet")
                    );
                    break;
                }
            } else ++cIt;
        }

        float currentSpawnRate = (survivalTime < 10.f)
            ? 2.5f
            : std::max(0.8f, 2.0f - (((static_cast<int>(survivalTime) - 10) / 5) * 0.1f));

        if (enemySpawnTimer >= currentSpawnRate) {
            enemySpawnTimer = 0.f;
            std::vector<float> spawnLanes = level.getValidSpawnY();
            if (!spawnLanes.empty()) {
                float selectedLaneY = spawnLanes[std::rand() % spawnLanes.size()];
                bool isOnBottomGround = (selectedLaneY >= 650.f);
                sf::Vector2f enemySize;
                int enemyType; float targetY; bool isFlying;

                if (isOnBottomGround) {
                    isFlying = false; enemyType = 1; enemySize = {250.f, 250.f};
                    targetY = selectedLaneY - enemySize.y + 55.f;
                } else {
                    isFlying = true; enemyType = 0; enemySize = {200.f, 200.f};
                    targetY = selectedLaneY - enemySize.y + 30.f - static_cast<float>(std::rand() % 40);
                }

                float speed = 150.f
                    + (((survivalTime < 10.f) ? 0.f : static_cast<float>(static_cast<int>(survivalTime) - 10) / 5.f) * 40.f)
                    + static_cast<float>(std::rand() % 200);

                enemies.emplace_back(1300.f, targetY, speed, isFlying);
                setupEnemyAnimation(enemies.back(), enemyType);
                enemies.back().shape.setSize(enemySize);
            }
        }

        for (auto it = enemies.begin(); it != enemies.end(); ) {
            it->update(dt, level.getPlatforms());
            if (!it->isDying && player.getBounds().findIntersection(it->getBounds())) player.takeDamage(3);
            if (it->isDead || it->getPosition().x + it->getSize().x < 0) it = enemies.erase(it); else ++it;
        }
    }
    else if (gameState == GameState::BOSS_FIGHT && boss) {
        if (player.getCenter().x < boss->shape.getPosition().x) boss->shape.setScale({1.f, 1.f});
        else boss->shape.setScale({-1.f, 1.f});

        boss->update(dt, bossBullets, 720.f - 50.f, player.getCenter());

        if (!shieldPickup.active) {
            shieldPickup.cooldownTimer -= dt;
            if (shieldPickup.cooldownTimer <= 0.f && !level.getPlatforms().empty()) {
                shieldPickup.active = true;
                int pIdx = std::rand() % level.getPlatforms().size();
                shieldPickup.shape.setPosition({
                    level.getPlatforms()[pIdx].getPosition().x + static_cast<float>(std::rand() % static_cast<int>(std::max(1.f, level.getPlatforms()[pIdx].getSize().x - 24.f))),
                    level.getPlatforms()[pIdx].getPosition().y - 70.f
                });
            }
        } else if (player.getBounds().findIntersection(shieldPickup.shape.getGlobalBounds())) {
            player.activateShield(3.0f); shieldPickup.active = false; shieldPickup.cooldownTimer = 20.f; audio.playSound("pickup");
        }

        if (player.getAmmo() == 0 && !player.getHasShotgun() && !level.getPlatforms().empty()) {
            shotgunSpawnTimer += dt;
            if (shotgunSpawnTimer >= 6.0f && !shotgunPickup.active) {
                shotgunPickup.active = true;
                int pIdx = std::rand() % level.getPlatforms().size();
                shotgunPickup.shape.setPosition({
                    level.getPlatforms()[pIdx].getPosition().x + (level.getPlatforms()[pIdx].getSize().x / 2.f) - 17.5f,
                    level.getPlatforms()[pIdx].getPosition().y - 50.f
                });
            }
        } else shotgunSpawnTimer = 0.f;

        if (shotgunPickup.active && player.getBounds().findIntersection(shotgunPickup.shape.getGlobalBounds())) {
            shotgunPickup.active = false; player.pickUpShotgun(); audio.playSound("pickup");
        }

        for (auto bIt = bossBullets.begin(); bIt != bossBullets.end(); ) {
            bIt->update(dt);
            if (player.getBounds().findIntersection(bIt->getBounds()))
                { player.takeDamage(3); bIt = bossBullets.erase(bIt); }
            else if (bIt->getPosition().x < 0 || bIt->getPosition().x > 1280 || bIt->getPosition().y < 0 || bIt->getPosition().y > 720)
                bIt = bossBullets.erase(bIt);
            else ++bIt;
        }
    }

    if (player.getHealth() < prevHealth) audio.playSound("hurt");
    for (auto& coin : coins) coin.update(dt);
}

void Game::render() {
    window.clear(sf::Color(30, 30, 30));
    level.drawBackgrounds(window);

    if (gameState == GameState::MENU || gameState == GameState::ABOUT) {
        ui.drawMenus(window, gameState);
        window.display(); return;
    }

    level.drawPlatforms(window);

    for (const auto& enemy  : enemies)       enemy.draw(window);
    for (const auto& coin   : coins)         window.draw(coin.shape);
    for (const auto& bullet : playerBullets) bullet.draw(window);
    for (const auto& bullet : bossBullets)   bullet.draw(window);

    if (gunPickup.active)     window.draw(gunPickup.shape);
    if (shotgunPickup.active) window.draw(shotgunPickup.shape);
    if (healthPotion.active)  window.draw(healthPotion.shape);
    if (shieldPickup.active)  window.draw(shieldPickup.shape);

    if (gameState == GameState::BOSS_FIGHT && boss) boss->draw(window);

    player.draw(window);

    ui.drawHUD(window, player, boss.get(), static_cast<int>(currentScore), kills, gameState);

    if (isGamePaused && (gameState == GameState::PLAYING || gameState == GameState::BOSS_FIGHT))
        ui.drawPauseScreen(window);

    if (gameState == GameState::GAME_OVER || gameState == GameState::WIN)
        ui.drawGameOverScreen(window, gameState, static_cast<int>(currentScore), kills);

    window.display();
}

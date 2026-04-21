#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

#include "GameState.h"
#include "Player.h"
#include "Boss.h"
#include "WorldObjects.h"

// Your beautiful new architecture
#include "AssetManager.h"
#include "AudioManager.h"
#include "UIManager.h"
#include "LevelManager.h"

class Game {
private:
    sf::RenderWindow window;
    GameState gameState;

    AssetManager assets;
    AudioManager audio;
    UIManager ui;
    LevelManager level;

    Player player;
    std::unique_ptr<Boss> boss;
    std::vector<Enemy> enemies;
    std::vector<Bullet> playerBullets;
    std::vector<Bullet> bossBullets;
    std::vector<Coin> coins;

    GunPickup gunPickup;
    ShotgunPickup shotgunPickup;
    HealthPotion healthPotion;
    ShieldPickup shieldPickup;

    float targetGunSpawnTime, coinSpawnTimer, shotgunSpawnTimer, currentScore;
    bool gunSpawned, isGamePaused;
    float survivalTime, enemySpawnTimer;
    int kills;

    void setupLevel();
    void resetGunTimer();
    void processEvents();
    void spawnCoin();
    void setupEnemyAnimation(Enemy& enemy, int enemyType);
    void update(float dt);
    void render();

public:
    Game();
    void run();
};
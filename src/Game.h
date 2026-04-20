#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "GameState.h"
#include "Player.h"
#include "Boss.h"
#include "WorldObjects.h"

class Game {
private:
    sf::RenderWindow window;

    // --- EXISTING BACKGROUND/PLATFORM LAYERS ---
    sf::Texture backTexture;
    sf::Texture farTexture;
    sf::Texture middleTexture;
    sf::Texture platformTexture;

    sf::Sprite backSprite;
    sf::Sprite farSprite;
    sf::Sprite middleSprite;
    // (Note: groundSprite should still be removed)

    // --- NEW: UI & WORLD OBJECT TEXTURES ---
    sf::Texture coinTex;
    sf::Texture shieldTex;
    sf::Texture healthTex;
    sf::Texture ammoTex;
    sf::Texture playerHeartTex;
    sf::Texture bossHeartTex;

    // ADD THESE THREE NEW ENEMY TEXTURES:
    sf::Texture ghostTex;
    sf::Texture skeletonTex;
    sf::Texture spiderTex;

    sf::Texture bossTex;
    sf::Texture fireballTex;

    // --- NEW: UI SPRITES ---
    sf::Sprite playerHeartSprite;
    sf::Sprite bossHeartSprite;
    sf::Sprite ammoIconSprite;

    // ADD THESE TWO NEW TEXTURES:
    sf::Texture bowTex;
    sf::Texture goldenBowTex;

    // ADD THIS NEW TEXTURE:
    sf::Texture arrowTex;

    Player player;
    std::vector<sf::RectangleShape> platforms;
    std::vector<Enemy> enemies;
    std::vector<Bullet> playerBullets;
    std::vector<Bullet> bossBullets;
    std::vector<Coin> coins;

    std::unique_ptr<Boss> boss;
    
    GameState gameState;

    GunPickup gunPickup;
    ShotgunPickup shotgunPickup;
    HealthPotion healthPotion;
    ShieldPickup shieldPickup;

    float targetGunSpawnTime;
    bool gunSpawned;
    float coinSpawnTimer;
    float shotgunSpawnTimer;
    float currentScore;
    float checkpointScore;
    int currentLevel;
    int checkpointLevel;
    sf::Clock enemySpawnClock;
    sf::Font font;
    sf::Text scoreText, coinText, stateText, ammoText;
    sf::Clock survivalClock;

    void loadLevel(int level);
    void createPlatform(float x, float y, float width, float height);
    void resetGunTimer();
    std::vector<float> getValidSpawnY() const;
    void processEvents();
    void spawnCoin();
    void update(float dt);
    void render();

public:
    Game();
    void run();
};
#include "LevelManager.h"

LevelManager::LevelManager() : platformTexture(nullptr) {}

void LevelManager::initBackgrounds(const sf::Texture& back, const sf::Texture& far, const sf::Texture& mid) {
    float scale = 720.f / static_cast<float>(back.getSize().y);

    backSprite = std::make_unique<sf::Sprite>(back);
    backSprite->setScale({scale, scale});
    backSprite->setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(1280.f / scale), static_cast<int>(back.getSize().y)}));

    farSprite = std::make_unique<sf::Sprite>(far);
    farSprite->setScale({scale, scale});
    farSprite->setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(1280.f / scale), static_cast<int>(far.getSize().y)}));

    middleSprite = std::make_unique<sf::Sprite>(mid);
    middleSprite->setScale({scale, scale});
    middleSprite->setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(1280.f / scale), static_cast<int>(mid.getSize().y)}));
}

void LevelManager::initPlatforms(const sf::Texture& platTex) {
    platformTexture = &platTex;
}

void LevelManager::createPlatform(float x, float y, float width, float height) {
    sf::RectangleShape plat({width, height});
    plat.setPosition({x, y});

    if (platformTexture) {
        plat.setTexture(platformTexture);
        plat.setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(width), static_cast<int>(height)}));
    }

    platforms.push_back(plat);
}

void LevelManager::setupStandardLevel() {
    platforms.clear();
    float platHeight = 50.f;
    createPlatform(-100.f, 720.f - platHeight, 2000.f, platHeight);
    createPlatform(150.f,  480.f, 200.f, platHeight);
    createPlatform(450.f,  400.f, 250.f, platHeight);
    createPlatform(850.f,  450.f, 180.f, platHeight);
    createPlatform(1050.f, 250.f, 200.f, platHeight);
    createPlatform(300.f,  200.f, 150.f, platHeight);
}

void LevelManager::setupBossArena() {
    platforms.clear();
    createPlatform(-100.f, 720.f - 50.f, 2000.f, 50.f);
}

void LevelManager::drawBackgrounds(sf::RenderWindow& window) {
    if (backSprite)   window.draw(*backSprite);
    if (farSprite)    window.draw(*farSprite);
    if (middleSprite) window.draw(*middleSprite);
}

void LevelManager::drawPlatforms(sf::RenderWindow& window) {
    for (const auto& platform : platforms)
        window.draw(platform);
}

const std::vector<sf::RectangleShape>& LevelManager::getPlatforms() const {
    return platforms;
}

std::vector<float> LevelManager::getValidSpawnY() const {
    std::vector<float> lanes;
    for (const auto& plat : platforms)
        lanes.push_back(plat.getPosition().y);
    return lanes;
}

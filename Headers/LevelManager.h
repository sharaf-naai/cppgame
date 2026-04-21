#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

class LevelManager {
private:
    std::unique_ptr<sf::Sprite> backSprite;
    std::unique_ptr<sf::Sprite> farSprite;
    std::unique_ptr<sf::Sprite> middleSprite;

    std::vector<sf::RectangleShape> platforms;
    const sf::Texture* platformTexture;

    void createPlatform(float x, float y, float width, float height);

public:
    LevelManager();

    void initBackgrounds(const sf::Texture& back, const sf::Texture& far, const sf::Texture& mid);
    void initPlatforms(const sf::Texture& platTex);

    void setupStandardLevel();
    void setupBossArena();

    void drawBackgrounds(sf::RenderWindow& window);
    void drawPlatforms(sf::RenderWindow& window);

    const std::vector<sf::RectangleShape>& getPlatforms() const;
    std::vector<float> getValidSpawnY() const;
};
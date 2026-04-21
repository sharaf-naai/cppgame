#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "GameState.h"
#include "Player.h"
#include "Boss.h"

class UIManager {
private:
    std::unique_ptr<sf::Text> scoreText;
    std::unique_ptr<sf::Text> coinText;
    std::unique_ptr<sf::Text> ammoText;
    std::unique_ptr<sf::Text> killText;
    std::unique_ptr<sf::Text> stateText;
    std::unique_ptr<sf::Text> menuText;

    std::unique_ptr<sf::Sprite> playerHeartSprite;
    std::unique_ptr<sf::Sprite> bossHeartSprite;
    std::unique_ptr<sf::Sprite> ammoIconSprite;

public:
    UIManager();

    void init(const sf::Font& font, const sf::Texture& heartTex, const sf::Texture& bossHeartTex, const sf::Texture& ammoTex);
    void drawHUD(sf::RenderWindow& window, Player& player, Boss* boss, int score, int kills, GameState state);
    void drawMenus(sf::RenderWindow& window, GameState state);
    void drawPauseScreen(sf::RenderWindow& window);
    void drawGameOverScreen(sf::RenderWindow& window, GameState state, int score, int kills);
};
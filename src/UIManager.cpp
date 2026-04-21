#include "UIManager.h"
#include <string>

UIManager::UIManager() {}

void UIManager::init(const sf::Font& font, const sf::Texture& heartTex, const sf::Texture& bossHeartTex, const sf::Texture& ammoTex) {
    // --- Setup Text ---
    scoreText = std::make_unique<sf::Text>(font, "", 24);
    scoreText->setFillColor(sf::Color::White);
    scoreText->setOutlineColor(sf::Color::Black);
    scoreText->setOutlineThickness(2.f);

    coinText = std::make_unique<sf::Text>(font, "", 24);
    coinText->setFillColor(sf::Color::Yellow);
    coinText->setOutlineColor(sf::Color::Black);
    coinText->setOutlineThickness(2.f);
    coinText->setPosition({1130.f, 20.f});

    killText = std::make_unique<sf::Text>(font, "", 24);
    killText->setFillColor(sf::Color(255, 100, 100));
    killText->setOutlineColor(sf::Color::Black);
    killText->setOutlineThickness(2.f);
    killText->setPosition({20.f, 20.f});

    stateText = std::make_unique<sf::Text>(font, "", 50);
    stateText->setOutlineColor(sf::Color::White);
    stateText->setOutlineThickness(3.f);

    menuText = std::make_unique<sf::Text>(font, "", 30);
    menuText->setOutlineColor(sf::Color::Black);
    menuText->setOutlineThickness(3.f);

    // --- Setup Sprites ---
    playerHeartSprite = std::make_unique<sf::Sprite>(heartTex);
    playerHeartSprite->setScale({2.5f, 2.5f});

    bossHeartSprite = std::make_unique<sf::Sprite>(bossHeartTex);
    bossHeartSprite->setScale({2.5f, 2.5f});

    ammoIconSprite = std::make_unique<sf::Sprite>(ammoTex);
    if (ammoTex.getSize().x > 0) {
        float scaleX = 25.f / static_cast<float>(ammoTex.getSize().x);
        float scaleY = 25.f / static_cast<float>(ammoTex.getSize().y);
        ammoIconSprite->setScale({scaleX, scaleY});
    }
}

void UIManager::drawHUD(sf::RenderWindow& window, Player& player, Boss* boss, int score, int kills, GameState state) {
    if (!scoreText) return; // Failsafe if init wasn't called

    scoreText->setString("Score: " + std::to_string(score));
    sf::FloatRect bounds = scoreText->getLocalBounds();
    scoreText->setPosition({640.f - (bounds.size.x / 2.f), 20.f});
    window.draw(*scoreText);

    killText->setString("Kills: " + std::to_string(kills));
    window.draw(*killText);

    if (state == GameState::PLAYING) {
        coinText->setString("Coins: " + std::to_string(player.getCoins()) + "/8");
        window.draw(*coinText);
    }

    if (state == GameState::BOSS_FIGHT && boss) {
        float bossHealthPercent = boss->getHealth() / static_cast<float>(boss->getMaxHealth());
        int heartsToDraw = static_cast<int>(bossHealthPercent * 10.f);
        for (int i = 0; i < heartsToDraw; ++i) {
            bossHeartSprite->setPosition({20.f + (i * 45.f), 100.f});
            window.draw(*bossHeartSprite);
        }
    }

    int fullHearts = player.getHealth() / 2;
    bool hasHalfHeart = player.getHealth() % 2 != 0;

    // SFML 3 Fix: getTexture() returns a reference, use the dot operator (.)
    int hWidth = playerHeartSprite->getTexture().getSize().x;
    int hHeight = playerHeartSprite->getTexture().getSize().y;

    for (int i = 0; i < fullHearts; ++i) {
        playerHeartSprite->setTextureRect(sf::IntRect({0, 0}, {hWidth, hHeight}));
        playerHeartSprite->setPosition({20.f + (i * 45.f), 60.f});
        window.draw(*playerHeartSprite);
    }
    if (hasHalfHeart) {
        playerHeartSprite->setTextureRect(sf::IntRect({0, 0}, {hWidth / 2, hHeight}));
        playerHeartSprite->setPosition({20.f + (fullHearts * 45.f), 60.f});
        window.draw(*playerHeartSprite);
    }

    if (player.getHasWeapon()) {
        for (int i = 0; i < player.getClipAmmo(); ++i) {
            ammoIconSprite->setColor(player.getHasShotgun() ? sf::Color(255, 165, 0) : sf::Color::White);
            ammoIconSprite->setPosition({1250.f - (i * 30.f), 60.f});
            window.draw(*ammoIconSprite);
        }
    }
}

void UIManager::drawMenus(sf::RenderWindow& window, GameState state) {
    if (!menuText) return;

    sf::RectangleShape overlay({1280.f, 720.f});
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(overlay);

    if (state == GameState::MENU) {
        menuText->setCharacterSize(80);
        menuText->setFillColor(sf::Color::Yellow);
        menuText->setString("EMBERWOOD ARENA");
        sf::FloatRect bounds = menuText->getLocalBounds();
        menuText->setPosition({640.f - bounds.size.x / 2.f, 150.f});
        window.draw(*menuText);

        menuText->setCharacterSize(35);
        menuText->setFillColor(sf::Color::White);
        menuText->setString("Press 'S' to Start\n\nPress 'A' for About\n\nPress 'Q' to Quit");
        bounds = menuText->getLocalBounds();
        menuText->setPosition({640.f - bounds.size.x / 2.f, 350.f});
        window.draw(*menuText);
    } else if (state == GameState::ABOUT) {
        menuText->setCharacterSize(60);
        menuText->setFillColor(sf::Color::Cyan);
        menuText->setString("HOW TO PLAY");
        sf::FloatRect bounds = menuText->getLocalBounds();
        menuText->setPosition({640.f - bounds.size.x / 2.f, 100.f});
        window.draw(*menuText);

        menuText->setCharacterSize(30);
        menuText->setFillColor(sf::Color::White);
        menuText->setString("Left / Right Arrows : Move\nSpacebar : Jump\nX : Shoot Weapon\nC : Manual Reload\nP : Pause Game\n\nCollect 8 Coins to summon the Boss!\n\nPress 'B' to Return to Menu");
        bounds = menuText->getLocalBounds();
        menuText->setPosition({640.f - bounds.size.x / 2.f, 250.f});
        window.draw(*menuText);
    }
}

void UIManager::drawPauseScreen(sf::RenderWindow& window) {
    if (!menuText) return;

    sf::RectangleShape overlay({1280.f, 720.f});
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(overlay);

    menuText->setCharacterSize(80);
    menuText->setFillColor(sf::Color::Yellow);
    menuText->setString("PAUSED");
    sf::FloatRect bounds = menuText->getLocalBounds();
    menuText->setPosition({640.f - bounds.size.x / 2.f, 250.f});
    window.draw(*menuText);

    menuText->setCharacterSize(30);
    menuText->setFillColor(sf::Color::White);
    menuText->setString("Press 'P' to Resume\nPress 'R' to Restart\nPress 'M' for Main Menu\nPress 'Q' to Quit");
    sf::FloatRect sBounds = menuText->getLocalBounds();
    menuText->setPosition({640.f - sBounds.size.x / 2.f, 400.f});
    window.draw(*menuText);
}

void UIManager::drawGameOverScreen(sf::RenderWindow& window, GameState state, int score, int kills) {
    if (!stateText) return;

    sf::RectangleShape overlay({1280.f, 720.f});
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(overlay);

    if (state == GameState::GAME_OVER) {
        stateText->setFillColor(sf::Color::Red);
        stateText->setString("GAME OVER\nScore: " + std::to_string(score) + "\nKills: " + std::to_string(kills) + "\nPress R to Restart\nPress Q to Quit");
    } else {
        stateText->setFillColor(sf::Color::Green);
        stateText->setString("YOU WIN!\nFinal Score: " + std::to_string(score) + "\nTotal Kills: " + std::to_string(kills) + "\nPress R to Restart\nPress Q to Quit");
    }

    sf::FloatRect bounds = stateText->getLocalBounds();
    stateText->setPosition({640.f - (bounds.size.x / 2.f), 360.f - (bounds.size.y / 2.f)});
    window.draw(*stateText);
}
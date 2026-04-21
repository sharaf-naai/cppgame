#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <iostream>

class AssetManager {
private:
    std::map<std::string, sf::Texture> textures;
    sf::Font mainFont;

public:
    void loadTexture(const std::string& name, const std::string& fileName, const std::string& path);
    sf::Texture& getTexture(const std::string& name);

    void loadFont(const std::string& fileName, const std::string& path);
    sf::Font& getFont();
};
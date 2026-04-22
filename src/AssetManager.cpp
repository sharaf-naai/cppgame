#include "AssetManager.h"

void AssetManager::loadTexture(const std::string& name, const std::string& fileName, const std::string& path) {
    sf::Texture tex;
    if (tex.loadFromFile(path + fileName))
        textures[name] = std::move(tex);
    else
        std::cerr << "CRITICAL WARNING: Missing Texture: " << fileName << "\n";
}

sf::Texture& AssetManager::getTexture(const std::string& name) {
    return textures.at(name);
}

void AssetManager::loadFont(const std::string& fileName, const std::string& path) {
    if (!mainFont.openFromFile(path + fileName))
        std::cerr << "CRITICAL ERROR: Could not load font: " << fileName << "\n";
}

sf::Font& AssetManager::getFont() {
    return mainFont;
}

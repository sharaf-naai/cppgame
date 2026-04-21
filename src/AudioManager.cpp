#include "AudioManager.h"
#include <iostream>

void AudioManager::loadSound(const std::string& name, const std::string& fileName, const std::string& path) {
    sf::SoundBuffer buffer;
    if (buffer.loadFromFile(path + fileName)) {
        buffers[name] = std::move(buffer);

        // Safely emplace the sound into the map using the buffer
        sounds.emplace(name, sf::Sound(buffers.at(name)));
    } else {
        std::cerr << "CRITICAL WARNING: Missing Audio: " << fileName << "\n";
    }
}

void AudioManager::playSound(const std::string& name) {
    // SFML 3 FIX: We use an iterator (it) instead of sounds[name].
    // This prevents C++ from trying to build a default/blank sf::Sound!
    auto it = sounds.find(name);
    if (it != sounds.end()) {
        it->second.play(); // "second" gets the actual sf::Sound from the map
    }
}

void AudioManager::playMusic(const std::string& fileName, const std::string& path) {
    if (backgroundMusic.openFromFile(path + fileName)) {
        backgroundMusic.setLooping(true);
        backgroundMusic.setVolume(30.f);
        backgroundMusic.play();
    }
}
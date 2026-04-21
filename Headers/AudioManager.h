#pragma once
#include <SFML/Audio.hpp>
#include <map>
#include <string>

class AudioManager {
private:
    std::map<std::string, sf::SoundBuffer> buffers;
    std::map<std::string, sf::Sound> sounds;
    sf::Music backgroundMusic;

public:
    void loadSound(const std::string& name, const std::string& fileName, const std::string& path);
    void playSound(const std::string& name);
    void playMusic(const std::string& fileName, const std::string& path);
};
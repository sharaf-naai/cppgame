cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(main
        src/main.cpp
        src/Game.cpp
        src/Player.cpp
        src/Boss.cpp
        Headers/Game.h      # <--- Update these
        Headers/Player.h    # <--- Update these
        Headers/Boss.h      # <--- Update these
        Headers/Entity.h    # <--- Update these
        Headers/WorldObjects.h
        Headers/GameState.h
        Headers/AssetManager.h
        Headers/AudioManager.h
        Headers/UIManager.h
        Headers/LevelManager.h
        src/AssetManager.cpp
        src/UIManager.cpp
        src/LevelManager.cpp
        src/AudioManager.cpp
)

target_include_directories(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Headers)
target_compile_features(main PRIVATE cxx_std_17)
target_link_libraries(main PRIVATE SFML::Graphics SFML::Audio)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT main)
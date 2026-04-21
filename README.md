# Emberwood Arena

![C++17](https://img.shields.io/badge/C++-17-blue.svg)
![SFML](https://img.shields.io/badge/SFML-3.0.2-green.svg)
![CMake](https://img.shields.io/badge/CMake-3.28+-orange.svg)

**Emberwood Arena** is a fast-paced, single-level 2D arcade platformer built from scratch using modern C++17 and the newly released SFML 3.

Survive a dangerous forest arena with a deadly lava floor, dodge descending enemies, and collect 8 golden coins to summon the final Boss. Manage your health, utilize shields, and swap between a standard Bow and a devastating close-range Golden Bow (Shotgun) to claim victory.

## 📸 Screenshots

| Main Menu | Gameplay & Parkour |
| :---: | :---: |
| *![Main Menu](path/to/your/menu_screenshot.png)* <br> *Clean, state-driven UI menu allowing seamless transitions.* | *![Gameplay](path/to/your/gameplay_screenshot.png)* <br> *Navigating the platforms while avoiding enemies and the lava floor.* |

| Weapons & Pickups | The Final Boss |
| :---: | :---: |
| *![Pickups](path/to/your/pickups_screenshot.png)* <br> *Collecting shields and swapping to the Golden Bow for burst damage.* | *![Boss Fight](path/to/your/boss_screenshot.png)* <br> *Dodging targeted fireballs in the final showdown.* |

*(Note: Replace `path/to/your/...` with actual image links once you upload them to your repository!)*

## 🎮 Gameplay & Features

* **Arcade Survival:** Continuously dodge enemies (Ghosts, Skeletons, Spiders) that descend from above.
* **The Floor is Lava:** Missing a jump and hitting the bottom floor results in instant death.
* **Dynamic Weaponry:** * **Standard Bow:** Reliable, long-range linear damage.
   * **Golden Bow (Shotgun):** Spawns when out of ammo during the boss fight. Deals massive damage at close range but heavily drops off at a distance.
* **Resource Management:** Collect Health Potions (only spawn when health is critically low) and timed Shield drops to survive.
* **Boss Fight:** Collecting 8 coins triggers the boss phase. The boss tracks the player, pivots dynamically, and unleashes rotating fireball barrages with dedicated pause/vulnerability windows.
* **Score System:** Points are awarded for survival time, killing enemies, grabbing coins, and landing boss hits, with a massive time-bonus for defeating the boss quickly.

## ⌨️ Controls

* **Left / Right Arrows:** Move Player
* **Spacebar:** Jump
* **X:** Shoot Weapon
* **S:** Start Game (From Menu)
* **A:** About / How to Play (From Menu)
* **R:** Restart (On Game Over / Win)
* **Q:** Quit Game

## 🏗️ Object-Oriented Architecture

This project was built with a strong emphasis on clean, modular Object-Oriented Programming (OOP) principles:

* **`Game` Class (The Manager):** Acts as the central state machine. It handles the `SFML` window context, event polling, delta-time (`dt`) calculation, rendering layers, and transitions between `GameState` enums (MENU, ABOUT, PLAYING, BOSS_FIGHT, etc.).
* **`Player` Class:** Encapsulates all player logic, including AABB collision detection against platforms, gravity/jump physics, i-frame timers, weapon states (ammo clips, reloading), and rendering.
* **`Boss` & `Entity` Hierarchy:** Enemies and the Boss are separated into distinct classes to isolate their AI behaviors. The Boss utilizes a state-driven attack loop (tracking the player, calculating `atan2` angles for fireball trajectories, and managing cooldown phases).
* **Struct-Based Data:** Lightweight objects like `Bullet`, `Coin`, and `ShieldPickup` are implemented as simple structs with their own internal `update()` logic to keep the main game loop clean.

## 🚀 Building the Project

This project uses **CMake** and **FetchContent** to automatically download and link SFML 3. You do not need to install SFML manually on your system!

### Prerequisites
* A C++17 compatible compiler (GCC, Clang, MSVC)
* CMake (3.28 or higher)

### Build Instructions

1. Clone the repository:
   ```bash
   git clone https://github.com/sharaf-naai/cppgame.git

2. Generate the build files:
   ```bash
   cmake -B build

3. Compile the game:

   ```bash
   cmake --build build --config Release

Run the executable located in the build/bin/ folder.

🎨 Assets & Credits
All visual assets, sprites, and textures used in this project were sourced from talented creators on itch.io.

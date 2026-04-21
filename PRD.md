# Product Requirements Document (PRD)
## Forest Platformer — Classic Mode

---

**Document Version:** 1.0  
**Author:** Zidane Nait Cheikh  
**Date:** April 20, 2026  
**Status:** Final  

---

## Table of Contents

1. [Product Overview](#1-product-overview)
2. [Goals & Objectives](#2-goals--objectives)
3. [Target Audience](#3-target-audience)
4. [Scope](#4-scope)
5. [Technical Requirements](#5-technical-requirements)
6. [Functional Requirements](#6-functional-requirements)
7. [Non-Functional Requirements](#7-non-functional-requirements)
8. [Game Systems Specification](#8-game-systems-specification)
9. [User Interface Requirements](#9-user-interface-requirements)
10. [Asset Requirements](#10-asset-requirements)
11. [Architecture Requirements](#11-architecture-requirements)
12. [Out of Scope](#12-out-of-scope)
13. [Risks & Mitigations](#13-risks--mitigations)
14. [Acceptance Criteria](#14-acceptance-criteria)

---

## 1. Product Overview

**Forest Platformer — Classic Mode** is a 2D side-scrolling action-platformer developed in C++17 using the SFML 3.0.2 graphics library. The player controls a character navigating a forest-themed level, defeating enemies, collecting coins, and ultimately confronting a boss enemy. The game targets players who enjoy classic arcade-style platformers with a progressive difficulty curve and a clear win condition.

The project serves simultaneously as a functional game and as a software engineering demonstration, showcasing object-oriented programming principles, real-time game loop design, collision detection, and state machine management in C++.

---

## 2. Goals & Objectives

| # | Goal | Measurable Outcome |
|---|------|--------------------|
| G-01 | Deliver a complete game loop with a defined start and end | Player can reach WIN or GAME_OVER state from the MENU state |
| G-02 | Implement a scalable enemy system with increasing difficulty | Spawn rate decreases from 2.5 s to 0.8 s over survival time |
| G-03 | Implement a boss fight as the game's climax | Boss is summoned after 8 coins; game ends on boss defeat |
| G-04 | Demonstrate OOP architecture in C++ | Entity hierarchy with polymorphic Boss classes compiles and runs correctly |
| G-05 | Achieve stable 60 FPS rendering | Frame rate cap set via `window.setFramerateLimit(60)` |
| G-06 | Support cross-platform execution | Builds and runs on both Windows and macOS via CMake |

---

## 3. Target Audience

- **Primary:** Computer science students and beginner-to-intermediate C++ developers interested in game development with SFML.
- **Secondary:** Casual gamers who enjoy retro 2D platformers with survival mechanics.
- **Tertiary:** Academic evaluators assessing the quality and completeness of a C++ game project.

---

## 4. Scope

### 4.1 In Scope

- Single-player, single-level gameplay
- Parallax scrolling background (3 layers)
- Player movement, jumping, and projectile shooting
- Three enemy types with distinct behavior (Ghost, Skeleton, Spider)
- One boss type (Level1Boss) with a two-phase behavior pattern
- Coin collection as the progression trigger
- Four pickup types: Bow, Golden Bow/Shotgun, Health Potion, Shield
- Score system based on survival time, kills, and collection
- Six distinct game states managed by a finite state machine
- Keyboard-only input

### 4.2 Out of Scope

- Multiplayer functionality
- Level 2 (CloudBoss is implemented but not integrated)
- Sound and music
- Save/load system
- Mouse-driven UI
- Mobile or console platforms

---

## 5. Technical Requirements

| ID | Requirement | Details |
|----|-------------|---------|
| TR-01 | Language | C++17 |
| TR-02 | Graphics Library | SFML 3.0.2 (fetched via CMake FetchContent) |
| TR-03 | Build System | CMake ≥ 3.28 |
| TR-04 | Window Resolution | 1280 × 720 pixels, fixed |
| TR-05 | Frame Rate | Locked at 60 FPS |
| TR-06 | Compiler Support | GCC, Clang, or MSVC with C++17 support |
| TR-07 | Platform | Windows 10+, macOS 12+ |
| TR-08 | Memory | Smart pointer (`std::unique_ptr`) used for boss ownership |

---

## 6. Functional Requirements

### 6.1 Game State Machine

| State | Description | Transition Conditions |
|-------|-------------|----------------------|
| `MENU` | Main menu with title and keyboard options | Press `S` → PLAYING; Press `A` → ABOUT; Press `Q` → Exit |
| `ABOUT` | How-to-play screen | Press `B` or `Esc` → MENU |
| `PLAYING` | Active gameplay | 8 coins collected → BOSS_FIGHT; Player HP = 0 → GAME_OVER |
| `BOSS_FIGHT` | Boss encounter | Boss HP = 0 → WIN; Player HP = 0 → GAME_OVER |
| `GAME_OVER` | Death screen with final score | Press `R` → PLAYING (restart) |
| `WIN` | Victory screen with final score | Press `R` → PLAYING (restart) |

### 6.2 Player

| ID | Requirement |
|----|-------------|
| FR-P01 | Player spawns at position (50, 670) at level start |
| FR-P02 | Player moves horizontally using Left/Right arrow keys at 300 px/s |
| FR-P03 | Player jumps with Spacebar; jump force = −900 px/s², gravity = 1500 px/s² |
| FR-P04 | Player sprite flips horizontally to match movement direction |
| FR-P05 | Player starts with 10 HP; HP cannot exceed 10 |
| FR-P06 | Player receives 1.2 seconds of invincibility frames after taking damage |
| FR-P07 | Player is clamped to the horizontal window bounds [0, 1220] |
| FR-P08 | Player shoots with `X` key; shooting direction follows facing direction or `Up` key for upward shot |
| FR-P09 | Player cannot shoot while reloading (reload time: 1.5 s) |
| FR-P10 | Player collision detection uses AABB (Axis-Aligned Bounding Box) against all platforms |

### 6.3 Enemy System

| ID | Requirement |
|----|-------------|
| FR-E01 | Enemies spawn off-screen to the right (x = 1300) |
| FR-E02 | Ghost enemies are flying; Skeleton and Spider enemies are ground-bound with gravity |
| FR-E03 | All enemies move leftward at a speed of 150–350 px/s (scales with survival time) |
| FR-E04 | Spawn rate starts at 2.5 s and decreases by 0.1 s per 5-second interval, floored at 0.8 s |
| FR-E05 | Contact with an enemy deals 2 damage to the player |
| FR-E06 | Player arrows destroy an enemy on contact; player scores 250 points per kill |
| FR-E07 | Enemies that exit the left boundary (x < 0) are removed from memory |

### 6.4 Coin & Boss Trigger

| ID | Requirement |
|----|-------------|
| FR-C01 | One coin spawns every 5 seconds on a random platform if no coin is currently present |
| FR-C02 | Collecting a coin grants 500 points and increments the coin counter |
| FR-C03 | Collecting the 8th coin immediately triggers BOSS_FIGHT state |
| FR-C04 | On BOSS_FIGHT trigger, all existing enemies are cleared and the Level1Boss is instantiated |
| FR-C05 | On BOSS_FIGHT trigger, the player receives a Bow with 30 arrows |

### 6.5 Boss (Level1Boss)

| ID | Requirement |
|----|-------------|
| FR-B01 | Level1Boss has 40 HP and a movement speed of 200 px/s |
| FR-B02 | Boss alternates between two states: SHOOTING (state 0) and REPOSITION (state 1) |
| FR-B03 | In SHOOTING state, boss moves vertically, bouncing between y=50 and the floor |
| FR-B04 | Boss fires a 3-bullet spread aimed at the player every 1.2 seconds |
| FR-B05 | After 6 volleys, boss enters REPOSITION state for 4 seconds, then teleports to a new X position |
| FR-B06 | Boss sprite flips to face the player horizontally |
| FR-B07 | Each arrow hit deals 1 damage; shotgun damage scales with proximity (1–4) |
| FR-B08 | Boss defeat grants 5000 base points + up to 15,000 time-bonus points |

### 6.6 Pickup System

| Pickup | Trigger | Effect | Cooldown |
|--------|---------|--------|----------|
| Bow | Spawns 15–20 s after level start on a random platform | Grants 15 arrows (clip: 5) | Consumed on pickup |
| Health Potion | Spawns when player HP ≤ 4, after a 15 s cooldown | Restores 4 HP | 15 s after use |
| Shield | Spawns in BOSS_FIGHT, 20 s cooldown | Grants 3 s of full immunity | 20 s after use |
| Golden Bow | Spawns in BOSS_FIGHT if ammo = 0 and no shotgun, after 6 s | Grants shotgun with 15 shells (clip: 3) | Consumed on pickup |

### 6.7 Score System

| Source | Points Awarded |
|--------|---------------|
| Survival time | 120 points/second |
| Enemy kill | 250 points |
| Coin collected | 500 points |
| Boss hit | 100 × damage dealt |
| Boss defeated | 5,000 + max(0, 15,000 − survivalTime × 150) |

---

## 7. Non-Functional Requirements

| ID | Category | Requirement |
|----|----------|-------------|
| NFR-01 | Performance | Game loop must run at a stable 60 FPS on standard hardware |
| NFR-02 | Memory Safety | No raw `new`/`delete` for game objects; use RAII and STL containers |
| NFR-03 | Maintainability | Code is split across purpose-specific files (Entity, WorldObjects, Player, Boss, Game) |
| NFR-04 | Extensibility | Boss system uses abstract base class to allow future boss types without modifying Game |
| NFR-05 | Robustness | All file loads emit a warning to `std::cerr` on failure; game does not crash on missing asset |
| NFR-06 | Portability | All asset paths are relative; no hardcoded absolute OS-specific paths |

---

## 8. Game Systems Specification

### 8.1 Physics Model

The game uses a custom Euler-integration physics model:
- **Gravity constant:** 1500 px/s²
- **Jump force:** −900 px/s (initial upward velocity)
- **Horizontal movement:** discrete velocity assignment (no acceleration/friction)
- **Collision resolution:** AABB overlap detection with positional correction

### 8.2 Camera

The camera is fixed (no scrolling). The entire level fits within the 1280×720 viewport. Parallax background layers are rendered with scaled texture repetition.

### 8.3 Parallax Background

Three background layers (`back.png`, `far.png`, `middle.png`) are each scaled to fill the 720 px window height and rendered as repeated textures. Static in the current implementation (no horizontal scroll offset).

### 8.4 Platform Layout

| # | X | Y | Width | Height |
|---|---|---|-------|--------|
| 0 (ground) | −100 | 670 | 2000 | 50 |
| 1 | 150 | 550 | 200 | 50 |
| 2 | 450 | 400 | 250 | 50 |
| 3 | 850 | 450 | 180 | 50 |
| 4 | 1050 | 250 | 200 | 50 |
| 5 | 300 | 200 | 150 | 50 |

---

## 9. User Interface Requirements

### 9.1 HUD (In-Game)

| Element | Position | Description |
|---------|----------|-------------|
| Score | Centered, top | "Score: XXXXX" in white with black outline |
| Coin counter | Top-right (~1130, 20) | "Coins: X/8" in yellow |
| Player HP hearts | Top-left (20+i×45, 60) | One heart sprite per HP point |
| Boss HP hearts | Top-left (20+i×45, 100) | One boss-heart sprite per 10% HP |
| Ammo icons | Top-right, stacked right-to-left | One icon per clip bullet remaining |

### 9.2 Menu Screens

- **Main Menu:** Game title in large yellow text; keyboard shortcut prompts in white
- **About Screen:** Controls reference in cyan/white
- **Game Over / Win Screen:** Full-screen overlay with final score and restart prompt

### 9.3 Visual Feedback

| Event | Visual |
|-------|--------|
| Player hit | Sprite flashes (alternating 100% / 40% opacity at 0.2 s intervals) for 1.2 s |
| Shield active | Cyan translucent circle aura drawn around the player |
| Boss facing player | Boss sprite x-scale flipped (−1 or +1) |

---

## 10. Asset Requirements

| Filename | Type | Usage |
|----------|------|-------|
| `player.png` | Sprite | Player character |
| `back.png` | Texture | Background layer 1 (far) |
| `far.png` | Texture | Background layer 2 |
| `middle.png` | Texture | Background layer 3 (near) |
| `forest.png` | Texture | Platform surface tile |
| `enemy.png` | Sprite | Ghost enemy |
| `skeleton.png` | Sprite | Skeleton enemy |
| `spider.png` | Sprite | Spider enemy |
| `boss.png` | Sprite | Level 1 Boss |
| `fireball.png` | Sprite | Boss projectile |
| `Arrow.png` | Sprite | Player projectile |
| `bow.png` | Sprite | Bow pickup icon |
| `goldenbow.png` | Sprite | Golden Bow pickup icon |
| `Coins.png` | Sprite | Coin collectible |
| `health.png` | Sprite | Health potion pickup |
| `shield.png` | Sprite | Shield pickup |
| `heart.png` | Sprite | Player HP HUD icon |
| `bossheart.png` | Sprite | Boss HP HUD icon |
| `ammo.png` | Sprite | Ammo HUD icon |
| `pixelfont.ttf` | Font | All in-game text |

All assets reside in `src/` and are loaded with the relative path `../src/` from the CMake binary directory.

---

## 11. Architecture Requirements

### 11.1 Class Hierarchy

```
Entity  (abstract base: shape, getBounds, getPosition, getSize, draw)
├── Player      (movement, shooting, health, coins, pickups)
└── Boss        (abstract: update is pure virtual)
    ├── Level1Boss   (vertical movement + spread fire)
    └── CloudBoss    (horizontal movement + rain attack — implemented, not yet integrated)

WorldObjects.h  (standalone structs)
├── Bullet      (extends Entity, velocity, shotgun flag)
├── Enemy       (struct: flying/ground, gravity, platform collision)
├── Coin        (struct: CircleShape)
└── Pickup      (base struct)
    ├── GunPickup
    ├── ShotgunPickup
    ├── HealthPotion
    └── ShieldPickup

Game            (owns all systems, state machine, render loop)
GameState       (enum class: MENU, ABOUT, PLAYING, BOSS_FIGHT, GAME_OVER, WIN)
```

### 11.2 Design Patterns Applied

| Pattern | Implementation |
|---------|---------------|
| Game Loop | `Game::run()` with `sf::Clock` delta-time |
| Finite State Machine | `GameState` enum + switch logic in `processEvents`, `update`, `render` |
| Template Method / Polymorphism | `Boss::update()` is pure virtual; `Level1Boss` and `CloudBoss` override it |
| RAII | `std::unique_ptr<Boss>` for automatic boss lifetime management |
| Component Pattern (light) | `Entity` provides a reusable shape + bounds interface |

---

## 12. Out of Scope

- Audio system (sound effects, background music)
- Level 2 with CloudBoss integration
- Animated sprites (sprite sheet frame cycling)
- Controller / gamepad support
- Persistent high score storage
- Pause menu
- Settings screen (resolution, volume)
- Networked multiplayer

---

## 13. Risks & Mitigations

| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|------------|
| Missing asset file causes crash | Medium | High | All loads check return value; fallback color applied |
| Hardcoded absolute path breaks cross-platform build | High | High | All paths changed to relative `../src/` |
| Boss collision desync due to origin offset | Medium | Medium | Boss origin is set at center after texture assignment |
| Memory leak from raw pointer enemies/bullets | Low | Medium | Enemies and bullets stored in `std::vector`; auto-cleaned |
| Frame rate drops on low-end hardware | Low | Low | Fixed frame cap; no physics sub-stepping needed at this scale |

---

## 14. Acceptance Criteria

| # | Criterion | Pass Condition |
|---|-----------|----------------|
| AC-01 | Game launches without crash | Window opens; MENU state renders correctly |
| AC-02 | Player image displays correctly | `player.png` sprite visible on screen (not magenta fallback) |
| AC-03 | Keyboard input responds | S starts game, arrows move player, Space jumps, X shoots |
| AC-04 | Enemy spawning escalates | Enemy speed and frequency visibly increase after 10 s of play |
| AC-05 | Coin collection triggers boss | After 8 coins, boss appears and enemies clear |
| AC-06 | Boss fight is winnable | Dealing 40 damage to boss transitions to WIN state |
| AC-07 | Death triggers GAME_OVER | Player HP = 0 → GAME_OVER screen with score |
| AC-08 | Restart works correctly | Pressing R from GAME_OVER or WIN resets all state and returns to PLAYING |
| AC-09 | Score increases continuously | Score value in HUD grows every second |
| AC-10 | Game runs at 60 FPS | No visible frame drops under normal gameplay conditions |

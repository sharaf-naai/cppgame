# Product Requirements Document
## Emberwood Arena

**Author:** Zidane Nait Cheikh  
**Date:** April 22, 2026  
**Status:** Final

---

## 1. Product Overview

**Emberwood Arena** is a 2D action-platformer developed in C++17 with SFML 3.0.2. The player fights through waves of enemies, collects coins to trigger a boss encounter, and must defeat a giant spider boss to win. The project demonstrates object-oriented design, real-time game loop architecture, AABB collision resolution, and finite state machine management.

---

## 2. Goals & Objectives

| # | Goal | Outcome |
|---|------|---------|
| G-01 | Complete game loop with a clear win condition | Player can reach WIN or GAME_OVER from MENU |
| G-02 | Progressive difficulty via scaling enemy speed and spawn rate | Spawn interval: 2.5 s → 0.8 s; speed scales with survival time |
| G-03 | Boss fight as climax | Spider boss spawns on 8th coin; WIN state on defeat |
| G-04 | Stable 60 FPS | `window.setFramerateLimit(60)` enforced |
| G-05 | Cross-platform build | CMake + SFML FetchContent; relative asset paths |

---

## 3. Scope

### In Scope
- Single-player, single-level gameplay
- Parallax background (3 layers), 6-platform level + boss arena
- Player movement, jumping, bow and shotgun weapons with clip/reload
- Two enemy types: Ghost (flying), Skeleton (ground)
- One boss: Spider (60 HP), with targeted projectile attacks
- Coin collection as the boss trigger (8 coins)
- Four pickups: Bow, Golden Bow (shotgun), Health Potion, Shield
- Full audio: background music + 6 sound effects
- Score system: survival time, kills, coins, boss damage
- Six game states: MENU, ABOUT, PLAYING, BOSS_FIGHT, GAME_OVER, WIN

### Out of Scope
- Multiplayer
- Save/load or high score persistence
- Mouse-driven UI or controller support
- Additional levels or boss types

---

## 4. Technical Requirements

| ID | Requirement | Value |
|----|-------------|-------|
| TR-01 | Language | C++17 |
| TR-02 | Graphics/Audio | SFML 3.0.2 |
| TR-03 | Build System | CMake ≥ 3.28 |
| TR-04 | Window Resolution | 1280 × 720 px (fixed) |
| TR-05 | Frame Rate | 60 FPS cap |
| TR-06 | Compiler | GCC, Clang, or MSVC with C++17 |

---

## 5. Functional Requirements

### 5.1 Game State Machine

| State | Entry | Exit |
|-------|-------|------|
| `MENU` | Launch | `S` → PLAYING; `A` → ABOUT; `Q` → quit |
| `ABOUT` | From MENU | `B` / `Esc` → MENU |
| `PLAYING` | `S` from MENU or restart | 8 coins → BOSS_FIGHT; HP = 0 → GAME_OVER |
| `BOSS_FIGHT` | 8 coins collected | Boss HP = 0 → WIN; Player HP = 0 → GAME_OVER |
| `GAME_OVER` | Player HP = 0 | `R` → restart PLAYING |
| `WIN` | Boss defeated | `R` → restart PLAYING |

### 5.2 Player

| ID | Requirement |
|----|-------------|
| FR-P01 | 16 HP max (8 full hearts displayed) |
| FR-P02 | Horizontal movement: ← / → at 350 px/s |
| FR-P03 | Jump: Space, force = −820 px/s; gravity = 1400 px/s² |
| FR-P04 | Sprite flips to match facing direction |
| FR-P05 | 1 second of invincibility frames after taking damage |
| FR-P06 | Clamped to screen bounds [0, 1280] |
| FR-P07 | Shoot with `X`; `C` for manual reload |
| FR-P08 | Bow: 6-bullet clip, 0.3 s cooldown; Shotgun: 3-bullet clip, 0.8 s, 3-way spread |
| FR-P09 | Auto-reloads from reserve when clip is empty |

### 5.3 Enemies

| ID | Requirement |
|----|-------------|
| FR-E01 | Spawn off-screen right (x = 1300) |
| FR-E02 | Ghost: flying, size 200×200; Skeleton: ground, size 250×250 |
| FR-E03 | Base speed: 150 px/s + time scaling + random 0–200 px/s |
| FR-E04 | Spawn interval: 2.5 s before 10 s; then decreases 0.1 s per 5 s interval, floor 0.8 s |
| FR-E05 | Contact deals 3 damage to player |
| FR-E06 | Arrow hit triggers death animation; awards +250 score and +1 kill |
| FR-E07 | Enemies exiting left boundary (x + width < 0) are removed |

### 5.4 Coins & Boss Trigger

| ID | Requirement |
|----|-------------|
| FR-C01 | One coin spawns every 5 s on a random platform if none present |
| FR-C02 | Collecting a coin: +500 score, +1 coin counter |
| FR-C03 | 8th coin triggers BOSS_FIGHT: enemies cleared, Spider boss spawns at x=1100 |

### 5.5 Boss (Spider)

| ID | Requirement |
|----|-------------|
| FR-B01 | 60 HP; walks toward player if distance > 350 px; idles within range |
| FR-B02 | Attacks every 2 s: fires a targeted bullet aimed at player center |
| FR-B03 | Hit animation interrupts attack; 0.3 s stagger |
| FR-B04 | Arrow hit = 1 damage; shotgun = 1–4 damage (distance-based falloff from origin) |
| FR-B05 | Defeat awards 5,000 + max(0, 15,000 − survivalTime × 150) bonus |

### 5.6 Pickups

| Pickup | Spawn Condition | Effect | Cooldown |
|--------|----------------|--------|----------|
| Bow | 15–20 s after level start | 30 arrows, clip auto-reloads | Consumed |
| Health Potion | HP ≤ 10, cooldown expired | +6 HP | 15 s |
| Shield | BOSS_FIGHT, every 20 s | 3 s full immunity + visual aura | 20 s |
| Golden Bow | BOSS_FIGHT, ammo = 0, no shotgun, after 6 s | Shotgun with 15 shells | Consumed |

### 5.7 Score System

| Source | Value |
|--------|-------|
| Survival | +120 pts/s |
| Enemy kill | +250 pts |
| Coin collected | +500 pts |
| Boss hit | +100 × damage |
| Boss defeated | +5,000 + time bonus (up to +15,000) |

---

## 6. Non-Functional Requirements

| ID | Category | Requirement |
|----|----------|-------------|
| NFR-01 | Performance | Stable 60 FPS under normal play conditions |
| NFR-02 | Memory Safety | `std::unique_ptr` for boss; STL vectors for dynamic objects; no raw new/delete |
| NFR-03 | Robustness | Failed asset loads emit `std::cerr` warning; game does not crash |
| NFR-04 | Portability | All asset paths relative to executable; no hardcoded absolute paths |
| NFR-05 | Modularity | Responsibilities split: AssetManager, AudioManager, UIManager, LevelManager |

---

## 7. Level Design

### Standard Level (PLAYING)

| # | X | Y | Width | Height |
|---|---|---|-------|--------|
| Ground | −100 | 670 | 2000 | 50 |
| Platform 1 | 150 | 480 | 200 | 50 |
| Platform 2 | 450 | 400 | 250 | 50 |
| Platform 3 | 850 | 450 | 180 | 50 |
| Platform 4 | 1050 | 250 | 200 | 50 |
| Platform 5 | 300 | 200 | 150 | 50 |

### Boss Arena (BOSS_FIGHT)
Single ground platform spanning the full level width. No floating platforms.

---

## 8. Architecture Overview

```
Entity              (base: shape, getBounds, draw)
├── Bullet          (velocity, animation, shotgun flag)
└── Boss            (spider AI, targeted projectile)

Player              (movement, weapons, health, animation)
Enemy               (flying/ground, sprite animation, hitbox shrink)
Coin, GunPickup, ShotgunPickup, HealthPotion, ShieldPickup  (structs)

Game                (state machine, update loop, render, collision)
AssetManager        (texture/font loading and lookup)
AudioManager        (sound buffer/playback, background music)
UIManager           (HUD, menus, overlays)
LevelManager        (backgrounds, platforms, spawn lanes)
GameState           (enum: MENU, ABOUT, PLAYING, BOSS_FIGHT, GAME_OVER, WIN)
```

---

## 9. Acceptance Criteria

| # | Criterion | Pass |
|---|-----------|------|
| AC-01 | Game launches; MENU renders | Window opens without crash |
| AC-02 | Player moves and jumps | Arrow keys and Space respond correctly |
| AC-03 | Enemies spawn and scale | Speed/frequency increases after 10 s |
| AC-04 | 8 coins trigger boss | Boss appears; enemies clear |
| AC-05 | Boss is defeatable | 60 damage → WIN state |
| AC-06 | Death triggers GAME_OVER | HP = 0 → death animation → GAME_OVER screen |
| AC-07 | Restart resets all state | `R` from GAME_OVER/WIN returns to fresh PLAYING |
| AC-08 | Score accumulates | HUD score increases every second |
| AC-09 | Audio plays | Background music and sound effects audible |
| AC-10 | Stable 60 FPS | No visible drops under normal gameplay |

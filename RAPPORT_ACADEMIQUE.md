# Rapport Académique
## Conception et Développement d'un Jeu de Plateforme 2D en C++ avec SFML

---

**Auteur :** Zidane Nait Cheikh  
**Date :** 20 Avril 2026  
**Langage :** C++17  
**Bibliothèque :** SFML 3.0.2  
**Système de build :** CMake 3.28  

---

## Table des Matières

1. [Introduction](#1-introduction)
2. [Contexte et Motivation](#2-contexte-et-motivation)
3. [Architecture du Projet](#3-architecture-du-projet)
4. [Principes de Programmation Orientée Objet](#4-principes-de-programmation-orientée-objet)
5. [La Boucle de Jeu et le Modèle Physique](#5-la-boucle-de-jeu-et-le-modèle-physique)
6. [Système de Gestion des États (State Machine)](#6-système-de-gestion-des-états-state-machine)
7. [Systèmes de Jeu Détaillés](#7-systèmes-de-jeu-détaillés)
8. [Gestion de la Mémoire et des Ressources](#8-gestion-de-la-mémoire-et-des-ressources)
9. [Rendu Graphique et Interface Utilisateur](#9-rendu-graphique-et-interface-utilisateur)
10. [Difficultés Rencontrées et Solutions Apportées](#10-difficultés-rencontrées-et-solutions-apportées)
11. [Résultats et Tests](#11-résultats-et-tests)
12. [Perspectives et Améliorations Futures](#12-perspectives-et-améliorations-futures)
13. [Conclusion](#13-conclusion)
14. [Bibliographie](#14-bibliographie)

---

## 1. Introduction

Ce rapport présente la conception et le développement d'un jeu vidéo de plateforme 2D intitulé **Forest Platformer — Classic Mode**, réalisé en langage **C++17** à l'aide de la bibliothèque graphique **SFML 3.0.2** (Simple and Fast Multimedia Library). Le projet illustre l'application concrète de concepts fondamentaux de l'informatique : la programmation orientée objet (POO), la gestion de la mémoire, les structures de données dynamiques, la détection de collisions en temps réel, et la conception de systèmes interactifs pilotés par événements.

Le jeu propose une expérience de plateforme classique : le joueur évolue dans un environnement forestier, affronte des vagues d'ennemis à difficulté progressive, collecte des pièces pour déclencher un combat de boss, et doit vaincre ce dernier pour remporter la victoire. L'ensemble du cycle de jeu — du menu principal à l'écran de victoire — est entièrement fonctionnel et géré par une machine à états finis.

---

## 2. Contexte et Motivation

### 2.1 Choix du domaine

Le développement de jeux vidéo est un terrain d'application particulièrement riche pour illustrer les concepts avancés de programmation. Il mobilise simultanément :

- La **conception logicielle** (architecture, design patterns, modularité)
- La **programmation système** (boucle temps-réel, gestion des événements)
- La **mathématique appliquée** (physique, vecteurs, trigonométrie)
- La **gestion des ressources** (textures, polices, mémoire dynamique)

### 2.2 Choix technologiques

**C++17** a été retenu pour ses performances natives, sa compatibilité avec les bibliothèques multimédia et la richesse des fonctionnalités modernes qu'il apporte (initialisation de liste, `std::optional`, `std::unique_ptr`, structured bindings).

**SFML 3.0.2** a été choisi pour les raisons suivantes :
- API simple et claire, adaptée à l'apprentissage
- Gestion native des fenêtres, événements, sprites, textures et shapes
- Compatibilité cross-platform (Windows, macOS, Linux)
- Intégration CMake via FetchContent (aucune installation manuelle requise)

**CMake 3.28** assure la portabilité du système de build et automatise le téléchargement de SFML à la compilation.

---

## 3. Architecture du Projet

### 3.1 Structure des fichiers

```
cppgame/
├── CMakeLists.txt          # Configuration du build
├── src/
│   ├── main.cpp            # Point d'entrée (instancie Game, appelle run())
│   ├── GameState.h         # Énumération des états de jeu
│   ├── Entity.h            # Classe de base abstraite pour les entités
│   ├── WorldObjects.h      # Bullet, Enemy, Coin, Pickups
│   ├── Player.h / .cpp     # Logique complète du joueur
│   ├── Boss.h / .cpp       # Hiérarchie Boss (abstrait + Level1Boss + CloudBoss)
│   ├── Game.h / .cpp       # Orchestrateur principal (boucle, états, rendu)
│   └── [assets/]           # Textures PNG et police TTF
```

### 3.2 Hiérarchie des classes

La figure suivante représente la hiérarchie complète des classes du projet :

```
                    ┌──────────────┐
                    │    Entity    │  (classe de base)
                    │  + shape     │  sf::RectangleShape
                    │  + getBounds │
                    │  + getPos    │
                    │  + draw()    │
                    └──────┬───────┘
                           │ hérite
              ┌────────────┼────────────┐
              │            │            │
         ┌────▼───┐  ┌─────▼────┐  ┌───▼───┐
         │ Player │  │   Boss   │  │Bullet │
         │        │  │(abstrait)│  │       │
         └────────┘  └─────┬────┘  └───────┘
                           │ hérite
                  ┌─────────┴──────────┐
                  │                    │
           ┌──────▼─────┐    ┌────────▼────┐
           │ Level1Boss │    │  CloudBoss  │
           └────────────┘    └─────────────┘

         Structs indépendants (WorldObjects.h) :
         Enemy, Coin, Pickup, GunPickup,
         ShotgunPickup, HealthPotion, ShieldPickup
```

### 3.3 Responsabilités par module

| Module | Responsabilité principale |
|--------|--------------------------|
| `main.cpp` | Point d'entrée, instanciation de `Game` |
| `Game` | Propriétaire de toutes les entités, orchestrateur de la boucle de jeu, du rendu et des états |
| `Player` | Mouvement, physique, tir, santé, invincibilité, bouclier |
| `Boss` / `Level1Boss` | Intelligence artificielle du boss, comportement à phases |
| `Entity` | Interface partagée (forme, limites, position) |
| `WorldObjects.h` | Structures légères pour les objets du monde (balles, ennemis, pièces, ramassables) |
| `GameState.h` | Définition de l'énumération des états |

---

## 4. Principes de Programmation Orientée Objet

### 4.1 Encapsulation

La classe `Player` encapsule l'intégralité de l'état du joueur dans ses membres privés : santé, vélocité, direction, état du bouclier, état de rechargement, etc. L'accès externe se fait exclusivement via des méthodes publiques (`getHealth()`, `getCenter()`, `takeDamage()`, `heal()`, etc.), respectant ainsi le principe d'encapsulation.

```cpp
// Player.h — membres privés, accès contrôlé
private:
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Vector2f velocity;
    float speed, jumpForce, gravity, floorY;
    bool isGrounded, isInvincible, facingRight, hasGun, hasShotgun;
    int health, ammo, clipAmmo, coins;
    // ...
public:
    int getHealth() const { return health; }
    void takeDamage(int amount);
    void heal(int amount);
```

### 4.2 Héritage

La classe `Boss` est une **classe abstraite** qui définit l'interface commune à tous les boss (santé, vitesse, méthode `update` pure virtuelle). `Level1Boss` et `CloudBoss` en héritent et implémentent chacun leur propre comportement.

```cpp
// Boss.h
class Boss : public Entity {
public:
    virtual void update(float dt, std::vector<Bullet>& bossBullets,
                        float floorY, sf::Vector2f playerCenter) = 0; // Pure virtuelle
};

class Level1Boss : public Boss {
public:
    void update(float dt, std::vector<Bullet>& bossBullets,
                float floorY, sf::Vector2f playerCenter) override;
};
```

Cela permet d'ajouter de nouveaux boss sans modifier la classe `Game`, conformément au **Principe Ouvert/Fermé** (Open/Closed Principle).

### 4.3 Polymorphisme

Le pointeur `std::unique_ptr<Boss>` dans `Game` est de type `Boss*` mais pointe vers une instance de `Level1Boss` au moment de l'exécution. L'appel `boss->update(...)` déclenche automatiquement la bonne implémentation via la table virtuelle (vtable) de C++.

```cpp
// Game.h
std::unique_ptr<Boss> boss;

// Game.cpp — instanciation polymorphique
boss = std::make_unique<Level1Boss>(1100.f, 250.f);

// Appel polymorphique lors du rendu/mise à jour
boss->update(dt, bossBullets, player.getFloorY(), player.getCenter());
```

### 4.4 Abstraction

La classe `Entity` fournit une interface abstraite unifiée. N'importe quel système peut interagir avec une entité via `getBounds()`, `getPosition()`, `getSize()` et `draw()` sans connaître le type concret sous-jacent.

---

## 5. La Boucle de Jeu et le Modèle Physique

### 5.1 Architecture de la boucle de jeu

La boucle de jeu suit le patron classique **Processus → Mise à jour → Rendu** :

```
┌────────────────────────────────────┐
│            Game::run()             │
│                                    │
│  ┌──────────────────────────────┐  │
│  │    Démarrer sf::Clock (dt)   │  │
│  └──────────────┬───────────────┘  │
│                 │                  │
│  ┌──────────────▼───────────────┐  │
│  │  processEvents()             │  │  ← Événements clavier/fenêtre
│  └──────────────┬───────────────┘  │
│                 │                  │
│  ┌──────────────▼───────────────┐  │
│  │  update(dt)                  │  │  ← Physique, IA, collisions
│  │  (si PLAYING ou BOSS_FIGHT)  │  │
│  └──────────────┬───────────────┘  │
│                 │                  │
│  ┌──────────────▼───────────────┐  │
│  │  render()                    │  │  ← Dessin de toutes les entités
│  └──────────────┬───────────────┘  │
│                 │                  │
│  └─────── Répéter à 60 FPS ───────┘  │
└────────────────────────────────────┘
```

Le **delta-time** (`dt`), calculé avec `sf::Clock::restart().asSeconds()`, est passé à toutes les fonctions de mise à jour. Cela découple la logique de jeu de la fréquence d'images et garantit un comportement cohérent indépendamment des performances matérielles.

### 5.2 Modèle physique

Le moteur physique est un **intégrateur d'Euler explicite** implémenté manuellement. Pour chaque frame :

**Mouvement horizontal :**
```
position.x += velocity.x × dt
```

**Gravité et mouvement vertical :**
```
velocity.y += gravity × dt     (gravité = 1500 px/s²)
position.y += velocity.y × dt
```

**Saut :**
```
si (Espace pressé ET isGrounded) alors velocity.y = −900 px/s
```

### 5.3 Détection et résolution des collisions

La détection de collision utilise des **Boîtes Englobantes Alignées sur les Axes** (AABB — Axis-Aligned Bounding Box), via la méthode SFML `findIntersection()`.

La résolution se fait en deux passes séparées pour éviter les artéfacts de coin :

1. **Passe horizontale :** déplacer, détecter, corriger la position X
2. **Passe verticale :** déplacer, détecter, corriger la position Y (et activer `isGrounded`)

```cpp
// Passe horizontale (extrait simplifié)
shape.move({velocity.x * dt, 0.f});
for (const auto& platform : platforms) {
    if (shape.getGlobalBounds().findIntersection(platform.getGlobalBounds())) {
        if (velocity.x > 0.f)
            shape.setPosition({platform.getPosition().x - shape.getSize().x, shape.getPosition().y});
        else
            shape.setPosition({platform.getPosition().x + platform.getSize().x, shape.getPosition().y});
        velocity.x = 0.f;
    }
}
```

---

## 6. Système de Gestion des États (State Machine)

Le jeu est entièrement piloté par une **machine à états finis** (FSM — Finite State Machine) implémentée via l'énumération `GameState`.

### 6.1 Diagramme de transitions

```
               ┌──────────────────────────────────────────┐
               │                 [Q]                       │
               ▼                  │                        │
           ┌────────┐    [S]   ┌──────────┐               │
  Start ──▶│  MENU  ├─────────▶│ PLAYING  │               │
           └───┬────┘          └───┬──┬───┘               │
               │[A]                │  │                    │
               ▼                   │  │HP=0                │
           ┌───────┐    [B/Esc]    │  ▼                    │
           │ ABOUT │──────────────▶│ GAME_OVER ──[R]──────▶┤
           └───────┘            8  │                       │
                               coins                       │
                                │  ▼                       │
                           ┌───▼──────────┐                │
                           │ BOSS_FIGHT   │                │
                           └──┬────┬──────┘               │
                              │    │                       │
                     Boss HP=0│    │HP=0                   │
                              ▼    ▼                       │
                            WIN  GAME_OVER ──[R]──────────▶┘
```

### 6.2 Séparation des responsabilités par état

Chaque méthode du `Game` comporte une logique conditionnelle basée sur `gameState` :

- `processEvents()` : gère les entrées clavier selon l'état actif
- `update()` : ne s'exécute que dans `PLAYING` et `BOSS_FIGHT`
- `render()` : affiche le HUD complet dans les états de jeu, et les menus dans `MENU`/`ABOUT`

Cette séparation garantit qu'aucune logique de jeu ne s'exécute involontairement pendant les écrans de menu ou de fin de partie.

---

## 7. Systèmes de Jeu Détaillés

### 7.1 Système de Score

Le score est un flottant (`currentScore`) qui s'accumule en temps réel selon plusieurs sources :

| Source | Valeur |
|--------|--------|
| Survie | +120 points/seconde |
| Ennemi éliminé | +250 points |
| Pièce collectée | +500 points |
| Coup sur le boss | +100 × dégâts infligés |
| Boss vaincu | +5 000 + bonus temps (jusqu'à +15 000) |

La formule du bonus de temps récompense la rapidité :
```
bonus = max(0, 15 000 − survivalTime × 150)
```

### 7.2 Système d'Ennemis et Difficulté Progressive

Les ennemis apparaissent depuis le bord droit de l'écran (`x = 1300`) à un intervalle contrôlé par `enemySpawnClock`. Le taux d'apparition évolue dynamiquement :

```
si survivalTime < 10s → intervalle = 2.5s
sinon → intervalle = max(0.8, 2.0 − (((survivalTime − 10) / 5) × 0.1))
```

La vitesse des ennemis augmente également :
```
speed = 150 + ((survivalTime - 10) / 5 × 40) + random(0, 200)
```

Trois types d'ennemis offrent une variété de comportements :

| Ennemi | Type | Comportement |
|--------|------|-------------|
| Fantôme (Ghost) | Volant | Se déplace horizontalement en ligne droite, spawn sur les plateformes en hauteur |
| Squelette (Skeleton) | Sol | Soumis à la gravité, spawn au sol |
| Araignée (Spider) | Sol | Identique au squelette, texture distincte |

### 7.3 Intelligence Artificielle du Boss (Level1Boss)

Le `Level1Boss` implémente une IA à deux phases contrôlée par une variable d'état entière :

**Phase 0 — Attaque (state = 0) :**
- Déplacement vertical continu, rebondissant entre `y = 50` et le sol
- Tir d'une salve de 3 projectiles dirigés vers le joueur toutes les 1.2 secondes
- Utilisation de la trigonométrie pour l'angle :
  ```cpp
  float angle = std::atan2(playerCenter.y - bossCenter.y,
                           playerCenter.x - bossCenter.x);
  float spreads[3] = {-0.1f, 0.0f, 0.1f};
  // 3 balles avec dispersion angulaire
  ```
- Après 6 salves → transition vers la Phase 1

**Phase 1 — Repositionnement (state = 1) :**
- Pause de 4 secondes
- Téléportation à une nouvelle position X (gauche ou droite selon la position courante)
- Retour à la Phase 0

Ce cycle crée un rythme de combat lisible et prévisible pour le joueur, tout en maintenant une pression constante.

### 7.4 Système de Ramassables (Pickups)

Le système de ramassables suit un patron commun : chaque pickup possède un flag `active` et un `cooldownTimer`. La détection de collecte utilise `findIntersection()` entre les limites du joueur et celles du pickup.

```
Santé ≤ 4  AND  cooldown = 0  →  spawn Health Potion
                                    Contact joueur  →  +4 HP, cooldown = 15s

Temps ≥ targetGunSpawnTime  →  spawn Bow
                                    Contact joueur  →  15 flèches

BOSS_FIGHT AND ammo = 0  →  countdown 6s  →  spawn Golden Bow
                                    Contact joueur  →  15 munitions shotgun

BOSS_FIGHT  →  cooldown 20s  →  spawn Shield
                                    Contact joueur  →  immunité 3s
```

### 7.5 Système de Projectiles

Les projectiles (`Bullet`) sont stockés dans des `std::vector<Bullet>` séparés pour le joueur et le boss. Chaque frame :

1. La texture est appliquée si absent (lazy assignment)
2. La position est mise à jour : `position += velocity × dt`
3. Une vérification de collision est effectuée contre les cibles
4. Les projectiles hors-écran ou ayant touché leur cible sont effacés via `erase`

Le dégât du shotgun contre le boss implémente une **atténuation par distance** :
```cpp
int damage = std::max(1, 4 - static_cast<int>(
    std::abs(boss->getCenter().x - bullet.originX) / 150.f
));
```

---

## 8. Gestion de la Mémoire et des Ressources

### 8.1 RAII et Pointeurs Intelligents

La propriété du boss est gérée par `std::unique_ptr<Boss>`, garantissant la libération automatique de la mémoire dès que le boss est détruit ou remplacé :

```cpp
// Instanciation propre sans new/delete
boss = std::make_unique<Level1Boss>(1100.f, 250.f);

// À la fin du combat, boss.reset() libère automatiquement la mémoire
boss.reset();
```

### 8.2 Conteneurs STL

Tous les objets dynamiques (ennemis, balles, pièces) sont stockés dans des `std::vector`. L'effacement se fait via l'idiome erase-iterator :

```cpp
for (auto it = enemies.begin(); it != enemies.end(); ) {
    it->update(dt, platforms);
    if (condition_suppression) it = enemies.erase(it);
    else ++it;
}
```

Cet idiome garantit l'invalidation correcte des itérateurs et évite les accès mémoire invalides.

### 8.3 Durée de Vie des Textures

SFML exige que les textures aient une durée de vie au moins égale à leurs sprites. Dans ce projet, toutes les textures sont déclarées comme membres de `Game` ou de `Player`, garantissant qu'elles restent valides pendant toute la durée de vie de leurs sprites associés.

```cpp
// Game.h — textures et sprites déclarés ensemble
sf::Texture playerHeartTex;    // ← durée de vie garantie
sf::Sprite  playerHeartSprite; // ← référence texture valide

// Initialisation correcte dans le constructeur
playerHeartSprite.setTexture(playerHeartTex, true);
```

---

## 9. Rendu Graphique et Interface Utilisateur

### 9.1 Parallaxe et Fond

Le fond est composé de trois couches superposées (`back.png`, `far.png`, `middle.png`), chacune mise à l'échelle pour couvrir exactement la hauteur de la fenêtre (720 px) :

```cpp
float scale = 720.f / static_cast<float>(backTexture.getSize().y);
backSprite.setScale({scale, scale});
backSprite.setTextureRect(sf::IntRect({0, 0},
    {static_cast<int>(1280.f / scale), static_cast<int>(backTexture.getSize().y)}));
```

La texture est définie comme répétée (`setRepeated(true)`) pour permettre un éventuel scrolling futur.

### 9.2 Ordre de Rendu (Painter's Algorithm)

Le rendu suit l'algorithme du peintre (couches arrière vers couches avant) :

```
1. Fond (back → far → middle)
2. Plateformes
3. Ennemis
4. Pièces
5. Projectiles (joueur et boss)
6. Ramassables
7. Boss
8. Joueur
9. HUD (score, cœurs, munitions)
10. Écrans de superposition (GAME_OVER, WIN)
```

### 9.3 Retour Visuel

Le joueur reçoit un retour visuel immédiat sur les événements :

- **Dommages :** clignotement par alternance d'opacité (255 ↔ 100) toutes les 0.2 secondes pendant 1.2 secondes
- **Bouclier actif :** aura circulaire cyan dessinée autour du joueur
- **Direction du boss :** retournement du sprite via scale x négatif (`setScale({-1.f, 1.f})`)

---

## 10. Difficultés Rencontrées et Solutions Apportées

### 10.1 Problème : Chemin de fichier codé en dur (Portabilité)

**Problème :** La texture du joueur était chargée depuis un chemin absolu Windows (`C:/Users/chara/Desktop/...`), rendant le jeu non fonctionnel sur macOS.

**Solution :** Alignement avec la convention déjà appliquée dans `Game.cpp` — utilisation d'un chemin relatif `../src/player.png` à partir du répertoire de build CMake.

```cpp
// Avant (non portable)
texture.loadFromFile("C:/Users/chara/Desktop/.../src/player.png");

// Après (cross-platform)
texture.loadFromFile("../src/player.png");
```

**Leçon retenue :** Les chemins de fichiers doivent toujours être relatifs dans un projet multi-plateforme.

### 10.2 Problème : Collision de traversée de plateforme

**Problème :** Les ennemis au sol pouvaient traverser les plateformes lors de chutes à grande vitesse (tunnelling).

**Solution :** Augmentation de la tolérance de détection verticale à 30 pixels, garantissant que la correction de position est appliquée même si l'objet a partiellement traversé la surface dans la même frame.

```cpp
if (velocityY >= 0.f &&
    shape.getPosition().y + shape.getSize().y - (velocityY * dt) <= plat.getPosition().y + 30.f) {
    shape.setPosition({shape.getPosition().x, plat.getPosition().y - shape.getSize().y});
    velocityY = 0.f;
}
```

### 10.3 Problème : Durée de vie des textures SFML

**Problème :** En SFML, un `sf::Sprite` contient un pointeur vers sa texture. Si la texture est détruite ou déplacée, le sprite affiche des artéfacts.

**Solution :** Toutes les textures sont déclarées comme membres de `Game` ou de `Player` (et non comme variables locales), garantissant leur durée de vie par construction.

### 10.4 Problème : Flipping du sprite du boss avec offset d'origine

**Problème :** Retourner le sprite du boss via un scale X négatif provoquait un déplacement visible de la hitbox si l'origine n'était pas au centre.

**Solution :** L'origine du boss est explicitement définie au centre du sprite après l'assignation de la texture :

```cpp
sf::Vector2f bSize = boss->shape.getSize();
boss->shape.setOrigin({bSize.x / 2.f, bSize.y / 2.f});
boss->shape.move({bSize.x / 2.f, bSize.y / 2.f});
```

### 10.5 Problème : Invalidation d'itérateurs dans les boucles de suppression

**Problème :** Utiliser une boucle `for (auto& e : enemies)` avec `enemies.erase()` à l'intérieur est un comportement indéfini en C++.

**Solution :** Utilisation systématique du patron `while (it != container.end())` avec `it = container.erase(it)` lors d'une suppression, ou `++it` sinon.

---

## 11. Résultats et Tests

### 11.1 Tests fonctionnels effectués

| Scénario | Résultat Attendu | Résultat Obtenu |
|----------|-----------------|-----------------|
| Lancement du jeu | Menu principal affiché | ✓ |
| Pression de S | Démarrage de la partie | ✓ |
| Déplacement gauche/droite | Joueur se déplace, sprite se retourne | ✓ |
| Saut (Espace) | Joueur saute, gravité appliquée | ✓ |
| Chute depuis une plateforme | Joueur tombe et atterrit au sol | ✓ |
| Tir (X) | Flèche projetée dans la direction regardée | ✓ |
| Tir en hauteur (X + Haut) | Flèche projetée vers le haut | ✓ |
| Contact avec un ennemi | −2 HP, clignotement d'invincibilité | ✓ |
| Collecte d'une pièce | Compteur +1, score +500 | ✓ |
| 8 pièces collectées | Boss apparaît, ennemis effacés | ✓ |
| Boss à 0 HP | Écran de victoire avec score final | ✓ |
| Joueur à 0 HP | Écran GAME_OVER avec score | ✓ |
| Pression de R | Redémarrage complet de la partie | ✓ |
| Collecte Health Potion | +4 HP (plafonné à 10) | ✓ |
| Collecte Shield | Aura cyan, immunité 3 secondes | ✓ |

### 11.2 Performance

Le jeu maintient 60 FPS stables sur une machine standard avec `window.setFramerateLimit(60)`. La complexité algorithmique par frame est linéaire en nombre d'entités actives — acceptable pour les tailles de collection observées (< 20 ennemis simultanés, < 30 projectiles).

### 11.3 Stabilité mémoire

L'utilisation de `std::vector` avec suppression par itérateur et `std::unique_ptr<Boss>` garantit l'absence de fuites mémoire sur les chemins principaux de l'application.

---

## 12. Perspectives et Améliorations Futures

### 12.1 Court terme

| Amélioration | Description |
|-------------|-------------|
| **Audio** | Intégrer `SFML::Audio` pour les effets sonores (tir, saut, impact) et une musique de fond |
| **Animations** | Implémenter un système de spritesheet avec défilement d'images pour le joueur et les ennemis |
| **Scrolling du fond** | Utiliser le delta-time pour faire défiler les couches de parallaxe en temps réel |
| **Niveau 2** | Intégrer `CloudBoss` déjà implémenté, avec une transition après la victoire sur Level1Boss |

### 12.2 Moyen terme

| Amélioration | Description |
|-------------|-------------|
| **Meilleur score persistant** | Stocker le meilleur score dans un fichier texte via `std::fstream` |
| **Écran de pause** | Ajouter un état `PAUSED` avec reprise de la partie |
| **Types d'ennemis additionnels** | Ennemis avec patterns de déplacement plus complexes (sinusoïdal, charge) |
| **Système de particules** | Effets visuels à l'impact des projectiles |

### 12.3 Long terme

| Amélioration | Architecture requise |
|-------------|---------------------|
| **Éditeur de niveaux** | Système de chargement depuis un fichier JSON ou XML |
| **Multijoueur local** | Support 2 joueurs sur le même clavier |
| **Système de composants (ECS)** | Refactorisation vers une architecture Entity-Component-System pour une meilleure scalabilité |

---

## 13. Conclusion

Ce projet démontre qu'il est possible de développer un jeu vidéo complet et fonctionnel en C++ en appliquant rigoureusement les principes de la programmation orientée objet. L'utilisation de l'héritage pour la hiérarchie des boss, du polymorphisme pour l'appel unifié de `update()`, de l'encapsulation pour l'état du joueur, et des conteneurs STL pour la gestion dynamique des entités, aboutit à une base de code modulaire et extensible.

Le projet illustre également des problématiques concrètes de développement logiciel : portabilité des chemins de fichiers, gestion de la durée de vie des ressources, résolution de collisions AABB, et organisation d'une boucle temps-réel. La machine à états finis assure une séparation claire des responsabilités entre les différents modes de jeu.

En termes de résultats, le jeu est entièrement jouable : le joueur peut démarrer une partie, survivre à des vagues d'ennemis d'intensité croissante, collecter 8 pièces pour déclencher le combat de boss, vaincre celui-ci, et obtenir un score final. L'expérience est cohérente et les systèmes communiquent correctement entre eux.

Ce travail constitue une base solide pour des développements futurs — notamment l'intégration du second boss `CloudBoss` déjà implémenté, l'ajout d'audio, d'animations, et d'un second niveau — tout en restant accessible à la relecture et à la modification grâce à son architecture claire.

---

## 14. Bibliographie

1. **SFML Documentation officielle** — *SFML 3.0 API Reference*, https://www.sfml-dev.org/documentation/3.0/
2. **Stroustrup, B.** (2013). *The C++ Programming Language*, 4th Edition. Addison-Wesley.
3. **Meyers, S.** (2014). *Effective Modern C++*. O'Reilly Media.
4. **Nystrom, R.** (2014). *Game Programming Patterns*. Genever Benning. Disponible en ligne : https://gameprogrammingpatterns.com/
5. **Gregory, J.** (2018). *Game Engine Architecture*, 3rd Edition. CRC Press.
6. **CMake Documentation** — *CMake 3.28 FetchContent*, https://cmake.org/cmake/help/v3.28/module/FetchContent.html
7. **cppreference.com** — *C++ Standard Library Reference*, https://en.cppreference.com/
8. **Lafebvre, L.** (2020). *SFML Game Development By Example*. Packt Publishing.

---

*Document rédigé dans le cadre d'un projet académique de développement de jeu vidéo en C++.*
*Tous les extraits de code sont tirés directement du code source du projet.*

# Rapport Académique
## Conception et Développement d'un Jeu de Plateforme 2D en C++ avec SFML

**Auteur :** Zidane Nait Cheikh  
**Date :** 22 Avril 2026  
**Langage :** C++17 | **Bibliothèque :** SFML 3.0.2 | **Build :** CMake 3.28

---

## Table des Matières

1. [Introduction](#1-introduction)
2. [Contexte et Choix Technologiques](#2-contexte-et-choix-technologiques)
3. [Architecture du Projet](#3-architecture-du-projet)
4. [Principes POO Appliqués](#4-principes-poo-appliqués)
5. [Boucle de Jeu et Physique](#5-boucle-de-jeu-et-physique)
6. [Machine à États Finis](#6-machine-à-états-finis)
7. [Systèmes de Jeu](#7-systèmes-de-jeu)
8. [Gestion des Ressources et de la Mémoire](#8-gestion-des-ressources-et-de-la-mémoire)
9. [Rendu et Interface Utilisateur](#9-rendu-et-interface-utilisateur)
10. [Difficultés et Solutions](#10-difficultés-et-solutions)
11. [Résultats et Tests](#11-résultats-et-tests)
12. [Conclusion](#12-conclusion)
13. [Bibliographie](#13-bibliographie)

---

## 1. Introduction

Ce rapport présente le développement du jeu **Emberwood Arena**, un jeu de plateforme 2D réalisé en **C++17** avec la bibliothèque **SFML 3.0.2**. Le joueur combat des vagues d'ennemis dans un décor forestier, collecte des pièces pour déclencher un combat contre un boss araignée, et doit le vaincre pour gagner. Le projet illustre concrètement les concepts fondamentaux de la programmation orientée objet, la gestion de la mémoire, la détection de collisions AABB, et la conception d'une boucle de jeu temps-réel.

---

## 2. Contexte et Choix Technologiques

Le développement de jeu vidéo est un domaine d'application idéal pour la programmation avancée : il mobilise simultanément la physique (vecteurs, intégration numérique), la gestion d'état, le rendu graphique et la gestion des ressources.

**C++17** a été retenu pour ses performances natives et ses fonctionnalités modernes (`std::optional`, `std::unique_ptr`, structured bindings, `if constexpr`).

**SFML 3.0.2** a été choisi pour :
- Une API claire adaptée à l'apprentissage (fenêtres, événements, textures, audio)
- Compatibilité Windows et macOS
- Intégration CMake via `FetchContent` (aucune installation manuelle)

---

## 3. Architecture du Projet

### 3.1 Structure des fichiers

```
cppgame/
├── CMakeLists.txt
├── Assets/              # Textures PNG, police TTF, fichiers audio WAV
├── Headers/
│   ├── Entity.h         # Classe de base (shape, getBounds, draw)
│   ├── WorldObjects.h   # Bullet, Enemy, Coin, Pickups
│   ├── Player.h/cpp     # Logique du joueur
│   ├── Boss.h/cpp       # Boss araignée (IA, projectiles)
│   ├── Game.h/cpp       # Orchestrateur principal
│   ├── AssetManager.h   # Chargement textures/police
│   ├── AudioManager.h   # Sons et musique
│   ├── UIManager.h      # HUD et menus
│   ├── LevelManager.h   # Backgrounds, plateformes
│   └── GameState.h      # Énumération des états
└── src/
    └── [.cpp correspondants]
```

### 3.2 Hiérarchie des classes

```
Entity
├── Bullet      (vitesse, animation, flag shotgun)
└── Boss        (IA araignée, tir ciblé, animation)

Player          (mouvement, armes, santé, animation)
Enemy           (fantôme volant / squelette au sol)
Coin, GunPickup, ShotgunPickup, HealthPotion, ShieldPickup

Game            (état, boucle, collisions, rendu)
AssetManager    (map<string, sf::Texture>)
AudioManager    (map<string, sf::Sound> + sf::Music)
UIManager       (sf::Text, sf::Sprite pour HUD)
LevelManager    (sf::Sprite backgrounds, vector<RectangleShape> plateformes)
```

### 3.3 Répartition des responsabilités

| Classe | Responsabilité |
|--------|---------------|
| `Game` | Propriétaire de toutes les entités ; orchestre la boucle, les états et le rendu |
| `Player` | Mouvement, physique, tir, rechargement, santé, bouclier, animation |
| `Boss` | IA : déplacement vers le joueur, attaque cooldown, tir ciblé, animation par phases |
| `AssetManager` | Chargement et accès aux textures et à la police via une `std::map` |
| `AudioManager` | Chargement et lecture des sons ; lecture en boucle de la musique |
| `UIManager` | Rendu du HUD (cœurs, score, munitions), menus, écrans de fin |
| `LevelManager` | Création et rendu des backgrounds (parallaxe) et des plateformes |

---

## 4. Principes POO Appliqués

### 4.1 Encapsulation

`Player` et `Boss` encapsulent leur état interne (santé, vélocité, timers) dans des membres privés. L'accès externe se fait exclusivement via des méthodes publiques :

```cpp
// Accès contrôlé — Player.h
int getHealth() const { return health; }
void takeDamage(int amount);
void heal(int amount);
```

### 4.2 Héritage

`Bullet` et `Boss` héritent de `Entity` qui fournit le `sf::RectangleShape` commun, `getBounds()`, `getPosition()`, `getSize()` et `draw()`. Cela évite la duplication de code pour les opérations graphiques communes.

```cpp
class Entity {
public:
    sf::RectangleShape shape;
    sf::FloatRect getBounds() const { return shape.getGlobalBounds(); }
    sf::Vector2f getPosition() const { return shape.getPosition(); }
    virtual void draw(sf::RenderWindow& window) const { window.draw(shape); }
};

class Boss : public Entity { /* réutilise shape directement */ };
class Bullet : public Entity { /* réutilise shape directement */ };
```

### 4.3 Abstraction

`Entity` fournit une interface unifiée. N'importe quel code peut manipuler `getBounds()` ou `draw()` sans connaître le type concret. Cela est utilisé notamment dans les boucles de collision de `Game`.

### 4.4 Composition vs. Héritage

`Game` compose les managers (`AssetManager`, `AudioManager`, etc.) par valeur — évitant la gestion manuelle de la durée de vie. Les entités dynamiques (`Boss`, `Enemy`, `Bullet`) sont gérées par des smart pointers ou des `std::vector`.

---

## 5. Boucle de Jeu et Physique

### 5.1 Boucle principale

```
Game::run()
  ┌─── sf::Clock::restart() → dt ───────────────────┐
  │  processEvents()   ← clavier, fermeture fenêtre  │
  │  update(dt)        ← physique, IA, collisions     │  × 60 FPS
  │  render()          ← dessin de toutes les entités │
  └──────────────────────────────────────────────────┘
```

Le **delta-time** (`dt`) découple la logique de jeu de la fréquence d'images, garantissant un comportement cohérent sur tout matériel.

### 5.2 Physique (intégrateur d'Euler)

```
velocity.y += 1400 × dt           // gravité
position   += velocity × dt       // intégration de position
```

Le saut applique une impulsion initiale :
```
si (Espace ET onGround) → velocity.y = −820 px/s
```

### 5.3 Résolution des collisions AABB

La collision est résolue en deux passes séparées pour éviter les artéfacts de coin :

1. **Passe horizontale :** déplacer X → détecter → corriger X
2. **Passe verticale :** déplacer Y → détecter → corriger Y et activer `onGround`

Les hitboxes du joueur et des ennemis sont plus petites que le sprite visuel (rétrécissement intentionnel) pour une détection plus équitable. L'API SFML `findIntersection()` détecte le chevauchement AABB.

---

## 6. Machine à États Finis

Le jeu est entièrement piloté par l'énumération `GameState` :

```
            ┌──────┐  [S]  ┌─────────┐  8 pièces  ┌───────────┐
Démarrage──▶│ MENU │──────▶│ PLAYING │───────────▶│ BOSS_FIGHT│
            └──┬───┘       └────┬────┘             └─────┬─────┘
               │[A]             │HP=0                     │HP=0     Boss HP=0
               ▼                ▼                         ▼         ▼
           ┌───────┐      ┌──────────┐               ┌──────────┐ ┌─────┐
           │ ABOUT │      │GAME_OVER │◀──────────────▶│GAME_OVER │ │ WIN │
           └───────┘      └────┬─────┘               └─────┬────┘ └──┬──┘
                               │[R]                         │[R]      │[R]
                               └────────────────────────────┴─────────┘
                                             PLAYING (restart)
```

Chaque méthode de `Game` filtre son comportement selon `gameState` :
- `processEvents()` : gère les touches selon l'état actif
- `update()` : ne s'exécute que dans `PLAYING` et `BOSS_FIGHT`
- `render()` : affiche les menus ou le HUD selon l'état

---

## 7. Systèmes de Jeu

### 7.1 Système de Score

| Source | Valeur |
|--------|--------|
| Survie | +120 pts/s |
| Ennemi éliminé | +250 pts |
| Pièce collectée | +500 pts |
| Coup sur le boss | +100 × dégâts |
| Boss vaincu | +5 000 + max(0, 15 000 − survivalTime × 150) |

### 7.2 Ennemis et Difficulté Progressive

Deux types d'ennemis apparaissent depuis le bord droit (x = 1300) :

| Ennemi | Type | Comportement |
|--------|------|-------------|
| Fantôme | Volant | Ligne droite horizontale, spawn sur plateformes en hauteur |
| Squelette | Sol | Spawn au sol, soumis à la gravité visuelle |

La vitesse et le taux d'apparition évoluent avec le temps de survie :
```
intervalle = max(0.8, 2.0 − (((survivalTime − 10) / 5) × 0.1))   si survivalTime > 10 s
vitesse    = 150 + ((survivalTime − 10) / 5 × 40) + rand(0, 200)
```

### 7.3 Intelligence Artificielle du Boss

Le boss araignée implémente une IA à trois phases :

1. **Déplacement :** si distance > 350 px du joueur → se rapproche à 80 px/s
2. **Idle :** dans la plage de portée → attend le cooldown d'attaque (2 s)
3. **Attaque :** animation de 5 frames ; au frame 3 : tire un projectile ciblé

Le calcul du tir ciblé utilise la normalisation vectorielle :
```cpp
sf::Vector2f direction = playerCenter - bossPos;
float distance = std::sqrt(direction.x² + direction.y²);
sf::Vector2f velocity = (direction / distance) * 700.f;
```

### 7.4 Système de Projectiles

Les projectiles `Bullet` (joueur et boss) sont stockés dans deux `std::vector<Bullet>` séparés. À chaque frame :
1. Position mise à jour : `shape.move(velocity × dt)`
2. Animation de sprite si `totalFrames > 1`
3. Test de collision avec cibles
4. Suppression si collision ou hors-écran (via itérateur erase)

Le dégât du shotgun sur le boss applique une atténuation par distance :
```cpp
int damage = std::max(1, 4 - (int)(|boss.x - bullet.originX| / 150.f));
```

### 7.5 Système de Ramassables

Chaque ramassable possède un flag `active` et un `cooldownTimer` :

| Ramassable | Condition d'apparition | Effet |
|-----------|----------------------|-------|
| Arc (Bow) | ≥ 15 s de survie | 30 flèches, rechargement auto (clip 6) |
| Potion de soin | HP ≤ 10 et cooldown = 0 | +6 HP, cooldown 15 s |
| Bouclier | BOSS_FIGHT, cooldown 20 s | Immunité 3 s + aura visuelle |
| Arc doré (Shotgun) | BOSS_FIGHT, ammo = 0, après 6 s | Fusil à pompe, 15 munitions (clip 3) |

---

## 8. Gestion des Ressources et de la Mémoire

### 8.1 RAII et Pointeurs Intelligents

Le boss est géré par `std::unique_ptr<Boss>`, garantissant la libération automatique :
```cpp
boss = std::make_unique<Boss>(1100.f, 720.f - 50.f);
// boss.reset() ou réassignation → libération automatique
```

Les textures (`sf::Texture`) et sons (`sf::SoundBuffer`) sont stockés dans des `std::map` membres de `AssetManager` et `AudioManager`, garantissant leur durée de vie pour toute la session.

### 8.2 Conteneurs STL

Ennemis, balles et pièces sont stockés dans des `std::vector`. La suppression utilise l'idiome erase-itérateur pour éviter l'invalidation :
```cpp
for (auto it = enemies.begin(); it != enemies.end(); ) {
    it->update(dt, platforms);
    if (it->isDead) it = enemies.erase(it);
    else ++it;
}
```

### 8.3 Architecture Modulaire des Managers

La séparation en managers (`AssetManager`, `AudioManager`, `UIManager`, `LevelManager`) applique le **Principe de Responsabilité Unique** : chaque classe a une raison unique de changer. Cela rend le code testable et maintenable indépendamment.

---

## 9. Rendu et Interface Utilisateur

### 9.1 Ordre de Rendu (Algorithme du Peintre)

```
1. Fond (back → far → middle)
2. Plateformes
3. Ennemis
4. Pièces
5. Projectiles (joueur et boss)
6. Ramassables
7. Boss
8. Joueur
9. HUD (score, cœurs, munitions, pièces)
10. Superpositions (pause, GAME_OVER, WIN)
```

### 9.2 Fond en Parallaxe

Trois couches (`back.png`, `far.png`, `middle.png`) sont mises à l'échelle pour couvrir exactement la fenêtre 720 px :
```cpp
float scale = 720.f / back.getSize().y;
backSprite->setScale({scale, scale});
backSprite->setTextureRect({0, 0, (int)(1280 / scale), (int)back.getSize().y});
```
Les textures sont marquées `setRepeated(true)` pour permettre le remplissage horizontal.

### 9.3 Retour Visuel

| Événement | Visuel |
|-----------|--------|
| Joueur blessé | Clignotement (alternance rouge/blanc) pendant 1 s |
| Bouclier actif | Cercle cyan translucide autour du joueur |
| Boss face au joueur | Scale X −1 ou +1 selon `facingRight` |
| Boss touché | Couleur rouge pendant 0.3 s (hitTimer) |

---

## 10. Difficultés et Solutions

### 10.1 Durée de vie des textures SFML

**Problème :** `sf::RectangleShape::setTexture()` stocke un pointeur brut. Si la texture est détruite ou déplacée (réallocation de `std::map`), le rendu produit des artéfacts.

**Solution :** Toutes les textures sont stockées dans la `std::map` de `AssetManager` (membre de `Game`) qui dure toute la session. Les textures ne sont jamais copiées après leur chargement initial (`std::move` à l'insertion).

### 10.2 Flipping du sprite avec origine décalée

**Problème :** Retourner un sprite via `setScale({-1, 1})` provoque un déplacement visible si l'origine n'est pas au centre.

**Solution :** L'origine du joueur est fixée à `{75, 75}` (centre du sprite 150×150) et celle du boss à `{437.5, 875}` (centre bas), neutralisant le déplacement lors du flip.

### 10.3 Invalidation d'itérateurs dans les boucles d'effacement

**Problème :** Utiliser `for (auto& e : vector)` avec `vector.erase()` est un comportement indéfini en C++.

**Solution :** Utilisation systématique du patron `it = container.erase(it)` lors d'une suppression, `++it` sinon.

### 10.4 Son SFML 3 : construction par défaut de sf::Sound

**Problème :** En SFML 3, `sf::Sound` ne peut pas être construit sans un buffer valide. L'insertion directe dans `std::map` avec `sounds[name]` tentait une construction par défaut.

**Solution :** Utilisation de `sounds.emplace(name, sf::Sound(buffers.at(name)))` pour construire le son directement avec son buffer.

---

## 11. Résultats et Tests

### 11.1 Tests fonctionnels

| Scénario | Résultat |
|----------|---------|
| Lancement → menu affiché | ✓ |
| S → démarrage de partie | ✓ |
| Déplacement gauche/droite, saut | ✓ |
| Tir arc (X) et rechargement (C) | ✓ |
| Contact ennemi → −3 HP, clignotement | ✓ |
| Flèche touche ennemi → mort animée + score | ✓ |
| 8 pièces → boss spawne, ennemis effacés | ✓ |
| Boss HP = 0 → écran WIN avec score | ✓ |
| Joueur HP = 0 → animation mort → GAME_OVER | ✓ |
| R depuis GAME_OVER/WIN → redémarrage complet | ✓ |
| Potion, bouclier, arc doré collectés | ✓ |
| Pause (P) → reprise/restart/menu | ✓ |
| Musique et effets sonores actifs | ✓ |

### 11.2 Performance

Le jeu maintient 60 FPS stables. La complexité par frame est O(n) en nombre d'entités actives (< 20 ennemis, < 30 projectiles simultanément), ce qui est négligeable pour la limite matérielle.

---

## 12. Conclusion

Ce projet démontre qu'il est possible de développer un jeu complet et jouable en C++ en appliquant rigoureusement les principes fondamentaux de la POO. L'encapsulation de l'état du joueur et du boss, la séparation en managers spécialisés, la gestion automatique de la mémoire par RAII, et la machine à états finis forment ensemble une architecture cohérente et extensible.

Les problèmes concrets rencontrés — durée de vie des textures, flipping avec origine, construction de `sf::Sound` en SFML 3 — illustrent l'importance de comprendre la sémantique des bibliothèques utilisées, pas seulement leur API de surface.

Le jeu est entièrement fonctionnel : de l'écran de menu à la victoire sur le boss, tous les systèmes communiquent correctement. Ce travail constitue une base solide pour des extensions futures (niveaux supplémentaires, types d'ennemis additionnels, système de particules).

---

## 13. Bibliographie

1. **SFML Documentation** — *SFML 3.0 API Reference*, https://www.sfml-dev.org/documentation/3.0/
2. **Stroustrup, B.** (2013). *The C++ Programming Language*, 4th Edition. Addison-Wesley.
3. **Meyers, S.** (2014). *Effective Modern C++*. O'Reilly Media.
4. **Nystrom, R.** (2014). *Game Programming Patterns*. Genever Benning. https://gameprogrammingpatterns.com/
5. **cppreference.com** — *C++ Standard Library Reference*, https://en.cppreference.com/

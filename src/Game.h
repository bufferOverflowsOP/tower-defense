#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <vector>
#include "Arrow.h"
#include "Assets.h"
#include "Enemy.h"
#include "HpBar.h"
#include "Tower.h"

class Game {
  public:
    Game();

    void run();

  private:
    void handleEvents();
    void update(float dt);
    void draw();
    void spawnEnemies(float dt);
    void updateGuards(float dt);
    Enemy* nearestEnemy(sf::Vector2f position);
    bool canPlaceTower(int col, int row) const;
    void updateGoldText();
    bool handleShopClick(sf::Vector2f click);
    void updateShopUi();
    void drawShop();
    void updateWaveText();
    int selectedTowerCost() const;
    const sf::Texture& selectedTowerTexture() const;

    sf::RenderWindow m_window;
    Assets m_assets;
    sf::Sprite m_grass;
    sf::Sprite m_path;
    sf::Sprite m_castle;
    sf::Sprite m_preview;
    sf::Sprite m_goldIcon;
    sf::Text m_goldText;
    sf::Text m_waveText;
    sf::Sprite m_archerCard;
    sf::Sprite m_barracksCard;
    sf::Sprite m_archerIcon;
    sf::Sprite m_barracksIcon;
    sf::Sprite m_archerCostIcon;
    sf::Sprite m_barracksCostIcon;
    sf::Text m_archerCostText;
    sf::Text m_barracksCostText;
    HpBar m_hpBar;
    std::vector<Enemy> m_enemies;
    std::map<int, Tower> m_towers;
    std::vector<Arrow> m_arrows;
    struct BarracksGuard {
        BarracksGuard(const sf::Texture& idleTexture, const sf::Texture& attackTexture,
                      sf::Vector2f pos);

        void update(float dt, Enemy* target);
        void draw(sf::RenderWindow& window);
        sf::Vector2f position() const;

        sf::Sprite sprite;
        const sf::Texture* idleTexture;
        const sf::Texture* attackTexture;
        float strikeTimer = 0.f;
        float animTimer = 0.f;
        int animFrame = 0;
        bool attacking = false;
    };
    std::vector<BarracksGuard> m_guards;
    TowerKind m_selectedTower = TowerKind::Archer;
    int m_castleHp = 0;
    int m_gold = 25;
    int m_wave = 1;
    int m_waveEnemiesLeft = 1;
    float m_spawnTimer = 0.f;
    bool m_waitingForWaveClear = false;
    bool m_betweenWaves = false;
    int m_castleMinCol = 0;
    int m_castleMaxCol = 0;
    int m_castleMinRow = 0;
    int m_castleMaxRow = 0;
    int m_hoverCol = -1;
    int m_hoverRow = -1;
};

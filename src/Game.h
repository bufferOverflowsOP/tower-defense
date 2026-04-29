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
    bool canPlaceTower(int col, int row) const;

    sf::RenderWindow m_window;
    Assets m_assets;
    sf::Sprite m_grass;
    sf::Sprite m_path;
    sf::Sprite m_castle;
    Enemy m_enemy;
    sf::Sprite m_preview;
    HpBar m_hpBar;
    std::map<int, Tower> m_towers;
    std::vector<Arrow> m_arrows;
    int m_castleHp = 0;
    int m_castleMinCol = 0;
    int m_castleMaxCol = 0;
    int m_castleMinRow = 0;
    int m_castleMaxRow = 0;
    int m_hoverCol = -1;
    int m_hoverRow = -1;
};

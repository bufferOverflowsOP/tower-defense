#include "Game.h"
#include <algorithm>
#include <cmath>
#include <optional>
#include "GameConfig.h"

namespace {

// clang-format off
const char* pathLayout[cfg::rows] = { // . = grass, X = path
    ".....................",
    ".....................",
    "XXXXXXXXXXX..........",
    "..........X..........",
    "..........X..........",
    "..........X..........",
    "..........XXXXXXXX...",
    "..........X..........",
    "..........X..........",
    "XXXXXXXXXXX..........",
    ".....................",
    ".....................",
};
// clang-format on

sf::Vector2f tileCenter(int col, int row) {
    return {col * cfg::tileSize + cfg::tileSize / 2.f,
            row * cfg::tileSize + cfg::tileSize / 2.f};
}

sf::Vector2f tileBottom(int col, int row) {
    return {col * cfg::tileSize + cfg::tileSize / 2.f, float((row + 1) * cfg::tileSize)};
}

int towerKey(int col, int row) {
    return row * cfg::cols + col;
}

}  // namespace

Game::Game()
    : m_window(sf::VideoMode({cfg::windowWidth, cfg::windowHeight}), "Tower Defense"),
      m_assets(),
      m_grass(m_assets.grass),
      m_path(m_assets.path),
      m_castle(m_assets.castle),
      m_enemy(m_assets.enemyRun, m_assets.enemyAttack, tileCenter(0, 2),
              {tileCenter(10, 2), tileCenter(10, 6), tileCenter(17, 6)}),
      m_preview(m_assets.tower),
      m_hpBar(m_assets.hpBarBase, m_assets.hpBarFill),
      m_castleHp(cfg::castleMaxHp) {
    m_window.setVerticalSyncEnabled(true);
    m_window.setFramerateLimit(60);

    sf::IntRect tileRect({{cfg::tileSize, cfg::tileSize}, {cfg::tileSize, cfg::tileSize}});
    m_grass.setTextureRect(tileRect);
    m_path.setTextureRect(tileRect);

    auto size = m_assets.castle.getSize();
    m_castle.setOrigin({size.x / 2.f, size.y / 2.f});
    m_castle.rotate(sf::degrees(90.f));
    m_castle.setPosition(
        {cfg::cols * cfg::tileSize - size.y / 2.f, 6 * cfg::tileSize + cfg::tileSize / 2.f});

    Tower::configure(m_preview, m_assets.tower);
    m_preview.setColor(sf::Color(255, 255, 255, 120));

    auto castleBounds = m_castle.getGlobalBounds();
    m_castleMinCol = int(castleBounds.position.x) / cfg::tileSize;
    m_castleMaxCol = int(castleBounds.position.x + castleBounds.size.x - 1.f) / cfg::tileSize;
    m_castleMinRow = int(castleBounds.position.y) / cfg::tileSize;
    m_castleMaxRow = int(castleBounds.position.y + castleBounds.size.y) / cfg::tileSize;
}

void Game::run() {
    sf::Clock clock;
    while (m_window.isOpen()) {
        float dt = clock.restart().asSeconds();
        handleEvents();
        if (!m_window.isOpen()) break;
        update(dt);
        if (!m_window.isOpen()) break;
        draw();
    }
}

void Game::handleEvents() {
    while (const std::optional event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) m_window.close();

        if (auto* mouseMove = event->getIf<sf::Event::MouseMoved>()) {
            sf::Vector2f pos = m_window.mapPixelToCoords(mouseMove->position);
            m_hoverCol = int(pos.x) / cfg::tileSize;
            m_hoverRow = int(pos.y) / cfg::tileSize;
        }

        if (auto* mousePress = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mousePress->button == sf::Mouse::Button::Left) {
                sf::Vector2f click = m_window.mapPixelToCoords(mousePress->position);
                int col = int(click.x) / cfg::tileSize;
                int row = int(click.y) / cfg::tileSize;
                if (canPlaceTower(col, row)) {
                    m_towers.try_emplace(towerKey(col, row), m_assets.tower, m_assets.archer,
                                         m_assets.archerShoot, tileBottom(col, row));
                }
            }
        }
    }
}

void Game::update(float dt) {
    m_enemy.update(dt);

    if (m_enemy.didAttack()) {
        m_castleHp = std::max(0, m_castleHp - 1);
        if (m_castleHp == 0) {
            m_window.close();
            return;
        }
    } else if (m_enemy.isDead()) {
        m_enemy.reset();
        m_arrows.clear();
    }

    m_hpBar.update(m_castleHp);

    for (auto& [_, tower] : m_towers) {
        tower.update(dt, m_enemy.getPosition(), m_enemy.getVelocity());
        if (tower.didFire()) {
            m_arrows.emplace_back(m_assets.arrow, tower.archerPosition(),
                                  tower.fireDirection() * Tower::kArrowSpeed);
        }
    }

    for (auto& arrow : m_arrows) {
        arrow.update(dt);
        if (!arrow.wasHit()) {
            sf::Vector2f diff = arrow.getPosition() - m_enemy.getPosition();
            if (std::hypot(diff.x, diff.y) < 20.f && !m_enemy.isDead()) {
                m_enemy.takeDamage(1);
                arrow.markHit();
            }
        }
    }

    m_arrows.erase(std::remove_if(m_arrows.begin(), m_arrows.end(),
                                  [](const Arrow& arrow) { return arrow.isExpired(); }),
                   m_arrows.end());
}

void Game::draw() {
    m_window.clear();

    for (int row = 0; row < cfg::rows; row++) {
        for (int col = 0; col < cfg::cols; col++) {
            sf::Vector2f pos(float(col * cfg::tileSize), float(row * cfg::tileSize));
            m_grass.setPosition(pos);
            m_window.draw(m_grass);
            if (pathLayout[row][col] == 'X') {
                m_path.setPosition(pos);
                m_window.draw(m_path);
            }
        }
    }

    for (auto& [_, tower] : m_towers) {
        tower.draw(m_window);
    }

    for (const auto& arrow : m_arrows) {
        arrow.draw(m_window);
    }

    if (canPlaceTower(m_hoverCol, m_hoverRow)) {
        m_preview.setPosition(tileBottom(m_hoverCol, m_hoverRow));
        m_window.draw(m_preview);
    }

    m_window.draw(m_castle);
    m_enemy.draw(m_window);
    m_hpBar.draw(m_window);
    m_window.display();
}

bool Game::canPlaceTower(int col, int row) const {
    return col >= 0 && col < cfg::cols && row >= 0 && row < cfg::rows &&
           pathLayout[row][col] != 'X' && !m_towers.count(towerKey(col, row)) &&
           !(col >= m_castleMinCol && col <= m_castleMaxCol && row >= m_castleMinRow &&
             row <= m_castleMaxRow);
}

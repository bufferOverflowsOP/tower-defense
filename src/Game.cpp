#include "Game.h"
#include <algorithm>
#include <cmath>
#include <optional>
#include <string>
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
    return {col * cfg::tileSize + cfg::tileSize / 2.f, row * cfg::tileSize + cfg::tileSize / 2.f};
}

sf::Vector2f tileBottom(int col, int row) {
    return {col * cfg::tileSize + cfg::tileSize / 2.f, float((row + 1) * cfg::tileSize)};
}

int towerKey(int col, int row) {
    return row * cfg::cols + col;
}

std::vector<sf::Vector2f> enemyWaypoints() {
    return {tileCenter(10, 2), tileCenter(10, 6), tileCenter(17, 6)};
}

constexpr float kSpawnDelay = 1.f;
constexpr float kArrowHitRadius = 20.f;
constexpr int kArcherCost = 25;
constexpr int kBarracksCost = 40;
constexpr int kEnemyReward = 10;
constexpr sf::Vector2f kArcherSlotPosition{12.f, 676.f};
constexpr sf::Vector2f kBarracksSlotPosition{84.f, 676.f};
constexpr sf::Vector2f kShopSlotSize{64.f, 80.f};

sf::FloatRect shopSlotBounds(TowerKind kind) {
    sf::Vector2f pos = kind == TowerKind::Archer ? kArcherSlotPosition : kBarracksSlotPosition;
    return {pos, kShopSlotSize};
}

} // namespace

Game::Game()
    : m_window(sf::VideoMode({cfg::windowWidth, cfg::windowHeight}), "Tower Defense"), m_assets(),
      m_grass(m_assets.grass), m_path(m_assets.path), m_castle(m_assets.castle),
      m_preview(m_assets.tower), m_goldIcon(m_assets.goldIcon), m_goldText(m_assets.uiFont, "", 24),
      m_archerCard(m_assets.shopCard), m_barracksCard(m_assets.shopCard),
      m_archerIcon(m_assets.tower), m_barracksIcon(m_assets.barracks),
      m_archerCostIcon(m_assets.goldIcon), m_barracksCostIcon(m_assets.goldIcon),
      m_archerCostText(m_assets.uiFont, std::to_string(kArcherCost), 14),
      m_barracksCostText(m_assets.uiFont, std::to_string(kBarracksCost), 14),
      m_hpBar(m_assets.hpBarBase, m_assets.hpBarFill), m_castleHp(cfg::castleMaxHp),
      m_spawnTimer(kSpawnDelay) {
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

    m_goldIcon.setScale({0.5f, 0.5f});
    m_goldIcon.setPosition({180.f, 10.f});
    m_goldText.setFillColor(sf::Color(255, 230, 90));
    m_goldText.setOutlineColor(sf::Color(45, 25, 12));
    m_goldText.setOutlineThickness(2.f);
    m_goldText.setPosition({216.f, 8.f});
    updateGoldText();

    m_archerCard.setScale({0.5f, 0.5f});
    m_barracksCard.setScale({0.5f, 0.5f});
    m_archerCard.setPosition(kArcherSlotPosition);
    m_barracksCard.setPosition(kBarracksSlotPosition);
    Tower::configure(m_archerIcon, m_assets.tower);
    Tower::configure(m_barracksIcon, m_assets.barracks);
    m_archerIcon.setScale({0.25f, 0.25f});
    m_barracksIcon.setScale({0.25f, 0.25f});
    m_archerIcon.setPosition(kArcherSlotPosition + sf::Vector2f{32.f, 48.f});
    m_barracksIcon.setPosition(kBarracksSlotPosition + sf::Vector2f{32.f, 48.f});
    m_archerCostIcon.setScale({0.25f, 0.25f});
    m_barracksCostIcon.setScale({0.25f, 0.25f});
    m_archerCostIcon.setPosition(kArcherSlotPosition + sf::Vector2f{12.f, 60.f});
    m_barracksCostIcon.setPosition(kBarracksSlotPosition + sf::Vector2f{12.f, 60.f});
    m_archerCostText.setFillColor(m_goldText.getFillColor());
    m_archerCostText.setOutlineColor(m_goldText.getOutlineColor());
    m_archerCostText.setOutlineThickness(m_goldText.getOutlineThickness());
    m_archerCostText.setPosition(kArcherSlotPosition + sf::Vector2f{29.f, 57.f});
    m_barracksCostText.setFillColor(m_goldText.getFillColor());
    m_barracksCostText.setOutlineColor(m_goldText.getOutlineColor());
    m_barracksCostText.setOutlineThickness(m_goldText.getOutlineThickness());
    m_barracksCostText.setPosition(kBarracksSlotPosition + sf::Vector2f{29.f, 57.f});
    updateShopUi();

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
        if (!m_window.isOpen())
            break;
        update(dt);
        if (!m_window.isOpen())
            break;
        draw();
    }
}

void Game::handleEvents() {
    while (const std::optional event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>())
            m_window.close();

        if (auto* mouseMove = event->getIf<sf::Event::MouseMoved>()) {
            sf::Vector2f pos = m_window.mapPixelToCoords(mouseMove->position);
            m_hoverCol = int(pos.x) / cfg::tileSize;
            m_hoverRow = int(pos.y) / cfg::tileSize;
        }

        if (auto* mousePress = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mousePress->button == sf::Mouse::Button::Left) {
                sf::Vector2f click = m_window.mapPixelToCoords(mousePress->position);
                if (handleShopClick(click))
                    continue;

                int col = int(click.x) / cfg::tileSize;
                int row = int(click.y) / cfg::tileSize;
                int cost = selectedTowerCost();
                if (m_gold >= cost && canPlaceTower(col, row)) {
                    m_towers.try_emplace(towerKey(col, row), m_selectedTower,
                                         selectedTowerTexture(), m_assets.archer,
                                         m_assets.archerShoot, tileBottom(col, row));
                    m_gold -= cost;
                    updateGoldText();
                    updateShopUi();
                }
            }
        }
    }
}

void Game::update(float dt) {
    spawnEnemies(dt);

    bool shouldClose = false;
    for (auto& enemy : m_enemies) {
        enemy.update(dt);
        if (enemy.didAttack()) {
            m_castleHp = std::max(0, m_castleHp - 1);
            shouldClose = m_castleHp == 0;
        }
    }

    m_enemies.erase(std::remove_if(m_enemies.begin(), m_enemies.end(),
                                   [this](const Enemy& enemy) {
                                       if (enemy.isDead()) {
                                           m_gold += kEnemyReward;
                                           updateGoldText();
                                           updateShopUi();
                                           return true;
                                       }
                                       return false;
                                   }),
                    m_enemies.end());

    if (shouldClose) {
        m_window.close();
        return;
    }

    m_hpBar.update(m_castleHp);

    for (auto& [_, tower] : m_towers) {
        Enemy* target = nearestEnemy(tower.position());
        if (target) {
            tower.update(dt, target->getPosition(), target->getVelocity());
        } else {
            tower.update(dt);
        }

        if (tower.didFire()) {
            m_arrows.emplace_back(m_assets.arrow, tower.archerPosition(),
                                  tower.fireDirection() * Tower::kArrowSpeed);
        }
    }

    for (auto& arrow : m_arrows) {
        arrow.update(dt);
        if (!arrow.wasHit()) {
            for (auto& enemy : m_enemies) {
                sf::Vector2f diff = arrow.getPosition() - enemy.getPosition();
                if (std::hypot(diff.x, diff.y) < kArrowHitRadius && !enemy.isDead()) {
                    enemy.takeDamage(1);
                    arrow.markHit();
                    break;
                }
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
        m_preview.setTexture(selectedTowerTexture(), true);
        Tower::configure(m_preview, selectedTowerTexture());
        m_preview.setPosition(tileBottom(m_hoverCol, m_hoverRow));
        m_window.draw(m_preview);
    }

    m_window.draw(m_castle);
    for (auto& enemy : m_enemies) {
        enemy.draw(m_window);
    }
    m_hpBar.draw(m_window);
    m_window.draw(m_goldIcon);
    m_window.draw(m_goldText);
    drawShop();
    m_window.display();
}

void Game::spawnEnemies(float dt) {
    m_spawnTimer += dt;
    if (m_spawnTimer < kSpawnDelay)
        return;

    m_spawnTimer = 0.f;
    m_enemies.emplace_back(m_assets.enemyRun, m_assets.enemyAttack, tileCenter(0, 2),
                           enemyWaypoints());
}

Enemy* Game::nearestEnemy(sf::Vector2f position) {
    Enemy* closest = nullptr;
    float closestDist = 0.f;

    for (auto& enemy : m_enemies) {
        if (enemy.isDead())
            continue;

        float dist = (enemy.getPosition() - position).length();
        if (!closest || dist < closestDist) {
            closest = &enemy;
            closestDist = dist;
        }
    }

    return closest;
}

bool Game::canPlaceTower(int col, int row) const {
    return col >= 0 && col < cfg::cols && row >= 0 && row < cfg::rows &&
           pathLayout[row][col] != 'X' && !m_towers.count(towerKey(col, row)) &&
           !(col >= m_castleMinCol && col <= m_castleMaxCol && row >= m_castleMinRow &&
             row <= m_castleMaxRow);
}

void Game::updateGoldText() {
    m_goldText.setString(std::to_string(m_gold));
}

bool Game::handleShopClick(sf::Vector2f click) {
    if (shopSlotBounds(TowerKind::Archer).contains(click)) {
        m_selectedTower = TowerKind::Archer;
        updateShopUi();
        return true;
    }

    if (shopSlotBounds(TowerKind::Barracks).contains(click)) {
        m_selectedTower = TowerKind::Barracks;
        updateShopUi();
        return true;
    }

    return false;
}

void Game::updateShopUi() {
    m_archerCard.setTexture(m_selectedTower == TowerKind::Archer ? m_assets.shopButtonSelected
                                                                 : m_assets.shopCard,
                            true);
    m_barracksCard.setTexture(m_selectedTower == TowerKind::Barracks ? m_assets.shopButtonSelected
                                                                     : m_assets.shopCard,
                              true);
    m_archerIcon.setColor(m_gold < kArcherCost ? sf::Color(120, 120, 120) : sf::Color::White);
    m_barracksIcon.setColor(m_gold < kBarracksCost ? sf::Color(120, 120, 120) : sf::Color::White);
    m_archerCostIcon.setColor(m_gold < kArcherCost ? sf::Color(120, 120, 120) : sf::Color::White);
    m_barracksCostIcon.setColor(m_gold < kBarracksCost ? sf::Color(120, 120, 120)
                                                       : sf::Color::White);
    m_archerCostText.setFillColor(m_gold < kArcherCost ? sf::Color(150, 130, 80)
                                                       : m_goldText.getFillColor());
    m_barracksCostText.setFillColor(m_gold < kBarracksCost ? sf::Color(150, 130, 80)
                                                           : m_goldText.getFillColor());
}

void Game::drawShop() {
    m_window.draw(m_archerCard);
    m_window.draw(m_barracksCard);
    m_window.draw(m_archerIcon);
    m_window.draw(m_barracksIcon);
    m_window.draw(m_archerCostIcon);
    m_window.draw(m_barracksCostIcon);
    m_window.draw(m_archerCostText);
    m_window.draw(m_barracksCostText);
}

int Game::selectedTowerCost() const {
    return m_selectedTower == TowerKind::Archer ? kArcherCost : kBarracksCost;
}

const sf::Texture& Game::selectedTowerTexture() const {
    return m_selectedTower == TowerKind::Archer ? m_assets.tower : m_assets.barracks;
}

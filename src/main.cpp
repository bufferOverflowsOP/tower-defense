#include <SFML/Graphics.hpp>
#include <map>
#include "Enemy.h"
#include "Tower.h"

const int tileSize = 64;
const int cols = 21, rows = 12;

// clang-format off
const char* pathLayout[rows] = { // . = grass, X = path
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
    return {col * tileSize + tileSize / 2.f, row * tileSize + tileSize / 2.f};
}

sf::Vector2f tileBottom(int col, int row) {
    return {col * tileSize + tileSize / 2.f, float((row + 1) * tileSize)};
}

int towerKey(int col, int row) {
    return row * cols + col;
}

int main() {
    sf::RenderWindow window(sf::VideoMode({cols * tileSize, rows * tileSize}),
                            "Tower Defense");
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    int pathMap[rows][cols]{};
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            pathMap[i][j] = (pathLayout[i][j] == 'X') ? 1 : 0;
        }
    }

    // grass and path (path is just darker version of grass)
    // TODO: add auto-tiling for grass/path edge
    sf::Texture grassTexture, pathTexture;
    grassTexture.loadFromFile("assets/Terrain/Tileset/Tilemap_color1.png");
    pathTexture.loadFromFile("assets/Terrain/Tileset/Tilemap_color4.png");
    sf::Sprite grassSprite(grassTexture), pathSprite(pathTexture);
    sf::IntRect tileRect({{tileSize, tileSize}, {tileSize, tileSize}});
    grassSprite.setTextureRect(tileRect);
    pathSprite.setTextureRect(tileRect);

    // red castle
    sf::Texture castleTexture;
    castleTexture.loadFromFile("assets/Buildings/Red Buildings/Castle.png");
    sf::Sprite castle(castleTexture);
    auto size = castleTexture.getSize();
    castle.setOrigin({size.x / 2.f, size.y / 2.f});
    castle.rotate(sf::degrees(90.f));
    castle.setPosition({cols * tileSize - size.y / 2.f, 6 * tileSize + tileSize / 2.f});

    // enemy
    sf::Texture enemyTexture;
    enemyTexture.loadFromFile("assets/Units/Black Units/Warrior/Warrior_Run.png");
    Enemy enemy(enemyTexture, tileCenter(0, 2),
                {tileCenter(10, 2), tileCenter(10, 6), tileCenter(17, 6)});

    // tower
    sf::Texture towerTexture;
    towerTexture.loadFromFile("assets/Buildings/Red Buildings/Tower.png");
    std::map<int, Tower> towers;

    // tower preview
    sf::Sprite previewSprite(towerTexture);
    Tower::configure(previewSprite, towerTexture);
    previewSprite.setColor(sf::Color(255, 255, 255, 120));

    auto canPlaceTower = [&](int col, int row) -> bool {
        return col >= 0 && col < cols && row >= 0 && row < rows && !pathMap[row][col] &&
               !towers.count(towerKey(col, row));
    };

    int hoverCol = -1, hoverRow = -1;

    sf::Clock clock;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();

            if (auto* m = event->getIf<sf::Event::MouseMoved>()) {
                sf::Vector2f pos = window.mapPixelToCoords(m->position);
                hoverCol = int(pos.x) / tileSize;
                hoverRow = int(pos.y) / tileSize;
            }

            if (auto* e = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (e->button == sf::Mouse::Button::Left) {
                    sf::Vector2f click = window.mapPixelToCoords(e->position);
                    int col = int(click.x) / tileSize;
                    int row = int(click.y) / tileSize;
                    if (canPlaceTower(col, row)) {
                        towers.emplace(towerKey(col, row),
                                       Tower{towerTexture, tileBottom(col, row)});
                    }
                }
            }
        }

        enemy.update(dt);

        window.clear();
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                sf::Vector2f pos(float(j * tileSize), float(i * tileSize));
                grassSprite.setPosition(pos);
                window.draw(grassSprite);
                if (pathMap[i][j]) {
                    pathSprite.setPosition(pos);
                    window.draw(pathSprite);
                }
            }
        }

        for (auto& [_, tower] : towers) {
            tower.draw(window);
        }

        if (canPlaceTower(hoverCol, hoverRow)) {
            previewSprite.setPosition(tileBottom(hoverCol, hoverRow));
            window.draw(previewSprite);
        }

        window.draw(castle);
        enemy.draw(window);
        window.display();
    }
}

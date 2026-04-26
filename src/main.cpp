#include <SFML/Graphics.hpp>
#include "Enemy.h"

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
	// TODO: add auto-tiling for grass/path edges
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

    sf::Clock clock;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        while (const std::optional event = window.pollEvent())
            if (event->is<sf::Event::Closed>()) window.close();

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

        window.draw(castle);
        enemy.draw(window);
        window.display();
    }
}

#include <SFML/Graphics.hpp>

const int tileSize = 64;
const int cols = 20, rows = 12;

int main() {


    sf::RenderWindow window(sf::VideoMode({cols * tileSize, rows * tileSize}), "Tower Defense");
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    // grass
	// TODO: set up auto-tiling for the terrain
    sf::Texture terrainTexture;
    terrainTexture.loadFromFile("assets/Terrain/Tileset/Tilemap_color1.png");
    sf::Sprite tile(terrainTexture);
    tile.setTextureRect(sf::IntRect({64, 64}, {tileSize, tileSize}));

    // red castle
    sf::Texture castleTexture;
    castleTexture.loadFromFile("assets/Buildings/Red Buildings/Castle.png");
    sf::Sprite castle(castleTexture);
    auto size = castleTexture.getSize();
    castle.setOrigin({size.x / 2.f, size.y / 2.f}); // make origin center
    castle.setPosition({cols * tileSize / 2.f, tileSize * 1.5f}); // place center top

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent())
            if (event->is<sf::Event::Closed>()) window.close();

        window.clear();

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                tile.setPosition({j * (float)tileSize, i * (float)tileSize});
                window.draw(tile);
            }
		}

        window.draw(castle);
        window.display();
    }
}

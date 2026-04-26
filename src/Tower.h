#pragma once
#include <SFML/Graphics.hpp>

class Tower {
  public:
    static void configure(sf::Sprite& s, const sf::Texture& t) {
        auto size = t.getSize();
        s.setOrigin({size.x / 2.f, float(size.y)});
        s.setScale({0.5f, 0.5f});
    }

    Tower(const sf::Texture& texture, const sf::Texture& archerTexture, sf::Vector2f pos)
        : m_sprite(texture), m_archer(archerTexture) {
        configure(m_sprite, texture);
        m_sprite.setPosition(pos);

        m_archer.setTextureRect({{0, 0}, {192, 192}});
        m_archer.setOrigin({96.f, 192.f});
        m_archer.setScale({kScale, 0.6f});
        m_archer.setPosition(pos - sf::Vector2f(0.f, 40.f));
    }

    void update(float, sf::Vector2f enemyPos) {
        float dir = (enemyPos.x < m_sprite.getPosition().x) ? -1.f : 1.f;
        m_archer.setScale({dir * kScale, 0.6f});
    }

    void draw(sf::RenderWindow& window) {
        window.draw(m_sprite);
        window.draw(m_archer);
    }

  private:
    static constexpr float kScale = 0.6f;

    sf::Sprite m_sprite;
    sf::Sprite m_archer;
};

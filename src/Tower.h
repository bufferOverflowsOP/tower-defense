#pragma once
#include <SFML/Graphics.hpp>

class Tower {
  public:
    static void configure(sf::Sprite& s, const sf::Texture& t) {
        auto size = t.getSize();
        s.setOrigin({size.x / 2.f, float(size.y)});
        s.setScale({0.5f, 0.5f});
    }

    Tower(const sf::Texture& texture, sf::Vector2f pos) : m_sprite(texture) {
        configure(m_sprite, texture);
        m_sprite.setPosition(pos);
    }

    void draw(sf::RenderWindow& window) { window.draw(m_sprite); }

  private:
    sf::Sprite m_sprite;
};

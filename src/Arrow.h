#pragma once
#include <SFML/Graphics.hpp>
#include "GameConfig.h"

class Arrow {
  public:
    Arrow(const sf::Texture& texture, sf::Vector2f pos, sf::Vector2f velocity)
        : m_sprite(texture), m_pos(pos), m_velocity(velocity) {
        auto sz = sf::Vector2f(texture.getSize());
        m_sprite.setOrigin(sz / 2.f);
        m_sprite.setRotation(velocity.angle());
        m_sprite.setPosition(m_pos);
    }

    void update(float dt) {
        m_pos += m_velocity * dt;
        m_sprite.setPosition(m_pos);
    }

    void draw(sf::RenderWindow& window) const { window.draw(m_sprite); }

    sf::Vector2f getPosition() const { return m_pos; }

    void markHit() { m_hit = true; }
    bool wasHit() const { return m_hit; }

    bool isExpired() const {
        return m_hit || m_pos.x < 0.f || m_pos.x > cfg::windowWidth || m_pos.y < 0.f ||
               m_pos.y > cfg::windowHeight;
    }

  private:
    sf::Sprite m_sprite;
    sf::Vector2f m_pos;
    sf::Vector2f m_velocity;
    bool m_hit = false;
};

#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Enemy {
  public:
    Enemy(const sf::Texture& texture, sf::Vector2f startPos,
          std::vector<sf::Vector2f> waypoints)
        : m_sprite(texture), m_waypoints(std::move(waypoints)), m_pos(startPos) {
        m_sprite.setTextureRect({{0, 0}, {192, 192}});
        m_sprite.setOrigin({96.f, 96.f});
    }

    void update(float dt) {
        m_animTimer += dt;
        if (m_animTimer >= m_frameDuration) {
            m_animTimer -= m_frameDuration;
            m_animFrame = (m_animFrame + 1) % 6;
            m_sprite.setTextureRect({{m_animFrame * 192, 0}, {192, 192}});
        }

        if (m_wpIdx < (int)m_waypoints.size()) {
            sf::Vector2f dir = m_waypoints[m_wpIdx] - m_pos;
            float dist = dir.length();
            if (dist <= m_speed * dt) {
                m_pos = m_waypoints[m_wpIdx++];
            } else {
                m_pos += dir.normalized() * m_speed * dt;
            }
        }

        m_sprite.setPosition(m_pos);
    }

    void draw(sf::RenderWindow& window) { window.draw(m_sprite); }

    bool reachedEnd() const { return m_wpIdx >= (int)m_waypoints.size(); }
    sf::Vector2f getPosition() const { return m_pos; }

    sf::Vector2f getVelocity() const {
        if (m_wpIdx >= (int)m_waypoints.size()) {
            return {};
        }

        sf::Vector2f dir = m_waypoints[m_wpIdx] - m_pos;
        return dir.normalized() * m_speed;
    }

    sf::Vector2f getNextWaypoint() const {
        if (m_wpIdx >= (int)m_waypoints.size()) {
            return m_pos;
        }

        return m_waypoints[m_wpIdx];
    }

    void takeDamage(int dmg) { m_hp = std::max(0, m_hp - dmg); }
    bool isDead() const { return m_hp <= 0; }
    int getHp() const { return m_hp; }

  private:
    sf::Sprite m_sprite;
    std::vector<sf::Vector2f> m_waypoints;
    sf::Vector2f m_pos;
    int m_hp = 3;
    int m_wpIdx = 0;
    float m_speed = 150.f;
    int m_animFrame = 0;
    float m_animTimer = 0.f;
    float m_frameDuration = 0.1f;
};

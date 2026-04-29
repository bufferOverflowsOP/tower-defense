#pragma once
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <vector>

class Enemy {
  public:
    Enemy(const sf::Texture& runTexture, const sf::Texture& attackTexture,
          sf::Vector2f startPos, std::vector<sf::Vector2f> waypoints)
        : m_sprite(runTexture), m_runTexture(&runTexture), m_attackTexture(&attackTexture),
          m_waypoints(std::move(waypoints)), m_pos(startPos), m_startPos(startPos) {
        m_sprite.setTextureRect({{0, 0}, {192, 192}});
        m_sprite.setOrigin({96.f, 96.f});
    }

    void reset() {
        m_hp = 3;
        m_wpIdx = 0;
        m_pos = m_startPos;
        m_animFrame = 0;
        m_animTimer = 0.f;
        m_attacking = false;
        m_didAttack = false;
        m_sprite.setTexture(*m_runTexture);
        setFrame(0);
    }

    void update(float dt) {
        m_didAttack = false;
        if (isDead()) return;
        if (reachedEnd()) {
            if (!m_attacking) {
                m_attacking = true;
                m_animFrame = 0;
                m_animTimer = 0.f;
                m_sprite.setTexture(*m_attackTexture);
                setFrame(0);
            }

            m_animTimer += dt;
            if (m_animTimer >= m_attackFrameDuration) {
                m_animTimer -= m_attackFrameDuration;
                m_animFrame++;
                if (m_animFrame >= 4) {
                    m_animFrame = 0;
                    m_didAttack = true;
                }
                setFrame(m_animFrame);
            }

            m_sprite.setPosition(m_pos);
            return;
        }

        m_animTimer += dt;
        if (m_animTimer >= m_frameDuration) {
            m_animTimer -= m_frameDuration;
            m_animFrame = (m_animFrame + 1) % 6;
            setFrame(m_animFrame);
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

    void draw(sf::RenderWindow& window) {
        if (isDead()) return;
        window.draw(m_sprite);
    }

    bool reachedEnd() const { return m_wpIdx >= (int)m_waypoints.size(); }
    sf::Vector2f getPosition() const { return m_pos; }
    bool didAttack() const { return m_didAttack; }

    sf::Vector2f getVelocity() const {
        if (m_wpIdx >= (int)m_waypoints.size()) {
            return {};
        }

        sf::Vector2f dir = m_waypoints[m_wpIdx] - m_pos;
        return dir.normalized() * m_speed;
    }

    void takeDamage(int dmg) { m_hp = std::max(0, m_hp - dmg); }
    bool isDead() const { return m_hp <= 0; }

  private:
    void setFrame(int frame) { m_sprite.setTextureRect({{frame * 192, 0}, {192, 192}}); }

    sf::Sprite m_sprite;
    const sf::Texture* m_runTexture;
    const sf::Texture* m_attackTexture;
    std::vector<sf::Vector2f> m_waypoints;
    sf::Vector2f m_pos;
    sf::Vector2f m_startPos;
    int m_hp = 3;
    int m_wpIdx = 0;
    float m_speed = 150.f;
    int m_animFrame = 0;
    float m_animTimer = 0.f;
    float m_frameDuration = 0.1f;
    float m_attackFrameDuration = 0.12f;
    bool m_attacking = false;
    bool m_didAttack = false;
};

#pragma once
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <utility>
#include <vector>

class Enemy {
  public:
    Enemy(const sf::Texture& runTexture, const sf::Texture& attackTexture,
          sf::Vector2f startPos, std::vector<sf::Vector2f> waypoints)
        : m_sprite(runTexture), m_runTexture(&runTexture), m_attackTexture(&attackTexture),
          m_waypoints(std::move(waypoints)), m_pos(startPos), m_startPos(startPos) {
        setFrame(0);
        m_sprite.setOrigin({kFrameSize / 2.f, kFrameSize / 2.f});
    }

    void reset() {
        m_hp = kMaxHp;
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
            if (m_animTimer >= kAttackFrameDuration) {
                m_animTimer -= kAttackFrameDuration;
                m_animFrame++;
                if (m_animFrame >= kAttackFrames) {
                    m_animFrame = 0;
                    m_didAttack = true;
                }
                setFrame(m_animFrame);
            }

            m_sprite.setPosition(m_pos);
            return;
        }

        m_animTimer += dt;
        if (m_animTimer >= kRunFrameDuration) {
            m_animTimer -= kRunFrameDuration;
            m_animFrame = (m_animFrame + 1) % kRunFrames;
            setFrame(m_animFrame);
        }

        if (m_wpIdx < (int)m_waypoints.size()) {
            sf::Vector2f dir = m_waypoints[m_wpIdx] - m_pos;
            float dist = dir.length();
            if (dist <= kSpeed * dt) {
                m_pos = m_waypoints[m_wpIdx++];
            } else {
                m_pos += dir.normalized() * kSpeed * dt;
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
        return dir.normalized() * kSpeed;
    }

    void takeDamage(int dmg) { m_hp = std::max(0, m_hp - dmg); }
    bool isDead() const { return m_hp <= 0; }

  private:
    static constexpr int kFrameSize = 192;
    static constexpr int kRunFrames = 6;
    static constexpr int kAttackFrames = 4;
    static constexpr int kMaxHp = 3;
    static constexpr float kSpeed = 150.f;
    static constexpr float kRunFrameDuration = 0.1f;
    static constexpr float kAttackFrameDuration = 0.12f;

    void setFrame(int frame) {
        m_sprite.setTextureRect({{frame * kFrameSize, 0}, {kFrameSize, kFrameSize}});
    }

    sf::Sprite m_sprite;
    const sf::Texture* m_runTexture;
    const sf::Texture* m_attackTexture;
    std::vector<sf::Vector2f> m_waypoints;
    sf::Vector2f m_pos;
    sf::Vector2f m_startPos;
    int m_hp = kMaxHp;
    int m_wpIdx = 0;
    int m_animFrame = 0;
    float m_animTimer = 0.f;
    bool m_attacking = false;
    bool m_didAttack = false;
};

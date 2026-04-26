#pragma once
#include <SFML/Graphics.hpp>

class Tower {
  public:
    static void configure(sf::Sprite& s, const sf::Texture& t) {
        auto size = t.getSize();
        s.setOrigin({size.x / 2.f, float(size.y)});
        s.setScale({0.5f, 0.5f});
    }

    static void configureArcher(sf::Sprite& s, sf::Vector2f pos) {
        s.setTextureRect({{0, 0}, {192, 192}});
        s.setOrigin({96.f, 192.f});
        s.setScale({kScale, 0.6f});
        s.setPosition(pos - sf::Vector2f(0.f, 40.f));
    }

    Tower(const sf::Texture& texture, const sf::Texture& archerTexture,
          const sf::Texture& shootTexture, sf::Vector2f pos)
        : m_sprite(texture), m_archer(archerTexture), m_archerShoot(shootTexture) {
        configure(m_sprite, texture);
        m_sprite.setPosition(pos);
        configureArcher(m_archer, pos);
        configureArcher(m_archerShoot, pos);
    }

    void update(float dt, sf::Vector2f enemyPos, sf::Vector2f enemyVelocity) {
        m_didFire = false;

        m_cooldownTimer += dt;

        sf::Vector2f towerPos = m_sprite.getPosition();

        if ((enemyPos - towerPos).length() < m_range && m_cooldownTimer >= m_cooldown) {
            m_didFire = true;
            m_fireDirection = computeAimDirection(enemyPos, enemyVelocity,
                                                  m_archer.getPosition());
            m_cooldownTimer = 0.f;
            m_isShooting = true;
            m_shootTimer = 0.f;
        }

        if (m_isShooting) {
            m_shootTimer += dt;
            int newFrame = static_cast<int>(m_shootTimer / m_shootFrameDuration);
            if (newFrame >= 8) {
                m_isShooting = false;
            } else {
                m_archerShoot.setTextureRect({{newFrame * 192, 0}, {192, 192}});
            }
        }

        float dir = (enemyPos.x < towerPos.x) ? -1.f : 1.f;
        m_archer.setScale({dir * kScale, 0.6f});
        m_archerShoot.setScale({dir * kScale, 0.6f});
    }

    void draw(sf::RenderWindow& window) {
        window.draw(m_sprite);
        if (m_isShooting)
            window.draw(m_archerShoot);
        else
            window.draw(m_archer);
    }

    bool didFire() const { return m_didFire; }
    sf::Vector2f fireDirection() const { return m_fireDirection; }
    sf::Vector2f archerPosition() const { return m_archer.getPosition(); }

    static constexpr float kArrowSpeed = 400.f;

  private:
    static sf::Vector2f computeAimDirection(sf::Vector2f enemyPos, sf::Vector2f enemyVel,
                                            sf::Vector2f towerPos) {
        // TODO: this is a very naive implementation which assumes the enemy will keep moving in the same direction and speed. add quadratic formula solution for better aiming
        float t = (enemyPos - towerPos).length() / kArrowSpeed;
        return (enemyPos + enemyVel * t - towerPos).normalized();
    }

  private:
    static constexpr float kScale = 0.6f;

    sf::Sprite m_sprite;
    sf::Sprite m_archer;

    float m_range = 192.f;
    float m_cooldown = 1.5f;
    float m_cooldownTimer = 0.f;
    bool m_didFire = false;
    sf::Vector2f m_fireDirection;

    sf::Sprite m_archerShoot;
    bool m_isShooting = false;
    float m_shootTimer = 0.f;
    float m_shootFrameDuration = 0.08f;
};

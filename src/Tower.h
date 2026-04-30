#pragma once
#include <SFML/Graphics.hpp>

enum class TowerKind { Archer, Barracks };

class Tower {
  public:
    static void configure(sf::Sprite& s, const sf::Texture& t) {
        auto size = t.getSize();
        s.setOrigin({size.x / 2.f, float(size.y)});
        s.setScale({0.5f, 0.5f});
    }

    static void configureArcher(sf::Sprite& s, sf::Vector2f pos) {
        s.setTextureRect({{0, 0}, {kArcherFrameSize, kArcherFrameSize}});
        s.setOrigin({kArcherFrameSize / 2.f, float(kArcherFrameSize)});
        s.setScale({kArcherScale, kArcherScale});
        s.setPosition(pos - sf::Vector2f(0.f, kArcherYOffset));
    }

    Tower(TowerKind kind, const sf::Texture& texture, const sf::Texture& archerTexture,
          const sf::Texture& shootTexture, sf::Vector2f pos)
        : m_kind(kind), m_sprite(texture), m_archer(archerTexture), m_archerShoot(shootTexture) {
        configure(m_sprite, texture);
        m_sprite.setPosition(pos);
        configureArcher(m_archer, pos);
        configureArcher(m_archerShoot, pos);
    }

    void update(float dt) {
        m_didFire = false;
        m_cooldownTimer += dt;
        updateShootAnimation(dt);
    }

    void update(float dt, sf::Vector2f enemyPos, sf::Vector2f enemyVelocity) {
        m_didFire = false;

        m_cooldownTimer += dt;

        if (m_kind != TowerKind::Archer)
            return;

        sf::Vector2f towerPos = m_sprite.getPosition();

        if ((enemyPos - towerPos).length() < kRange && m_cooldownTimer >= kCooldown) {
            m_didFire = true;
            m_fireDirection = computeAimDirection(enemyPos, enemyVelocity, archerPosition());
            m_cooldownTimer = 0.f;
            m_isShooting = true;
            m_shootTimer = 0.f;
        }

        updateShootAnimation(dt);

        float dir = (enemyPos.x < towerPos.x) ? -1.f : 1.f;
        m_archer.setScale({dir * kArcherScale, kArcherScale});
        m_archerShoot.setScale({dir * kArcherScale, kArcherScale});
    }

    void draw(sf::RenderWindow& window) {
        window.draw(m_sprite);
        if (m_kind == TowerKind::Archer) {
            window.draw(m_isShooting ? m_archerShoot : m_archer);
        }
    }

    bool didFire() const {
        return m_didFire;
    }
    sf::Vector2f fireDirection() const {
        return m_fireDirection;
    }
    sf::Vector2f position() const {
        return m_sprite.getPosition();
    }
    TowerKind kind() const {
        return m_kind;
    }
    sf::Vector2f archerPosition() const {
        const sf::Sprite& activeArcher = m_isShooting ? m_archerShoot : m_archer;
        return activeArcher.getTransform().transformPoint({120.f, 100.f});
    }

    static constexpr float kArrowSpeed = 400.f;

  private:
    static sf::Vector2f computeAimDirection(sf::Vector2f enemyPos, sf::Vector2f enemyVel,
                                            sf::Vector2f shotPos) {
        // TODO: this is a very naive implementation which assumes the enemy will keep moving
        // in the same direction and speed. add quadratic formula solution for better aiming
        float t = (enemyPos - shotPos).length() / kArrowSpeed;
        return (enemyPos + enemyVel * t - shotPos).normalized();
    }

    void updateShootAnimation(float dt) {
        if (!m_isShooting)
            return;

        m_shootTimer += dt;
        int newFrame = static_cast<int>(m_shootTimer / kShootFrameDuration);
        if (newFrame >= kShootFrames) {
            m_isShooting = false;
        } else {
            m_archerShoot.setTextureRect(
                {{newFrame * kArcherFrameSize, 0}, {kArcherFrameSize, kArcherFrameSize}});
        }
    }

  private:
    static constexpr int kArcherFrameSize = 192;
    static constexpr int kShootFrames = 8;
    static constexpr float kArcherScale = 0.6f;
    static constexpr float kArcherYOffset = 40.f;
    static constexpr float kRange = 192.f;
    static constexpr float kCooldown = 1.5f;
    static constexpr float kShootFrameDuration = 0.08f;

    TowerKind m_kind;
    sf::Sprite m_sprite;
    sf::Sprite m_archer;

    float m_cooldownTimer = 0.f;
    bool m_didFire = false;
    sf::Vector2f m_fireDirection;

    sf::Sprite m_archerShoot;
    bool m_isShooting = false;
    float m_shootTimer = 0.f;
};

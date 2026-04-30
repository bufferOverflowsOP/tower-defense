#include "HpBar.h"
#include "GameConfig.h"

HpBar::HpBar(const sf::Texture& baseTexture, const sf::Texture& fillTexture)
    : m_left(baseTexture, {{40, 0}, {kCapWidthPx, kHeightPx}}),
      m_center(baseTexture, {{128, 0}, {kCenterWidthPx, kHeightPx}}),
      m_right(baseTexture, {{256, 0}, {kCapWidthPx, kHeightPx}}), m_fill(fillTexture) {
    m_left.setPosition(kPos);
    m_left.setScale({kScale, kScale});

    m_center.setPosition(kPos + sf::Vector2f{kCapWidth, 0.f});
    m_center.setScale({kCenterWidth / kCenterWidthPx, kScale});

    m_right.setPosition(kPos + sf::Vector2f{kCapWidth + kCenterWidth, 0.f});
    m_right.setScale({kScale, kScale});

    m_fill.setScale({1.f, kScale});
    m_fill.setPosition(kPos + sf::Vector2f{kFillInset, 0.f});
}

void HpBar::update(int hp) {
    float hpRatio = float(hp) / cfg::castleMaxHp;
    m_fill.setTextureRect({{0, 0}, {int(kFillWidth * hpRatio), kHeightPx}});
}

void HpBar::draw(sf::RenderWindow& window) const {
    window.draw(m_left);
    window.draw(m_center);
    window.draw(m_right);
    window.draw(m_fill);
}

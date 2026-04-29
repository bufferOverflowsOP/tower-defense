#pragma once
#include <SFML/Graphics.hpp>

class HpBar {
  public:
    HpBar(const sf::Texture& baseTexture, const sf::Texture& fillTexture);

    void update(int hp);
    void draw(sf::RenderWindow& window) const;

  private:
    static constexpr sf::Vector2f kPos{12.f, 12.f};
    static constexpr float kScale = 0.5f;
    static constexpr float kWidth = 160.f;
    static constexpr int kCapWidthPx = 24;
    static constexpr int kCenterWidthPx = 64;
    static constexpr int kHeightPx = 64;
    static constexpr float kCapWidth = kCapWidthPx * kScale;
    static constexpr float kCenterWidth = kWidth - kCapWidth * 2.f;
    static constexpr float kFillInset = 6.f;
    static constexpr float kFillWidth = kWidth - kFillInset * 2.f;

    sf::Sprite m_left;
    sf::Sprite m_center;
    sf::Sprite m_right;
    sf::Sprite m_fill;
};

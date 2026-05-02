#pragma once
#include <SFML/Graphics.hpp>

struct Assets {
    Assets();

    sf::Texture grass;
    sf::Texture path;
    sf::Texture castle;
    sf::Texture enemyRun;
    sf::Texture enemyAttack;
    sf::Texture tower;
    sf::Texture barracks;
    sf::Texture barracksGuardIdle;
    sf::Texture barracksGuardAttack;
    sf::Texture archer;
    sf::Texture archerShoot;
    sf::Texture arrow;
    sf::Texture hpBarBase;
    sf::Texture hpBarFill;
    sf::Texture goldIcon;
    sf::Texture shopCard;
    sf::Texture shopButtonSelected;
    sf::Font uiFont;
};

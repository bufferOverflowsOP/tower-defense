#include "Assets.h"

Assets::Assets() {
    grass.loadFromFile("assets/Terrain/Tileset/Tilemap_color1.png");
    path.loadFromFile("assets/Terrain/Tileset/Tilemap_color4.png");
    castle.loadFromFile("assets/Buildings/Red Buildings/Castle.png");
    enemyRun.loadFromFile("assets/Units/Black Units/Warrior/Warrior_Run.png");
    enemyAttack.loadFromFile("assets/Units/Black Units/Warrior/Warrior_Attack1.png");
    tower.loadFromFile("assets/Buildings/Red Buildings/Tower.png");
    barracks.loadFromFile("assets/Buildings/Red Buildings/Barracks.png");
    archer.loadFromFile("assets/Units/Red Units/Archer/Archer_Idle.png");
    archerShoot.loadFromFile("assets/Units/Red Units/Archer/Archer_Shoot.png");
    arrow.loadFromFile("assets/Units/Red Units/Archer/Arrow.png");
    hpBarBase.loadFromFile("assets/UI Elements/UI Elements/Bars/BigBar_Base.png");
    hpBarFill.loadFromFile("assets/UI Elements/UI Elements/Bars/BigBar_Fill.png");
    goldIcon.loadFromFile("assets/UI Elements/UI Elements/Icons/Icon_03.png");
    shopCard.loadFromFile(
        "assets/UI Elements/UI Elements/Buttons/SmallBlueSquareButton_Regular.png");
    shopButtonSelected.loadFromFile(
        "assets/UI Elements/UI Elements/Buttons/SmallRedSquareButton_Regular.png");
    uiFont.openFromFile("assets/Fonts/MedievalSharp-Regular.ttf");
    hpBarFill.setRepeated(true);
}

#pragma once
#include "UIScreen.h"

class HUD :
    public UIScreen
{
    class Texture* mCrosshair;
    class Texture* mCrosshairEnemy;
    bool mTargetEnemy;

    class Texture* mRadar;
    class Texture* mBlipTex;
    class Texture* mRadarArrow;
    std::vector<Vector2> mBlips;
    float mRadarRange;
    float mRadarRadius;

    std::vector<class TargetComponent*> mTargetComps;

    void UpdateCrossHair(float deltaTime);
    void UpdateRadar(float deltaTime);

public:
    HUD(class Game* game);

    void Update(float deltaTime) override;
    void Draw(class Shader* shader) override;

    void AddTarget(class TargetComponent* target);
    void RemoveTarget(class TargetComponent* target);
};


#pragma once
#include "Actor.h"
class Tower :
    public Actor
{
    const float AttackTime = 2.5f;
    const float AttackRange = 400.f;
    float mAttackTimer;
    class MoveComponent* mc;


public:
    Tower(class Game* game);

    void UpdateActor(float deltaTime) override;
};


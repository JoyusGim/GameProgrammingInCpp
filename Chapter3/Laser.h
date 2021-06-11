#pragma once
#include "Actor.h"
class Laser :
    public Actor
{
    float mLifetime;
    class CircleColliderComponent* mCollider;

public:
    Laser(class Game* game);

    void UpdateActor(float deltaTime) override;
};


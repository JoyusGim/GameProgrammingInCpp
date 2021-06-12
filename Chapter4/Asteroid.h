#pragma once
#include "Actor.h"
class Asteroid :
    public Actor
{
    class CircleColliderComponent* mCollider;

public:
    Asteroid(class Game* game);
    virtual ~Asteroid();

    void UpdateActor(float deltaTime) override;

    class CircleColliderComponent* GetCollider() const;
};


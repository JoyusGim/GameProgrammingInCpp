#pragma once
#include "Actor.h"
class Bullet :
    public Actor
{
    class Actor* mTarget;
    class MoveComponent* mc;
    float mLifeTime = 5.f;
    class CircleColliderComponent* collider;

public:
    Bullet(class Game* game);

    void UpdateActor(float deltaTime) override;

    void SetTarget(class Actor* target);

    class CircleColliderComponent* GetCollider() const { return collider; }
};


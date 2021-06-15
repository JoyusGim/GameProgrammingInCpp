#pragma once
#include "Actor.h"
#include <vector>

class Enemy :
    public Actor
{
    class CircleColliderComponent* collider;
    std::vector<class Bullet*> engaged;

public:
    Enemy(class Game* game);
    ~Enemy() override;

    void UpdateActor(float deltaTime) override;
    class CircleColliderComponent* GetCollider() const { return collider; };
    void AddEngage(class Bullet* bullet) { engaged.push_back(bullet); }
};


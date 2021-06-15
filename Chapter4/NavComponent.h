#pragma once
#include "MoveComponent.h"
#include "Math.h"

class NavComponent :
    public MoveComponent
{
    const class Tile* mNextNode;

public:
    NavComponent(class Actor* owner, int updateOrder = 10);

    void Update(float deltaTime);
    void TurnTo(const Math::Vector2& pos);
    void StartPath(const Tile* start);
};


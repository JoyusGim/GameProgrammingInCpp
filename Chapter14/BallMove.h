#pragma once
#include "MoveComponent.h"
class BallMove :
    public MoveComponent
{
    class Actor* mPlayer;

public:
    BallMove(class Actor* owner);

    void Update(float deltaTime) override;

    void SetPlayer(Actor* player) { mPlayer = player; }
    TypeID GetType() const override { return TypeID::BallMove; }
};


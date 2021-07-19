#pragma once
#include "Actor.h"
class FollowActor :
    public Actor
{
    class MoveComponent* mMoveComp;
    class FollowCamera* mCameraComp;
    class SkeletalMeshComponent* mMeshComp;
    bool mMoving;

public:
    FollowActor(class Game* game);

    void ActorInput(const struct InputState& input) override;
    void UpdateActor(float deltaTime) override;

    TypeID GetType() const override { return Actor::FOLLOWACTOR; }
};


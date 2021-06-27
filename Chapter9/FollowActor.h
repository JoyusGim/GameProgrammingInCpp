#pragma once
#include "Actor.h"
class FollowActor :
    public Actor
{
    class MoveComponent* mMoveComp;
    class FollowCamera* mCameraComp;
    class MeshComponent* mMeshComp;

public:
    FollowActor(class Game* game);

    void ActorInput(const struct InputState& input) override;
    void UpdateActor(float deltaTime) override;
};


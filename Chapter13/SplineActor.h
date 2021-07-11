#pragma once
#include "Actor.h"
class SplineActor :
    public Actor
{
    class MoveComponent* mMoveComp;
    class SplineCamera* mCameraComp;
    class MeshComponent* mMeshComp;

public:
    SplineActor(class Game* game);

    void ActorInput(const struct InputState& input) override;
    void UpdateActor(float deltaTime) override;
};


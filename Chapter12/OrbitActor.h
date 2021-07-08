#pragma once
#include "Actor.h"
class OrbitActor :
    public Actor
{
    class MoveComponent* mMoveComp;
    class OrbitCamera* mCameraComp;
    class MeshComponent* mMeshComp;

public:
    OrbitActor(class Game* game);

    void ActorInput(const struct InputState& input) override;
    void UpdateActor(float deltaTime) override;
};


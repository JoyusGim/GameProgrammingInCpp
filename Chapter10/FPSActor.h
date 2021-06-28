#pragma once
#include "Actor.h"
#include "SoundEvent.h"

class FPSActor :
    public Actor
{
    class MoveComponent* mMoveComp;
    class AudioComponent* mAudioComp;
    class FPSCamera* mCameraComp;
    Actor* mModelActor;
    class MeshComponent* mMeshComp;

    SoundEvent mFootstep;
    float mLastFootstep;

public:
    FPSActor(class Game* game);
    void ActorInput(const struct InputState& keyState) override;
    void UpdateActor(float deltaTime) override;
};


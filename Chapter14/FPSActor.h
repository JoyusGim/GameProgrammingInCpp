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
    class BoxComponent* mBoxComp;

    SoundEvent mFootstep;
    float mLastFootstep;



public:
    FPSActor(class Game* game);
    void ActorInput(const struct InputState& keyState) override;
    void UpdateActor(float deltaTime) override;

    void FixCollisions();

    void Shoot();

    TypeID GetType() const override { return Actor::FPSACTOR; }
};


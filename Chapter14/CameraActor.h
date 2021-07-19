#pragma once
#include "Actor.h"

class CameraActor : public Actor
{
	class MoveComponent* mMoveComp;
	class AudioComponent* mAudioComp;

public:
	CameraActor(class Game* game);

	void UpdateActor(float deltaTime) override;
	void ActorInput(const struct InputState& keystate) override;

	TypeID GetType() const override { return Actor::CAMERAACTOR; }
};


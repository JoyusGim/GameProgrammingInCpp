#pragma once
#include "Actor.h"

class CameraActor : public Actor
{
	class MoveComponent* mMoveComp;
	class AudioComponent* mAudioComp;

public:
	CameraActor(class Game* game);

	void UpdateActor(float deltaTime) override;
	void ActorInput(const uint8_t* keystate) override;
};


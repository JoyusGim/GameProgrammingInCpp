#pragma once
#include "Actor.h"
class BallActor :
    public Actor
{
	class AudioComponent* mAudioComp;
	class BallMove* mMyMove;
	float mLifeSpan;

public:
	BallActor(class Game* game);

	void UpdateActor(float deltaTime) override;
	void SetPlayer(Actor* player);

	void HitTarget();

	TypeID GetType() const override { return Actor::BALLACTOR; }
};


#pragma once
#include "Actor.h"
class Ship : public Actor
{
	float mRightSpeed;
	float mDownSpeed;

public:
	Ship(class Game* game);

	void UpdateActor(float deltaTime) override;
	void ProcessKeyboard(const uint8_t* state);

	float GetRightSpeed();
	float GetDownSpeed();
};


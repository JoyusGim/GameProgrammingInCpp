#pragma once
#include "Component.h"

class MoveComponent : public Component
{
	float mAngularSpeed;
	float mForwardSpeed;

public:
	MoveComponent(class Actor* actor, int updateOrder = 10);

	void Update(float deltaTime) override;

	float GetAngularSpeed() const;
	float GetForwardSpeed() const;
	void SetAngularSpeed(float speed);
	void SetForwardSpeed(float speed);
};


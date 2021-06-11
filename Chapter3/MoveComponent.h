#pragma once
#include "Component.h"
#include "Math.h"

class MoveComponent : public Component
{
	float mMass;
	float mAngularSpeed;
	Math::Vector2 mVelocity;
	Math::Vector2 mAccumForce;

public:
	MoveComponent(class Actor* actor, int updateOrder = 10);

	void Update(float deltaTime) override;

	float GetAngularSpeed() const;
	void SetAngularSpeed(float speed);
	float GetMass() const;
	void SetMass(float mass);

	void AddForce(const Math::Vector2& force);
};


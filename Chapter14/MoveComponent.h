#pragma once
#include "Component.h"
#include "Math.h"

class MoveComponent : public Component
{
	float mMass;
	float mAngularSpeed;
	float mForwardSpeed;
	float mStrafeSpeed;
	Vector3 mVelocity;
	Vector3 mAccumForce;

public:
	MoveComponent(class Actor* actor, int updateOrder = 10);

	void Update(float deltaTime) override;
	void LoadProperties(const rapidjson::Value& inObj) override;
	void SaveProperties(rapidjson::Document::AllocatorType& alloc, rapidjson::Value& inObj) const override;

	float GetAngularSpeed() const;
	void SetAngularSpeed(float speed);
	float GetForwardSpeed() const;
	void SetForwardSpeed(float speed);
	float GetStrafeSpeed() const;
	void SetStrafeSpeed(float speed);
	float GetMass() const;
	void SetMass(float mass);

	void AddForce(const Vector3& force);
	TypeID GetType() const override { return TypeID::MoveComponent; }
};


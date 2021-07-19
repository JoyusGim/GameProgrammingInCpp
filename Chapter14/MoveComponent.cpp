#include "MoveComponent.h"
#include "Actor.h"
#include "Math.h"

MoveComponent::MoveComponent(Actor* actor, int updateOrder)	:
	Component(actor, updateOrder),
	mMass{ 0.f },
	mAngularSpeed{ 0.f },
	mForwardSpeed{ 0.f }
{
}

void MoveComponent::Update(float deltaTime)
{
	if (!Math::NearZero(mForwardSpeed) || !Math::NearZero(mStrafeSpeed))
	{
		Vector3 pos = mOwner->GetPosition();
		pos += mOwner->GetForward() * mForwardSpeed * deltaTime;
		pos += mOwner->GetRight() * mStrafeSpeed * deltaTime;
		mOwner->SetPosition(pos);
	}
	
	if (!Math::NearZero(mAngularSpeed))
	{
		Quaternion rot = mOwner->GetRotate();
		float angle = mAngularSpeed * deltaTime;

		Quaternion inc(Vector3::UnitZ, angle);
		rot = Quaternion::Concatenate(rot, inc);
		mOwner->SetRotate(rot);
	}
	/*
	if (!Math::NearZero(mForwardSpeed))
	{
		Vector3 pos = mOwner->GetPosition();
		pos += mOwner->GetForward() * mForwardSpeed * deltaTime;
		mOwner->SetPosition(pos);
	}
	*/
	/*
	// 질량이 0과 가까우면 계산을 피한다.
	if (!Math::NearZero(mMass))
	{
		Vector3 acceleration = mAccumForce / mMass;
		mVelocity += acceleration * deltaTime;

		Vector3 position = mOwner->GetPosition();
		position += mVelocity * deltaTime;

		mOwner->SetPosition(position);
		mAccumForce = Vector3::Zero;
	}
	*/
}

void MoveComponent::LoadProperties(const rapidjson::Value& inObj)
{
	Component::LoadProperties(inObj);

	JsonHelper::Get<float>(inObj, "angularSpeed", mAngularSpeed);
	JsonHelper::Get<float>(inObj, "forwardSpeed", mForwardSpeed);
	JsonHelper::Get<float>(inObj, "strafeSpeed", mStrafeSpeed);
}

void MoveComponent::SaveProperties(rapidjson::Document::AllocatorType& alloc, rapidjson::Value& inObj) const
{
	Component::SaveProperties(alloc, inObj);

	JsonHelper::Add<float>(alloc, inObj, "angularSpeed", mAngularSpeed);
	JsonHelper::Add<float>(alloc, inObj, "forwardSpeed", mForwardSpeed);
	JsonHelper::Add<float>(alloc, inObj, "strafeSpeed", mStrafeSpeed);
}

float MoveComponent::GetAngularSpeed() const
{
	return mAngularSpeed;
}

void MoveComponent::SetAngularSpeed(float speed)
{
	mAngularSpeed = speed;
}

float MoveComponent::GetForwardSpeed() const
{
	return mForwardSpeed;
}

void MoveComponent::SetForwardSpeed(float speed)
{
	mForwardSpeed = speed;
}

float MoveComponent::GetStrafeSpeed() const
{
	return mStrafeSpeed;
}

void MoveComponent::SetStrafeSpeed(float speed)
{
	mStrafeSpeed = speed;
}

float MoveComponent::GetMass() const
{
	return mMass;
}

void MoveComponent::SetMass(float mass)
{
	mMass = mass;
}

void MoveComponent::AddForce(const Vector3& force)
{
	mAccumForce += force;
}

#include "MoveComponent.h"
#include "Actor.h"
#include "Math.h"

MoveComponent::MoveComponent(Actor* actor, int updateOrder)	:
	Component(actor, updateOrder),
	mMass{ 0.f },
	mAngularSpeed{ 0.f }
{
}

void MoveComponent::Update(float deltaTime)
{
	if (!Math::NearZero(mAngularSpeed))
	{
		Quaternion rot = mOwner->GetRotate();
		float angle = mAngularSpeed * deltaTime;

		Quaternion inc(Vector3::UnitZ, angle);
		rot = Quaternion::Concatenate(rot, inc);
		mOwner->SetRotate(rot);
	}

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
}

float MoveComponent::GetAngularSpeed() const
{
	return mAngularSpeed;
}

void MoveComponent::SetAngularSpeed(float speed)
{
	mAngularSpeed = speed;
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

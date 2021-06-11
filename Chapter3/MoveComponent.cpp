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
		float rot = mOwner->GetRotate();
		rot += mAngularSpeed * deltaTime;
		mOwner->SetRotate(rot);
	}

	// ������ 0�� ������ ����� ���Ѵ�.
	if (!Math::NearZero(mMass))
	{
		Math::Vector2 acceleration = mAccumForce / mMass;
		mVelocity += acceleration * deltaTime;

		Math::Vector2 position = mOwner->GetPosition();
		position += mVelocity * deltaTime;

		mOwner->SetPosition(position);
		mAccumForce = Math::Vector2::Zero;
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

void MoveComponent::AddForce(const Math::Vector2& force)
{
	mAccumForce += force;
}

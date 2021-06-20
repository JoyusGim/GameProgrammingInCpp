#include "MoveComponent.h"
#include "Actor.h"
#include "Math.h"

MoveComponent::MoveComponent(Actor* actor, int updateOrder)	:
	Component(actor, updateOrder),
	mAngularSpeed{ 0.f },
	mForwardSpeed{ 0.f }
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

	if (!Math::NearZero(mForwardSpeed))
	{
		Vector2 pos = mOwner->GetPosition();
		pos += mOwner->GetForward() * mForwardSpeed * deltaTime;
		mOwner->SetPosition(pos);
	}
}

float MoveComponent::GetAngularSpeed() const
{
	return mAngularSpeed;
}

float MoveComponent::GetForwardSpeed() const
{
	return mForwardSpeed;
}

void MoveComponent::SetAngularSpeed(float speed)
{
	mAngularSpeed = speed;
}

void MoveComponent::SetForwardSpeed(float speed)
{
	mForwardSpeed = speed;
}

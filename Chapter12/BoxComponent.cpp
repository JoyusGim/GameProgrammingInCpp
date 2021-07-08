#include "BoxComponent.h"
#include "Actor.h"
#include "Game.h"
#include "PhysWorld.h"

BoxComponent::BoxComponent(Actor* owner)	:
	Component(owner),
	mObjectBox{ Vector3::Zero, Vector3::Zero },
	mWorldBox{ Vector3::Zero, Vector3::Zero },
	mShouldRotate{ true }
{
	mOwner->GetGame()->GetPhysWorld()->AddBox(this);
}

BoxComponent::~BoxComponent()
{
	mOwner->GetGame()->GetPhysWorld()->RemoveBox(this);
}

void BoxComponent::OnUpdateWorldTransform()
{
	mWorldBox = mObjectBox;
	mWorldBox.mMin *= mOwner->GetScale();
	mWorldBox.mMax *= mOwner->GetScale();

	if (mShouldRotate)
	{
		mWorldBox.Rotate(mOwner->GetRotate());
	}
	mWorldBox.mMin += mOwner->GetPosition();
	mWorldBox.mMax += mOwner->GetPosition();
}

void BoxComponent::SetObjectBox(const AABB& model)
{
	mObjectBox = model;
}

const AABB& BoxComponent::GetWorldBox() const
{
	return mWorldBox;
}

void BoxComponent::SetShouldRotate(bool value)
{
	mShouldRotate = value;
}

Actor* BoxComponent::GetOwner() const
{
	return mOwner;
}

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

void BoxComponent::LoadProperties(const rapidjson::Value& inObj)
{
	Component::LoadProperties(inObj);

	JsonHelper::Get<Vector3>(inObj, "objectMin", mObjectBox.mMin);
	JsonHelper::Get<Vector3>(inObj, "objectMax", mObjectBox.mMax);
	JsonHelper::Get<Vector3>(inObj, "worldMin", mWorldBox.mMin);
	JsonHelper::Get<Vector3>(inObj, "worldMax", mWorldBox.mMax);
	JsonHelper::Get<bool>(inObj, "shouldRotate", mShouldRotate);
}

void BoxComponent::SaveProperties(rapidjson::Document::AllocatorType& alloc, rapidjson::Value& inObj) const
{
	Component::SaveProperties(alloc, inObj);

	JsonHelper::Add<Vector3>(alloc, inObj, "objectMin", mObjectBox.mMin);
	JsonHelper::Add<Vector3>(alloc, inObj, "objectMax", mObjectBox.mMax);
	JsonHelper::Add<Vector3>(alloc, inObj, "worldMin", mWorldBox.mMin);
	JsonHelper::Add<Vector3>(alloc, inObj, "worldMax", mWorldBox.mMax);
	JsonHelper::Add<bool>(alloc, inObj, "shouldRotate", mShouldRotate);
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

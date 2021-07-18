#include "Component.h"
#include "Actor.h"

Component::Component(Actor* owner, int updateOrder)	:
	mOwner{ owner },
	mUpdateOrder{ updateOrder }
{
	mOwner->AddComponent(this);
}

Component::~Component()
{
	mOwner->RemoveComponent(this);
}

int Component::GetUpdateOrder() const
{
	return mUpdateOrder;
}

Actor* Component::GetOwner() const
{
	return mOwner;
}

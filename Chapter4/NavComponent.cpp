#include "NavComponent.h"
#include "Tile.h"

NavComponent::NavComponent(class Actor* owner, int updateOrder) :
	MoveComponent(owner, updateOrder),
	mNextNode{ nullptr }
{

}

void NavComponent::Update(float deltaTime)
{
	if (mNextNode)
	{
		Math::Vector2 diff = mOwner->GetPosition() - mNextNode->GetPosition();
		if (Math::NearZero(diff.Length(), 2.f))
		{
			mNextNode = mNextNode->GetParent();
			TurnTo(mNextNode->GetPosition());
		}
	}
	MoveComponent::Update(deltaTime);
}

void NavComponent::TurnTo(const Math::Vector2& pos)
{
	Math::Vector2 dir = pos - mOwner->GetPosition();
	float angle = atan2f(-dir.y, dir.x);
	mOwner->SetRotate(angle);
}

void NavComponent::StartPath(const Tile* start)
{
	mNextNode = start->GetParent();
	TurnTo(mNextNode->GetPosition());
}

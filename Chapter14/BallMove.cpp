#include "BallMove.h"
#include "Game.h"
#include "Actor.h"
#include "Collision.h"
#include "PhysWorld.h"
#include "TargetActor.h"
#include "BallActor.h"

BallMove::BallMove(Actor* owner)	:
	MoveComponent(owner),
	mPlayer{ nullptr }
{
}

void BallMove::Update(float deltaTime)
{
	const float segmentLength = 30.f;
	Vector3 start = mOwner->GetPosition();
	Vector3 dir = mOwner->GetForward();
	Vector3 end = start + dir * segmentLength;
	LineSegment ls(start, end);

	PhysWorld* phys = mOwner->GetGame()->GetPhysWorld();
	PhysWorld::CollisionInfo info;
	if (phys->SegmentCast(ls, info) && info.mActor != mPlayer)
	{
		dir = Vector3::Reflect(dir, info.mNormal);
		mOwner->RotateToNewForward(dir);

		TargetActor* target = dynamic_cast<TargetActor*>(info.mActor);
		if (target)
		{
			static_cast<BallActor*>(mOwner)->HitTarget();
		}
	}

	MoveComponent::Update(deltaTime);

}

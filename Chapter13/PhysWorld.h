#pragma once
#include <vector>
#include "Collision.h"
#include <functional>

class PhysWorld
{
	class Game* mGame;
	std::vector<class BoxComponent*> mBoxes;
	
public:
	struct CollisionInfo
	{
		Vector3 mPoint;
		Vector3 mNormal;
		class BoxComponent* mBox = nullptr;
		class Actor* mActor = nullptr;
	};

public:
	PhysWorld(class Game* game);

	void AddBox(class BoxComponent* box);
	void RemoveBox(class BoxComponent* box);

	bool SegmentCast(const LineSegment& l, CollisionInfo& outColl);
	void TestPairwise(std::function<void(Actor*, Actor*)> f);
	void TestSweepAndPrune(std::function<void(Actor*, Actor*)> f);
};


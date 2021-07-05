#include "PhysWorld.h"
#include "BoxComponent.h"
#include <algorithm>

PhysWorld::PhysWorld(Game* game)    :
    mGame{ game }
{
}

void PhysWorld::AddBox(BoxComponent* box)
{
    mBoxes.emplace_back(box);
}

void PhysWorld::RemoveBox(BoxComponent* box)
{
    auto iter = std::find(mBoxes.begin(), mBoxes.end(), box);
    if (iter != mBoxes.end())
    {
        // Swap to end of vector and pop off (avoid erase copies)
        std::iter_swap(iter, mBoxes.end() - 1);
        mBoxes.pop_back();
    }
}

bool PhysWorld::SegmentCast(const LineSegment& l, CollisionInfo& outColl)
{
    bool collided = false;
    float closestT = Math::Infinity;
    Vector3 norm;

    for (auto box : mBoxes)
    {
        float t;

        if (Intersect(l, box->GetWorldBox(), t, norm))
        {
            if (t < closestT)
            {
                closestT = t;
                outColl.mPoint = l.PointOnSegment(t);
                outColl.mNormal = norm;
                outColl.mBox = box;
                outColl.mActor = box->GetOwner();
                collided = true;
            }
        }
    }

    return collided;
}

void PhysWorld::TestPairwise(std::function<void(Actor*, Actor*)> f)
{
    for (size_t i = 0; i < mBoxes.size(); i++)
    {
        for (size_t j = i + 1; j < mBoxes.size(); j++)
        {
            BoxComponent* a = mBoxes[i];
            BoxComponent* b = mBoxes[j];
            if (Intersect(a->GetWorldBox(), b->GetWorldBox()))
            {
                f(a->GetOwner(), b->GetOwner());
            }
        }
    }
}

void PhysWorld::TestSweepAndPrune(std::function<void(Actor*, Actor*)> f)
{
    std::sort(mBoxes.begin(), mBoxes.end(),
        [](BoxComponent* a, BoxComponent* b)
        {
            return a->GetWorldBox().mMin.x <
                b->GetWorldBox().mMin.x;
        }
    );

    for (size_t i = 0; i < mBoxes.size(); i++)
    {
        BoxComponent* a = mBoxes[i];
        float max = a->GetWorldBox().mMax.x;
        for (size_t j = i + 1; j < mBoxes.size(); j++)
        {
            BoxComponent* b = mBoxes[j];
            if (b->GetWorldBox().mMin.x > max)
                break;
            else if (Intersect(a->GetWorldBox(), b->GetWorldBox()))
            {
                f(a->GetOwner(), b->GetOwner());
            }
        }
    }
}

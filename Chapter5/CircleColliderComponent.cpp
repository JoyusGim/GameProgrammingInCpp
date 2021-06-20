#include "CircleColliderComponent.h"
#include "Actor.h"

CircleColliderComponent::CircleColliderComponent(Actor* owner) :
    Component(owner),
    mRadius{ 0.f }
{
}

void CircleColliderComponent::SetRadius(float radius)
{
    mRadius = radius;
}

float CircleColliderComponent::GetRadius() const
{
    return mOwner->GetScale() * mRadius;
}

const Vector2& CircleColliderComponent::GetCenter() const
{
    return mOwner->GetPosition();
}

bool Intersect(const CircleColliderComponent& a, const CircleColliderComponent& b)
{
    Vector2 diff = a.GetCenter() - b.GetCenter();
    float squareDiff = diff.SquareLength();

    float squareRadius = a.GetRadius() + b.GetRadius();
    squareRadius *= squareRadius;

    return squareDiff <= squareRadius;
}

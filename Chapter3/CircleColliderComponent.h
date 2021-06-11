#pragma once
#include "Component.h"
#include "Math.h"

class CircleColliderComponent :
    public Component
{
    float mRadius;

public:
    CircleColliderComponent(class Actor* owner);

    void SetRadius(float radius);
    float GetRadius() const;

    const Math::Vector2& GetCenter() const;
};
bool Intersect(const CircleColliderComponent& a, const CircleColliderComponent& b);


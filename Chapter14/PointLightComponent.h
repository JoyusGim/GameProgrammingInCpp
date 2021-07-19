#pragma once
#include "Component.h"
#include "Math.h"

class PointLightComponent :
    public Component
{
    Vector3 mDiffuseColor;
    Vector3 mSpecColor;
    float mInnerRadius;
    float mOuterRadius;

public:
    PointLightComponent(class Actor* owner);
    ~PointLightComponent();

    void Draw(class Shader* shader, class Mesh* mesh);

    void SetDiffuseColor(const Vector3& color);
    void SetSpecColor(const Vector3& color);
    void SetInnerRadius(float radius);
    void SetOuterRadius(float radius);
    TypeID GetType() const override { return TypeID::PointLightComponent; }
};


#pragma once
#include "Component.h"
class CameraComponent :
    public Component
{
protected:
    void SetViewMatrix(const class Matrix4& view);

public:
    CameraComponent(class Actor* owner);
    TypeID GetType() const override { return TypeID::CameraComponent; }
};


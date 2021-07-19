#pragma once
#include "CameraComponent.h"
#include "Math.h"

class MirrorCamera :
    public CameraComponent
{
    float mHorzDist;
    float mVertDist;
    float mTargetDist;

    Vector3 ComputeCameraPos() const;

public:
    MirrorCamera(class Actor* owner);

    void Update(float deltaTime) override;

    void SetHorzDist(float dist);
    void SetVertDist(float dist);
    void SetTargetDist(float dist);

    void SnapToIdeal();
    TypeID GetType() const override { return TypeID::MirrorCamera; }
};


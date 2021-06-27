#pragma once
#include "CameraComponent.h"
#include "Math.h"

class OrbitCamera :
    public CameraComponent
{
    Vector3 mOffset;
    Vector3 mUp;
    float mPitchSpeed;
    float mYawSpeed;

public:
    OrbitCamera(class Actor* owner);

    void ProcessInput(const struct InputState& input) override;
    void Update(float deltaTime) override;
};


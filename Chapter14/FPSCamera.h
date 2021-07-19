#pragma once
#include "CameraComponent.h"
class FPSCamera :
    public CameraComponent
{
    float mPitchSpeed;
    float mMaxPitch;
    float mPitch;

public:
    FPSCamera(class Actor* owner);

    void Update(float deltaTime) override;

    void SetPitchSpeed(float speed);
    float GetPitchSpeed() const;
    void SetMaxPitch(float degree);
    float GetMaxPitch() const;
    void SetPitch(float pitch);
    float GetPitch() const;
    TypeID GetType() const override { return TypeID::FPSCamera; }
};


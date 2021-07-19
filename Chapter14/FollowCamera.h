#pragma once
#include "CameraComponent.h"
#include "Math.h"

class FollowCamera :
    public CameraComponent
{
    float mHorzDist;
    float mVertDist;
    float mTargetDist;
    
    float mSpringConstant;
    Vector3 mActualPos;
    Vector3 mVelocity;

    Vector3 ComputeCameraPos() const;

public:
    FollowCamera(class Actor* owner);

    void Update(float deltaTime) override;
    void LoadProperties(const rapidjson::Value& inObj) override;
    void SaveProperties(rapidjson::Document::AllocatorType& alloc, rapidjson::Value& inObj) const override;

    void SnapToIdeal();
    TypeID GetType() const override { return TypeID::FollowCamera; }
};


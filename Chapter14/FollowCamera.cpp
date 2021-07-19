#include "FollowCamera.h"
#include "Actor.h"
#include "Game.h"

Vector3 FollowCamera::ComputeCameraPos() const
{
    Vector3 cameraPos = mOwner->GetPosition();
    cameraPos -= mOwner->GetForward() * mHorzDist;
    cameraPos += Vector3::UnitZ * mVertDist;
    return cameraPos;
}

FollowCamera::FollowCamera(Actor* owner)    :
    CameraComponent(owner),
    mHorzDist{ 350.f },
    mVertDist{ 250.f },
    mTargetDist{ 100.f },
    mSpringConstant{ 64.f }
{
}

void FollowCamera::Update(float deltaTime)
{
    CameraComponent::Update(deltaTime);

    float damping = 2.0f * Math::Sqrt(mSpringConstant);
    Vector3 idealPos = ComputeCameraPos();

    Vector3 diff = mActualPos - idealPos;
    Vector3 acel = -mSpringConstant * diff - damping * mVelocity;

    mVelocity += acel * deltaTime;
    mActualPos += mVelocity * deltaTime;

    Vector3 target = mOwner->GetPosition() + mOwner->GetForward() * mTargetDist;
    
    Matrix4 view = Matrix4::CreateLookAt(mActualPos, target, Vector3::UnitZ);
    SetViewMatrix(view);
}

void FollowCamera::LoadProperties(const rapidjson::Value& inObj)
{
    CameraComponent::LoadProperties(inObj);

    JsonHelper::Get<Vector3>(inObj, "actualPos", mActualPos);
    JsonHelper::Get<Vector3>(inObj, "velocity", mVelocity);
    JsonHelper::Get<float>(inObj, "horzDist", mHorzDist);
    JsonHelper::Get<float>(inObj, "vertDist", mVertDist);
    JsonHelper::Get<float>(inObj, "targetDist", mTargetDist);
    JsonHelper::Get<float>(inObj, "springConstant", mSpringConstant);
}

void FollowCamera::SaveProperties(rapidjson::Document::AllocatorType& alloc, rapidjson::Value& inObj) const
{
    CameraComponent::SaveProperties(alloc, inObj);

    JsonHelper::Add<Vector3>(alloc, inObj, "actualPos", mActualPos);
    JsonHelper::Add<Vector3>(alloc, inObj, "velocity", mVelocity);
    JsonHelper::Add<float>(alloc, inObj, "horzDist", mHorzDist);
    JsonHelper::Add<float>(alloc, inObj, "vertDist", mVertDist);
    JsonHelper::Add<float>(alloc, inObj, "targetDist", mTargetDist);
    JsonHelper::Add<float>(alloc, inObj, "springConstant", mSpringConstant);
}

void FollowCamera::SnapToIdeal()
{
    mActualPos = ComputeCameraPos();
    mVelocity = Vector3::Zero;
    Vector3 target = mOwner->GetPosition() + mOwner->GetForward() * mTargetDist;
    Matrix4 view = Matrix4::CreateLookAt(mActualPos, target, Vector3::UnitZ);
    SetViewMatrix(view);
}

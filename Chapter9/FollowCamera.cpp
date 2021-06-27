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
    mVertDist{ 150.f },
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

void FollowCamera::SnapToIdeal()
{
    mActualPos = ComputeCameraPos();
    mVelocity = Vector3::Zero;
    Vector3 target = mOwner->GetPosition() + mOwner->GetForward() * mTargetDist;
    Matrix4 view = Matrix4::CreateLookAt(mActualPos, target, Vector3::UnitZ);
    SetViewMatrix(view);
}

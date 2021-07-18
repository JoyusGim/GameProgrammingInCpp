#include "MirrorCamera.h"
#include "Game.h"
#include "Actor.h"
#include "Renderer.h"

Vector3 MirrorCamera::ComputeCameraPos() const
{
    Vector3 cameraPos = mOwner->GetPosition();
    cameraPos += mOwner->GetForward() * mHorzDist;
    cameraPos += Vector3::UnitZ * mVertDist;
    return cameraPos;
}

MirrorCamera::MirrorCamera(Actor* owner)	:
	CameraComponent(owner),
    mHorzDist{ 150.f },
    mVertDist{ 200.f },
    mTargetDist{ 400.f }
{
}

void MirrorCamera::Update(float deltaTime)
{
	CameraComponent::Update(deltaTime);

    Vector3 idealPos = ComputeCameraPos();

    Vector3 target = mOwner->GetPosition() - mOwner->GetForward() * mTargetDist;

    Matrix4 view = Matrix4::CreateLookAt(idealPos, target, Vector3::UnitZ);
    
    mOwner->GetGame()->GetRenderer()->SetMirrorViewMatrix(view);
}

void MirrorCamera::SetHorzDist(float dist)
{
    mHorzDist = dist;
}

void MirrorCamera::SetVertDist(float dist)
{
    mVertDist = dist;
}

void MirrorCamera::SetTargetDist(float dist)
{
    mTargetDist = dist;
}

void MirrorCamera::SnapToIdeal()
{
    Vector3 idealPos = ComputeCameraPos();

    Vector3 target = mOwner->GetPosition() - mOwner->GetForward() * mTargetDist;

    Matrix4 view = Matrix4::CreateLookAt(idealPos, target, Vector3::UnitZ);

    mOwner->GetGame()->GetRenderer()->SetMirrorViewMatrix(view);
}

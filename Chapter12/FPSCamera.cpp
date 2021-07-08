#include "FPSCamera.h"
#include "Math.h"
#include "Actor.h"

FPSCamera::FPSCamera(Actor* owner)	:
	CameraComponent(owner),
	mPitchSpeed{ 0.f },
	mMaxPitch{ Math::Pi / 3.f },
	mPitch{ 0.f }
{
}

void FPSCamera::Update(float deltaTime)
{
	CameraComponent::Update(deltaTime);

	Vector3 cameraPos = mOwner->GetPosition();

	mPitch += mPitchSpeed * deltaTime;
	mPitch = Math::Clamp(mPitch, -mMaxPitch, mMaxPitch);
	Quaternion q(mOwner->GetRight(), mPitch);
	Vector3 viewForward = Vector3::Transform(
		mOwner->GetForward(), q
	);
		
	Vector3 target = cameraPos + viewForward * 100.f;
	Vector3 up = Vector3::Transform(Vector3::UnitZ, q);

	Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, up);
	SetViewMatrix(view);	
}

void FPSCamera::SetPitchSpeed(float speed)
{
	mPitchSpeed = speed;
}

float FPSCamera::GetPitchSpeed() const
{
	return mPitchSpeed;
}

void FPSCamera::SetMaxPitch(float degree)
{
	mMaxPitch = Math::ToRadians(degree);
}

float FPSCamera::GetMaxPitch() const
{
	return mMaxPitch;
}

void FPSCamera::SetPitch(float pitch)
{
	mPitch = pitch;
}

float FPSCamera::GetPitch() const
{
	return mPitch;
}
#include "OrbitCamera.h"
#include "Actor.h"
#include "InputSystem.h"
#include "Game.h"

OrbitCamera::OrbitCamera(Actor* owner) :
	CameraComponent(owner),
	mOffset{ -400.0f, 0.0f, 0.0f },
	mUp{ Vector3::UnitZ },
	mPitchSpeed{ 0.f },
	mYawSpeed{ 0.f }
{
}

void OrbitCamera::ProcessInput(const InputState& input)
{
	const float maxMouseSpeed = 500.f;
	const float maxOrbitSpeed = Math::Pi * 8;

	mPitchSpeed = 0.f;
	mYawSpeed = 0.f;

	if (input.Mouse.GetButtonState(SDL_BUTTON_LEFT) == Held)
	{
		Vector2 mousePos = input.Mouse.GetMousePos();
		float angularSpeed = 0.f;
		if (mousePos.x != 0.f)
		{
			angularSpeed = mousePos.x / maxMouseSpeed;
			angularSpeed *= maxOrbitSpeed;
		}
		mYawSpeed = -angularSpeed;

		float pitchSpeed = 0.f;

		if (mousePos.y != 0.f)
		{
			pitchSpeed = mousePos.y / maxMouseSpeed;
			pitchSpeed *= maxOrbitSpeed;
		}
		mPitchSpeed = pitchSpeed;
	}
}

void OrbitCamera::Update(float deltaTime)
{
	CameraComponent::Update(deltaTime);

	Quaternion yaw(Vector3::UnitZ, mYawSpeed * deltaTime);
	mOffset = Vector3::Transform(mOffset, yaw);
	mUp = Vector3::Transform(mUp, yaw);

	Vector3 forward = -1.f * mOffset;
	forward.Normalize();
	Vector3 right = Vector3::Cross(mUp, forward);
	right.Normalize();

	Quaternion pitch(right, mPitchSpeed * deltaTime);
	mOffset = Vector3::Transform(mOffset, pitch);
	mUp = Vector3::Transform(mUp, pitch);

	Vector3 target = mOwner->GetPosition();
	Vector3 cameraPos = target + mOffset;
	Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, mUp);
	SetViewMatrix(view);
}

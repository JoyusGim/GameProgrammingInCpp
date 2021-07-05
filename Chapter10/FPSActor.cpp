#include "FPSActor.h"
#include "InputSystem.h"
#include "MoveComponent.h"
#include "AudioComponent.h"
#include "FPSCamera.h"
#include "MeshComponent.h"
#include "BoxComponent.h"
#include "Game.h"
#include "Renderer.h"
#include "PlaneActor.h"
#include "BallActor.h"

FPSActor::FPSActor(Game* game)	:
	Actor(game)
{
	mMoveComp = new MoveComponent(this);
	mAudioComp = new AudioComponent(this);
	mLastFootstep = 0.f;
	mFootstep = mAudioComp->PlayEvent("event:/Footstep");
	mFootstep.SetPaused(true);

	mCameraComp = new FPSCamera(this);
	mCameraComp->SetMaxPitch(60.f);

	mModelActor = new Actor(game);
	mModelActor->SetScale(0.75f);
	mMeshComp = new MeshComponent(mModelActor);
	mMeshComp->SetMesh(game->GetRenderer()->GetMesh("Assets/Rifle.gpmesh"));

	mBoxComp = new BoxComponent(this);
	AABB myBox(Vector3(-25.f, -25.f, -87.5f), Vector3(25.f, 25.f, 87.5f));
	mBoxComp->SetObjectBox(myBox);
	mBoxComp->SetShouldRotate(false);
}

void FPSActor::ActorInput(const InputState& keyState)
{
	float forwardSpeed = 0.f;
	float strafeSpeed = 0.f;

	if (keyState.Keyboard.GetKeyState(SDL_SCANCODE_W) == Held)
	{
		forwardSpeed += 400.f;
	}
	if (keyState.Keyboard.GetKeyState(SDL_SCANCODE_S) == Held)
	{
		forwardSpeed -= 400.f;
	}
	if (keyState.Keyboard.GetKeyState(SDL_SCANCODE_A) == Held)
	{
		strafeSpeed -= 400.f;
	}
	if (keyState.Keyboard.GetKeyState(SDL_SCANCODE_D) == Held)
	{
		strafeSpeed += 400.f;
	}

	mMoveComp->SetForwardSpeed(forwardSpeed);
	mMoveComp->SetStrafeSpeed(strafeSpeed);

	const float maxMouseSpeed = 500.f;
	const float maxAngularSpeed = Math::Pi * 16;

	Vector2 mousePos = keyState.Mouse.GetMousePos();
	float angularSpeed = 0.f;
	if (mousePos.x != 0.f)
	{
		angularSpeed = mousePos.x / maxMouseSpeed;
		angularSpeed *= maxAngularSpeed;
	}
	mMoveComp->SetAngularSpeed(angularSpeed);

	const float maxPitchSpeed = Math::Pi * 16;
	float pitchSpeed = 0.f;

	if (mousePos.y != 0.f)
	{
		pitchSpeed = mousePos.y / maxMouseSpeed;
		pitchSpeed *= maxPitchSpeed;
	}
	mCameraComp->SetPitchSpeed(pitchSpeed);

	if (keyState.Mouse.GetButtonState(SDL_BUTTON_LEFT) == Pressed)
		Shoot();
}

void FPSActor::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);

	FixCollisions();

	mLastFootstep -= deltaTime;
	if ((!Math::NearZero(mMoveComp->GetForwardSpeed()) || 
		!Math::NearZero(mMoveComp->GetStrafeSpeed())) &&
		mLastFootstep <= 0.f)
	{
		mFootstep.SetPaused(false);
		mFootstep.Restart();
		mLastFootstep = 0.5f;
	}

	const Vector3 modelOffset(10.f, 10.f, -10.f);
	Vector3 modelPos = GetPosition();
	modelPos += GetForward() * modelOffset.x;
	modelPos += GetRight() * modelOffset.y;
	modelPos.z += modelOffset.z;
	mModelActor->SetPosition(modelPos);

	Quaternion q = GetRotate();

	q = Quaternion::Concatenate(q,
		Quaternion(GetRight(), mCameraComp->GetPitch()));
	mModelActor->SetRotate(q);

}

void FPSActor::FixCollisions()
{
	ComputeWorldTransform();

	const AABB& playerBox = mBoxComp->GetWorldBox();
	Vector3 pos = GetPosition();

	auto& planes = GetGame()->GetPlanes();
	for (auto pa : planes)
	{
		const AABB& planeBox = pa->GetBox()->GetWorldBox();
		if (Intersect(playerBox, planeBox))
		{
			float dx1 = planeBox.mMax.x - playerBox.mMin.x;
			float dx2 = planeBox.mMin.x - playerBox.mMax.x;
			float dy1 = planeBox.mMax.y - playerBox.mMin.y;
			float dy2 = planeBox.mMin.y - playerBox.mMax.y;
			float dz1 = planeBox.mMax.z - playerBox.mMin.z;
			float dz2 = planeBox.mMin.z - playerBox.mMax.z;

			float dx = (Math::Abs(dx1) < Math::Abs(dx2)) ? dx1 : dx2;
			float dy = (Math::Abs(dy1) < Math::Abs(dy2)) ? dy1 : dy2;
			float dz = (Math::Abs(dz1) < Math::Abs(dz2)) ? dz1 : dz2;

			if (Math::Abs(dx) <= Math::Abs(dy) && Math::Abs(dx) <= Math::Abs(dz))
				pos.x += dx;
			else if (Math::Abs(dy) <= Math::Abs(dx) && Math::Abs(dy) <= Math::Abs(dz))
				pos.y += dy;
			else
				pos.z += dz;

			SetPosition(pos);
			mBoxComp->OnUpdateWorldTransform();
		}
	}
}

void FPSActor::Shoot()
{
	Vector3 screenPoint(0.f, 0.f, 0.f);
	Vector3 start = GetGame()->GetRenderer()->Unproject(screenPoint);
	screenPoint.z = 0.9f;
	Vector3 end = GetGame()->GetRenderer()->Unproject(screenPoint);
	Vector3 dir = end - start;
	dir.Normalize();

	BallActor* ball = new BallActor(GetGame());
	ball->SetPlayer(this);
	ball->SetPosition(start + dir * 20.f);
	ball->RotateToNewForward(dir);
	mAudioComp->PlayEvent("event:/Shot");
}

#include "FPSActor.h"
#include "InputSystem.h"
#include "MoveComponent.h"
#include "AudioComponent.h"
#include "FPSCamera.h"
#include "MeshComponent.h"
#include "Game.h"
#include "Renderer.h"

static Actor* bullet;
static float lifeTime = 1.f;
static MoveComponent* mc;


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

	bullet = nullptr;
	lifeTime = 1.f;
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
	{
		if (!bullet)
		{
			bullet = new Actor(GetGame());
			bullet->SetPosition(
				mModelActor->GetPosition() +
				mModelActor->GetForward() * 70.f
			);
			bullet->SetScale(0.3f);

			Vector3 screenPoint = Vector3::Zero;
			Vector3 dir = Vector3::Zero;
			GetGame()->GetRenderer()->GetScreenDirection(screenPoint, dir);
			
			Vector3 up = Vector3::Cross(bullet->GetForward(), dir);
			up.Normalize();

			float rotate = Math::Acos(Vector3::Dot(dir, bullet->GetForward()));
			bullet->SetRotate(Quaternion(up, rotate));

			auto mesh = new MeshComponent(bullet);
			mesh->SetMesh(GetGame()->GetRenderer()->GetMesh("Assets/Sphere.gpmesh"));

			mc = new MoveComponent(bullet);
		}		
	}
}

void FPSActor::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);

	mLastFootstep -= deltaTime;
	if (!Math::NearZero(mMoveComp->GetForwardSpeed()) && mLastFootstep <= 0.f)
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


	if (bullet)
	{
		lifeTime -= deltaTime;
		if (lifeTime < 0.f)
		{
			delete bullet;
			bullet = nullptr;
			lifeTime = 1.f;
		}
		else
		{
			mc->SetForwardSpeed(700.f);
		}
	}
}

#include "FollowActor.h"
#include "MoveComponent.h"
#include "FollowCamera.h"
#include "SkeletalMeshComponent.h"
#include "Game.h"
#include "Renderer.h"
#include "InputSystem.h"
#include "Skeleton.h"
#include "Mesh.h"
#include "Animation.h"
#include "MirrorCamera.h"

FollowActor::FollowActor(Game* game)	:
	Actor(game),
	mMoving{ false }
{
	mMeshComp = new SkeletalMeshComponent(this);
	mMeshComp->SetMesh(game->GetRenderer()->GetMesh("Assets/CatWarrior.gpmesh"));
	mMeshComp->SetSkeleton(game->GetSkeleton("Assets/CatWarrior.gpskel"));
	mMeshComp->PlayAnimation(game->GetAnimation("Assets/CatActionIdle.gpanim"));

	mMoveComp = new MoveComponent(this);
	mCameraComp = new FollowCamera(this);
	SetPosition(Vector3(0.f, 0.f, -100.f));
	mCameraComp->SnapToIdeal();

	MirrorCamera* mirror = new MirrorCamera(this);
	mirror->SnapToIdeal();

	game->SetPlayer(this);
}

void FollowActor::ActorInput(const InputState& input)
{
	float forwardSpeed = 0.f;
	float angularSpeed = 0.f;

	if (input.Keyboard.GetKeyState(SDL_SCANCODE_W) == Held)
	{
		forwardSpeed += 300.f;
	}
	if (input.Keyboard.GetKeyState(SDL_SCANCODE_S) == Held)
	{
		forwardSpeed -= 300.f;
	}

	if (input.Keyboard.GetKeyState(SDL_SCANCODE_A) == Held)
	{
		angularSpeed -= Math::Pi;
	}
	if (input.Keyboard.GetKeyState(SDL_SCANCODE_D) == Held)
	{
		angularSpeed += Math::Pi;
	}

	if (!mMoving && !Math::NearZero(forwardSpeed))
	{
		mMoving = true;
		mMeshComp->PlayAnimation(GetGame()->GetAnimation("Assets/CatRunSprint.gpanim"), 0.3f, 1.25);
	}
	else if (mMoving && Math::NearZero(forwardSpeed))
	{
		mMoving = false;
		mMeshComp->PlayAnimation(GetGame()->GetAnimation("Assets/CatActionIdle.gpanim"), 0.3f);
	}

	mMoveComp->SetForwardSpeed(forwardSpeed);
	mMoveComp->SetAngularSpeed(angularSpeed);
}

void FollowActor::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);


}

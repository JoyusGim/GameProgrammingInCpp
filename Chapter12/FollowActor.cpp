#include "FollowActor.h"
#include "MoveComponent.h"
#include "FollowCamera.h"
#include "SkeletalMeshComponent.h"
#include "Game.h"
#include "Renderer.h"
#include "InputSystem.h"

FollowActor::FollowActor(Game* game)	:
	Actor(game)
{
	mMeshComp = new SkeletalMeshComponent(this);
	mMeshComp->SetMesh(game->GetRenderer()->GetMesh("Assets/CatWarrior.gpmesh"));

	mMoveComp = new MoveComponent(this);
	mCameraComp = new FollowCamera(this);
	SetPosition(Vector3(0.f, 0.f, -100.f));
	mCameraComp->SnapToIdeal();
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
		angularSpeed += Math::Pi;
	}
	if (input.Keyboard.GetKeyState(SDL_SCANCODE_D) == Held)
	{
		angularSpeed -= Math::Pi;
	}
	mMoveComp->SetForwardSpeed(forwardSpeed);
	mMoveComp->SetAngularSpeed(angularSpeed);
}

void FollowActor::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);


}

#include "CameraActor.h"
#include "MoveComponent.h"
#include "AudioComponent.h"
#include "Game.h"
#include "Renderer.h"
#include "AudioSystem.h"

CameraActor::CameraActor(Game* game)	:
	Actor(game)
{
	mMoveComp = new MoveComponent(this);
	mAudioComp = new AudioComponent(this);
}

void CameraActor::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);

	Vector3 cameraPos = GetPosition();
	Vector3 target = GetPosition() + GetForward() * 100.f;
	Vector3 up = Vector3::UnitZ;
	Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, up);
	GetGame()->GetRenderer()->SetViewMatrix(view);
	GetGame()->GetAudioSystem()->SetListener(view);
}

void CameraActor::ActorInput(const uint8_t* keystate)
{
	float forwardSpeed = 0.f;
	float angularSpeed = 0.f;

	if (keystate[SDL_SCANCODE_W])
	{
		forwardSpeed += 300.f;
	}
	if (keystate[SDL_SCANCODE_S])
	{
		forwardSpeed -= 300.f;
	}
	if (keystate[SDL_SCANCODE_A])
	{
		angularSpeed -= Math::TwoPi;
	}
	if (keystate[SDL_SCANCODE_D])
	{
		angularSpeed += Math::TwoPi;
	}

	mMoveComp->SetForwardSpeed(forwardSpeed);
	mMoveComp->SetAngularSpeed(angularSpeed);
}

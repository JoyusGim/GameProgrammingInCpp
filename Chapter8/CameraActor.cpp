#include "CameraActor.h"
#include "MoveComponent.h"
#include "AudioComponent.h"
#include "Game.h"
#include "Renderer.h"
#include "AudioSystem.h"
#include "InputSystem.h"

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

void CameraActor::ActorInput(const InputState& keystate)
{
	float forwardSpeed = 0.f;
	float angularSpeed = 0.f;

	if (keystate.Keyboard.GetKeyState(SDL_SCANCODE_W) == Held)
	{
		forwardSpeed += 300.f;
	}
	if (keystate.Keyboard.GetKeyState(SDL_SCANCODE_S) == Held)
	{
		forwardSpeed -= 300.f;
	}
	if (keystate.Keyboard.GetKeyState(SDL_SCANCODE_A) == Held)
	{
		angularSpeed -= Math::TwoPi;
	}
	if (keystate.Keyboard.GetKeyState(SDL_SCANCODE_D) == Held)
	{
		angularSpeed += Math::TwoPi;
	}

	if (keystate.Controller.GetButtonValue(SDL_CONTROLLER_BUTTON_A))
		forwardSpeed += 300.f;

	mMoveComp->SetForwardSpeed(forwardSpeed);
	mMoveComp->SetAngularSpeed(angularSpeed);
}

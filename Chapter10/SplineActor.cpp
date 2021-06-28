#include "SplineActor.h"
#include "SplineCamera.h"
#include "InputSystem.h"

SplineActor::SplineActor(Game* game)	:
	Actor(game)
{
	mCameraComp = new SplineCamera(this);

	Spline path;
	path.mControlPoints.emplace_back(Vector3::Zero);
	for (int i = 0; i < 5; i++)
	{
		if (i % 2 == 0)
			path.mControlPoints.emplace_back(Vector3(300.f * (i + 1), 300.f, 300.f));
		else
			path.mControlPoints.emplace_back(Vector3(300.f * (i + 1), 0.f, 0.f));
	}

	mCameraComp->SetSpline(path);
	mCameraComp->SetPaused(false);
}

void SplineActor::ActorInput(const InputState& input)
{
	if (input.Keyboard.GetKeyState(SDL_SCANCODE_R) == Released)
		mCameraComp->Restart();
}

void SplineActor::UpdateActor(float deltaTime)
{
}

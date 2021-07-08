#include "CameraComponent.h"
#include "Actor.h"
#include "Game.h"
#include "Renderer.h"
#include "AudioSystem.h"

void CameraComponent::SetViewMatrix(const Matrix4& view)
{
	Game* game = mOwner->GetGame();
	game->GetRenderer()->SetViewMatrix(view);
	game->GetAudioSystem()->SetListener(view);
}

CameraComponent::CameraComponent(Actor* owner)	:
	Component(owner)
{
}

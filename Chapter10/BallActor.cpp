#include "BallActor.h"
#include "Game.h"
#include "Renderer.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "BallMove.h"
#include "AudioComponent.h"

BallActor::BallActor(Game* game)	:
	Actor(game),
	mLifeSpan(2.f)
{
	MeshComponent* mc = new MeshComponent(this);
	Mesh* mesh = GetGame()->GetRenderer()->GetMesh("Assets/Sphere.gpmesh");
	mc->SetMesh(mesh);
	mMyMove = new BallMove(this);
	mMyMove->SetForwardSpeed(1500.f);
	mAudioComp = new AudioComponent(this);
}

void BallActor::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);

	mLifeSpan -= deltaTime;
	if (mLifeSpan < 0.f)
	{
		SetState(State::DEAD);
	}
}

void BallActor::SetPlayer(Actor* player)
{
	mMyMove->SetPlayer(player);
}

void BallActor::HitTarget()
{
	mAudioComp->PlayEvent("event:/Ding");
}

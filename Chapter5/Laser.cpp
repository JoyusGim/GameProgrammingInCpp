#include "Laser.h"
#include "Game.h"
#include "Asteroid.h"
#include "CircleColliderComponent.h"
#include "MoveComponent.h"
#include "SpriteComponent.h"

Laser::Laser(Game* game, float rotate)	:
	Actor(game),
	mLifetime{ 5.f }
{
	SetRotate(rotate);

	SpriteComponent* sc = new SpriteComponent(this);
	sc->SetTexture(game->GetTexture("Assets/Laser.png"));

	MoveComponent* mc = new MoveComponent(this);
	mc->SetMass(0.1f);
	mc->AddForce(GetForward() * 8000.f);

	mCollider = new CircleColliderComponent(this);
	mCollider->SetRadius(11.f);
}

void Laser::UpdateActor(float deltaTime)
{
	mLifetime -= deltaTime;

	if (mLifetime < 0.f)
	{
		SetState(State::DEAD);
	}
	else
	{
		for (auto ast : GetGame()->GetAsteroids())
		{
			if (Intersect(*mCollider, *(ast->GetCollider())))
			{
				SetState(State::DEAD);
				ast->SetState(State::DEAD);
				break;
			}
		}
	}
}

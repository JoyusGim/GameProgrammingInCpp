#include "Laser.h"
#include "Game.h"
#include "Asteroid.h"
#include "CircleColliderComponent.h"
#include "MoveComponent.h"
#include "SpriteComponent.h"

Laser::Laser(Game* game)	:
	Actor(game),
	mLifetime{ 5.f }
{
	SpriteComponent* sc = new SpriteComponent(this);
	sc->SetTexture(game->GetTexture("Assets/Laser.png"));

	MoveComponent* mc = new MoveComponent(this);
	mc->SetForwardSpeed(800.f); 

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

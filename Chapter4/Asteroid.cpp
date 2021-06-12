#include "Asteroid.h"
#include "Random.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "CircleColliderComponent.h"
#include "Game.h"

Asteroid::Asteroid(Game* game)	:
	Actor(game),
	mCollider{ nullptr }
{
	Math::Vector2 pos = Random::GetVector(Math::Vector2::Zero,
		Math::Vector2(1024.f, 768.f));
	SetPosition(pos);
	SetRotate(Random::GetFloatRange(0.f, 2.f * Math::PI));

	SpriteComponent* sc = new SpriteComponent(this);
	sc->SetTexture(game->GetTexture("Assets/Asteroid.png"));

	MoveComponent* mc = new MoveComponent(this);
	mc->SetForwardSpeed(150.f);

	mCollider = new CircleColliderComponent(this);
	mCollider->SetRadius(40.f);

	game->AddAsteroid(this);
}

Asteroid::~Asteroid()
{
	GetGame()->RemoveAsteroid(this);
}

void Asteroid::UpdateActor(float deltaTime)
{
	Math::Vector2 pos = GetPosition();
	if (pos.x < 0)	pos.x = 1024.f;
	else if (pos.x > 1024.f) pos.x = 0.f;

	if (pos.y < 0) pos.y = 768.f;
	else if (pos.y > 768.f) pos.y = 0;

	SetPosition(pos);
}

CircleColliderComponent* Asteroid::GetCollider() const
{
	return mCollider;
}

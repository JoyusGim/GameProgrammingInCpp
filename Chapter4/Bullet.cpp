#include "Bullet.h"
#include "Game.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "CircleColliderComponent.h"
#include "Enemy.h"

Bullet::Bullet(class Game* game) :
	Actor(game)
{
	SpriteComponent* sc = new SpriteComponent(this);
	sc->SetTexture(GetGame()->GetTexture("Assets/Projectile.png"));

	mc = new MoveComponent(this);
	mc->SetForwardSpeed(200.f);

	collider = new CircleColliderComponent(this);
	collider->SetRadius(5.f);
}

void Bullet::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);

	mLifeTime -= deltaTime;
	if (!mTarget)
		return;

	if (Intersect(*collider, *dynamic_cast<Enemy*>(mTarget)->GetCollider()))
	{
		SetState(Actor::State::DEAD);
		mTarget->SetState(Actor::State::DEAD);

		return;
	}

	Math::Vector2 diff = mTarget->GetPosition() - GetPosition();
	SetRotate(atan2(-diff.y, diff.x));

	if (mLifeTime < 0.f)
	{
		SetState(Actor::State::DEAD);
	}
}

void Bullet::SetTarget(class Actor* target)
{
	mTarget = target;
}

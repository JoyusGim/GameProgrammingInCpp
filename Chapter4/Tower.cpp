#include "Tower.h"
#include "Game.h"
#include "SpriteComponent.h"
#include "MoveComponent.h"
#include "Enemy.h"
#include "Bullet.h"
#include <algorithm>

Tower::Tower(class Game* game) :
	Actor(game)
{
	SpriteComponent* sc = new SpriteComponent(this, 200);
	sc->SetTexture(GetGame()->GetTexture("Assets/Tower.png"));

	mc = new MoveComponent(this);
	mc->SetForwardSpeed(0.f);

	mAttackTimer = AttackTime;
}

void Tower::UpdateActor(float deltaTime)
{
	Actor::UpdateActor(deltaTime);

	mAttackTimer -= deltaTime;

	auto enemise = GetGame()->GetEnemies();
	auto iter = std::min_element(enemise.begin(), enemise.end(),
		[this](const Enemy* a, const Enemy* b) {
			return (a->GetPosition() - this->GetPosition()).SquareLength() <
				(b->GetPosition() - this->GetPosition()).SquareLength();
		});

	if (iter != enemise.end())
	{
		Math::Vector2 dir = (*iter)->GetPosition() - GetPosition();
		float dist = dir.Length();
		if (dist < AttackRange)
			SetRotate(atan2f(-dir.y, dir.x));

		if (dist < AttackRange)
		{
			if(mAttackTimer <= 0.f)
			{
				Bullet* bullet = new Bullet(GetGame());
				bullet->SetTarget(*iter);
				bullet->SetPosition(GetPosition());
				(*iter)->AddEngage(bullet);
			}
		}
	}

	if (mAttackTimer <= 0.f)
		mAttackTimer = AttackTime;
}

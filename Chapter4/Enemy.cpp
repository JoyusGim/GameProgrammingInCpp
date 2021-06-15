#include "Enemy.h"
#include "Game.h"
#include "SpriteComponent.h"
#include "NavComponent.h"
#include "Grid.h"
#include "Tile.h"
#include "Bullet.h"
#include "CircleColliderComponent.h"

Enemy::Enemy(class Game* game) :
	Actor(game)
{
	SpriteComponent* sc = new SpriteComponent(this, 200);
	sc->SetTexture(GetGame()->GetTexture("Assets/Airplane.png"));

	SetPosition(GetGame()->GetGrid()->GetStartTile()->GetPosition());

	NavComponent* nc = new NavComponent(this);
	nc->SetForwardSpeed(150.f);
	nc->StartPath(GetGame()->GetGrid()->GetStartTile());

	collider = new CircleColliderComponent(this);
	collider->SetRadius(25.f);

	GetGame()->AddEnemy(this);
}


Enemy::~Enemy()
{
	GetGame()->RemoveEnemy(this);
	for (auto b : engaged)
	{
		b->SetTarget(nullptr);
	}
}

void Enemy::UpdateActor(float deltaTime)
{
	auto pos = GetPosition();
	auto tiles = GetGame()->GetGrid()->GetTiles();

	pos.y -= (192 - 32);
	if (tiles[pos.y / 64][pos.x / 64]->GetTileState() == Tile::TileState::Default)
		SetState(Actor::State::DEAD);
	
	Math::Vector2 diff = GetPosition() - GetGame()->GetGrid()->GetEndTile()->GetPosition();

	if (Math::NearZero(diff.Length(), 2.f))
	{
		SetState(Actor::State::DEAD);
	}
}

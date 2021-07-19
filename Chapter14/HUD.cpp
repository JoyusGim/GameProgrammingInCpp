#include "HUD.h"
#include <algorithm>
#include "Game.h"
#include "Renderer.h"
#include "Collision.h"
#include "PhysWorld.h"
#include "TargetComponent.h"
#include "Component.h"
#include "Actor.h"
#include "FollowActor.h"

void HUD::UpdateCrossHair(float deltaTime)
{
	mTargetEnemy = false;
	const float cAimDist = 5000.f;
	Vector3 start, dir;
	mGame->GetRenderer()->GetScreenDirection(start, dir);
	LineSegment l(start + dir * 25.f, start + dir * cAimDist);

	PhysWorld::CollisionInfo info;
	if (mGame->GetPhysWorld()->SegmentCast(l, info))
	{
		for (auto tc : mTargetComps)
		{
			if (tc->GetOwner() == info.mActor)
			{
				mTargetEnemy = true;
				break;
			}
		}
	}
}

void HUD::UpdateRadar(float deltaTime)
{
	mBlips.clear();

	Vector3 playerPos = mGame->GetPlayer()->GetPosition();
	Vector2 playerPos2D(playerPos.y, playerPos.x);

	Vector3 playerForward = mGame->GetPlayer()->GetForward();
	Vector2 playerForward2D(playerForward.x, playerForward.y);

	float angle = Math::Atan2(playerForward2D.y, playerForward2D.x);
	Matrix3 rotMat = Matrix3::CreateRotation(angle);

	for (auto target : mTargetComps)
	{
		Vector3 targetPos = target->GetOwner()->GetPosition();
		Vector2 targetPos2D(targetPos.y, targetPos.x);

		Vector2 playerToTarget = targetPos2D - playerPos2D;

		if (playerToTarget.SquareLength() <= (mRadarRange * mRadarRange))
		{
			Vector2 blipPos = playerToTarget;
			blipPos *= mRadarRadius / mRadarRange;
			blipPos = Vector2::Transform(blipPos, rotMat);
			mBlips.emplace_back(blipPos);
		}
	}
}

HUD::HUD(Game* game)	:
	UIScreen(game),
	mTargetComps{ false },
	mRadarRange{ 2000.f },
	mRadarRadius{ 92.f }
{
	mCrosshair = mGame->GetRenderer()->GetTexture("Assets/CrosshairGreen.png");
	mCrosshairEnemy = mGame->GetRenderer()->GetTexture("Assets/CrosshairRed.png");

	mRadar = mGame->GetRenderer()->GetTexture("Assets/Radar.png");
	mBlipTex = mGame->GetRenderer()->GetTexture("Assets/Blip.png");

	mRadarArrow = mGame->GetRenderer()->GetTexture("Assets/RadarArrow.png");
}

void HUD::Update(float deltaTime)
{
	UIScreen::Update(deltaTime);

	UpdateCrossHair(deltaTime);
	UpdateRadar(deltaTime);
}

void HUD::Draw(Shader* shader)
{
	//Texture* cross = mTargetEnemy ? mCrosshairEnemy : mCrosshair;
	//DrawTexture(shader, cross, Vector2::Zero, 2.f);

	const Vector2 cRadarPos(-390.f, 275.f);
	DrawTexture(shader, mRadar, cRadarPos);

	for (const Vector2& blip : mBlips)
	{
		DrawTexture(shader, mBlipTex, cRadarPos + blip, 1.f);
	}
	DrawTexture(shader, mRadarArrow, cRadarPos);

	//Texture* mirror = mGame->GetRenderer()->GetMirrorTexture();
	//DrawTexture(shader, mirror, Vector2(-350.f, -250.f), 1.f, true);
}

void HUD::AddTarget(TargetComponent* target)
{
	mTargetComps.push_back(target);
}

void HUD::RemoveTarget(TargetComponent* target)
{
	auto iter = std::find(mTargetComps.begin(), mTargetComps.end(), target);
	mTargetComps.erase(iter);
}

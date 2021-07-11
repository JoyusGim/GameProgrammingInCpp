#include "TargetComponent.h"
#include "HUD.h"
#include "Game.h"
#include "Actor.h"

TargetComponent::TargetComponent(Actor* owner)	:
	Component(owner)
{
	mOwner->GetGame()->GetHUD()->AddTarget(this);
}

TargetComponent::~TargetComponent()
{
	mOwner->GetGame()->GetHUD()->RemoveTarget(this);
}

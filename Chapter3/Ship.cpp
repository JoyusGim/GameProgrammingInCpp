#include "Ship.h"
#include "SpriteComponent.h"
#include "InputComponent.h"
#include "Game.h"

Ship::Ship(Game* game)  :
    Actor(game)
{
    SpriteComponent* asc = new SpriteComponent(this);
    asc->SetTexture(game->GetTexture("Assets/Ship.png"));

    InputComponent* ic = new InputComponent(this);
    ic->SetMaxForwardSpeed(150.f);
    ic->SetMaxAngularSpeed(10.f);
    ic->SetForwardKey(SDL_SCANCODE_W);
    ic->SetBackKey(SDL_SCANCODE_S);
    ic->SetClockWiseKey(SDL_SCANCODE_D);
    ic->SetCounterClockWiseKey(SDL_SCANCODE_A);
}

void Ship::UpdateActor(float deltaTime)
{
    Actor::UpdateActor(deltaTime);
}
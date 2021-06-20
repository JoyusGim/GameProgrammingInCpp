#include "Ship.h"
#include "SpriteComponent.h"
#include "InputComponent.h"
#include "Game.h"
#include "Laser.h"

Ship::Ship(Game* game)  :
    Actor(game),
    mLaserColltime{ 0.f }
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
    ic->SetMass(2.f);

    SetPosition(0, 0);
}

void Ship::UpdateActor(float deltaTime)
{
    mLaserColltime -= deltaTime;
}

void Ship::ActorInput(const uint8_t* keyState)
{
    if (keyState[SDL_SCANCODE_SPACE] && mLaserColltime <= 0.f)
    {
        Laser* laser = new Laser(GetGame(), GetRotate());
        laser->SetPosition(GetPosition() + GetForward() * 5.f);
        laser->SetRotate(GetRotate());

        mLaserColltime = 1.f;
    }
}

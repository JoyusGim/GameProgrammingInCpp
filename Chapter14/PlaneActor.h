#pragma once
#include "Actor.h"
class PlaneActor :
    public Actor
{
    class BoxComponent* mBox;

public:
    PlaneActor(class Game* game);

    class BoxComponent* GetBox() const { return mBox; }

    TypeID GetType() const override { return Actor::PLANEACTOR; }
};


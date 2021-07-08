#pragma once
#include "Component.h"
#include "Collision.h"

class BoxComponent :
    public Component
{
    AABB mObjectBox;
    AABB mWorldBox;
    bool mShouldRotate;

public:
    BoxComponent(class Actor* owner);
    ~BoxComponent();

    void OnUpdateWorldTransform();

    void SetObjectBox(const AABB& model);
    const AABB& GetWorldBox() const;
    void SetShouldRotate(bool value);
    class Actor* GetOwner() const;
};


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

    void OnUpdateWorldTransform() override;
    void LoadProperties(const rapidjson::Value& inObj) override;
    void SaveProperties(rapidjson::Document::AllocatorType& alloc, rapidjson::Value& inObj) const override;


    void SetObjectBox(const AABB& model);
    const AABB& GetWorldBox() const;
    void SetShouldRotate(bool value);
    class Actor* GetOwner() const;
    TypeID GetType() const override { return TypeID::BoxComponent; }
};


#pragma once
#include "MeshComponent.h"
#include "MatrixPalette.h"

class SkeletalMeshComponent :
    public MeshComponent
{
    const class Skeleton* mSkeleton;
    const class Animation* mCurrAnimation;
    const class Animation* mNextAnimation;
    float mAnimPlayRate;
    float mCurrAnimTime;
    float mNextAnimTime;
    float mBlendDuration;
    float mBlendTime;
    MatrixPalette mPalette;

public:
    SkeletalMeshComponent(class Actor* owner);

    void Draw(class Shader* shader) override;
    void Update(float deltaTime) override;
    void LoadProperties(const rapidjson::Value& inObj) override;
    void SaveProperties(rapidjson::Document::AllocatorType& alloc, rapidjson::Value& inObj) const override;

    void ComputeMatrixPalette();
    float PlayAnimation(const class Animation* anim, float blendDuration = 0.f, float playRate = 1.f);

    void SetSkeleton(class Skeleton* skeleton);
    TypeID GetType() const override { return TypeID::SkeletalMeshComponent; }
};
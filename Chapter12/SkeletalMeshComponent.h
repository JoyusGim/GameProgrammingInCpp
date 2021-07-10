#pragma once
#include "MeshComponent.h"
#include "MatrixPalette.h"

class SkeletalMeshComponent :
    public MeshComponent
{
    const class Skeleton* mSkeleton;
    const class Animation* mAnimation;
    float mAnimPlayRate;
    float mAnimTime;
    MatrixPalette mPalette;

public:
    SkeletalMeshComponent(class Actor* owner);

    void Draw(class Shader* shader) override;
    void Update(float deltaTime) override;

    void ComputeMatrixPalette();
    float PlayAnimation(const class Animation* anim, float playRate = 1.f);

    void SetSkeleton(class Skeleton* skeleton);
};
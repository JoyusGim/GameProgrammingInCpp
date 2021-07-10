#include "SkeletalMeshComponent.h"
#include "Actor.h"
#include "Game.h"
#include "Mesh.h"
#include "Texture.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Skeleton.h"
#include "Animation.h"

SkeletalMeshComponent::SkeletalMeshComponent(Actor* owner)	:
	MeshComponent(owner, true),
	mCurrAnimation{ nullptr },
	mCurrAnimTime{ 0.f },
	mNextAnimation{ nullptr },
	mNextAnimTime{ 0.f },
	mAnimPlayRate{ 0.f }
{
}

void SkeletalMeshComponent::Draw(Shader* shader)
{
	if (mMesh)
	{
		shader->SetMatrixUniform("uWorldTransform",
			mOwner->GetWorldTransform());

		shader->SetMatrixUniforms("uMatrixPalette", &mPalette.mEntry[0], MAX_SKELETON_BONES);
		shader->SetFloatUniform("uSpecPower", mMesh->GetSpecPower());

		Texture* t = mMesh->GetTexture(mTextureIndex);
		if (t) t->SetActive();
		VertexArray* va = mMesh->GetVertexArray();
		va->SetActive();
		glDrawElements(GL_TRIANGLES, va->GetNumIndices(), GL_UNSIGNED_INT, nullptr);
	}
}

void SkeletalMeshComponent::Update(float deltaTime)
{
	if (mCurrAnimation && mSkeleton)
	{
		mCurrAnimTime += deltaTime * mAnimPlayRate;
		while (mCurrAnimTime > mCurrAnimation->GetDuration())
		{
			mCurrAnimTime -= mCurrAnimation->GetDuration();
		}

		if (mNextAnimation)
		{
			mNextAnimTime += deltaTime * mAnimPlayRate;
			while (mNextAnimTime > mNextAnimation->GetDuration())
			{
				mNextAnimTime -= mNextAnimation->GetDuration();
			}
			mBlendTime += deltaTime * mAnimPlayRate;
			if (mBlendTime > mBlendDuration)
			{
				mCurrAnimation = mNextAnimation;
				mCurrAnimTime = mNextAnimTime;
				mNextAnimation = nullptr;
				mNextAnimTime = 0.f;
				mBlendDuration = 0.f;
				mBlendTime = 0.f;
			}
		}

		ComputeMatrixPalette();
	}
}

void SkeletalMeshComponent::ComputeMatrixPalette()
{
	const std::vector<Matrix4>& globalInvVindPoses =
		mSkeleton->GetGlobalInvBindPoses();
	std::vector<Matrix4> currentPoses;
		
	if (!mNextAnimation)
		mCurrAnimation->GetGlobalPoseAtTime(currentPoses, mSkeleton, mCurrAnimTime);
	else
		Animation::GetBlendedGlobalPoseAtTime(currentPoses, mSkeleton, mCurrAnimation, mCurrAnimTime, mNextAnimation, mNextAnimTime, mBlendTime / mBlendDuration);

	for (size_t i = 0; i < mSkeleton->GetNumBones(); i++)
	{
		mPalette.mEntry[i] = globalInvVindPoses[i] * currentPoses[i];
	}
}

float SkeletalMeshComponent::PlayAnimation(const Animation* anim, float blendDuration /* = 0.f */, float playRate /* = 1.f */)
{
	if (!mCurrAnimation)
	{
		mCurrAnimation = anim;
		mCurrAnimTime = 0.f;
		mAnimPlayRate = playRate;
	}
	else
	{
		mNextAnimation = anim;
		mNextAnimTime = 0.f;
		mAnimPlayRate = playRate;
	}
	mBlendDuration = blendDuration;
	mBlendTime = 0.f;

	if (!mCurrAnimation)
		return 0.f;

	ComputeMatrixPalette();
	return mCurrAnimation->GetDuration();
}

void SkeletalMeshComponent::SetSkeleton(Skeleton* skeleton)
{
	mSkeleton = skeleton;
}

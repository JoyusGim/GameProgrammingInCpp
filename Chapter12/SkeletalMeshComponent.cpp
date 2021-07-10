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
	MeshComponent(owner, true)
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
	if (mAnimation && mSkeleton)
	{
		mAnimTime += deltaTime * mAnimPlayRate;
		while (mAnimTime > mAnimation->GetDuration())
		{
			mAnimTime -= mAnimation->GetDuration();
		}

		ComputeMatrixPalette();
	}
}

void SkeletalMeshComponent::ComputeMatrixPalette()
{
	const std::vector<Matrix4>& globalInvVindPoses =
		mSkeleton->GetGlobalInvBindPoses();
	std::vector<Matrix4> currentPoses;
	mAnimation->GetGlobalPoseAtTime(currentPoses, mSkeleton, mAnimTime);

	for (size_t i = 0; i < mSkeleton->GetNumBones(); i++)
	{
		mPalette.mEntry[i] = globalInvVindPoses[i] * currentPoses[i];
	}
}

float SkeletalMeshComponent::PlayAnimation(const Animation* anim, float playRate /* = 1.f */)
{
	mAnimation = anim;
	mAnimTime = 0.f;
	mAnimPlayRate = playRate;

	if (!mAnimation)
		return 0.f;

	ComputeMatrixPalette();
	return mAnimation->GetDuration();
}

void SkeletalMeshComponent::SetSkeleton(Skeleton* skeleton)
{
	mSkeleton = skeleton;
}

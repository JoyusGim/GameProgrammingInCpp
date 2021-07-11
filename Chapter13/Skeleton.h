#pragma once
#include "BoneTransform.h"
#include <string>
#include <vector>

class Skeleton
{
public:
	struct Bone
	{
		BoneTransform mLocalBindPose;
		std::string mName;
		int mParent;
	};

private:
	std::vector<Bone> mBones;
	std::vector<Matrix4> mGlobalInvBindPoses;

	void ComputeGlobalInvBindPose();

public:
	bool Load(const std::string& fileName);

	size_t GetNumBones() const;
	const Bone& GetBone(size_t idx);
	const std::vector<Bone>& GetBones() const;
	const std::vector<Matrix4> GetGlobalInvBindPoses() const;
};


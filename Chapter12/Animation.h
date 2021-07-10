#pragma once
#include <vector>
#include <string>
#include "BoneTransform.h"

class Animation
{
	size_t mNumBones;
	size_t mNumFrames;
	float mDuration;
	float mFrameDuration;
	std::vector<std::vector<BoneTransform>> mTracks;

public:
	bool Load(const std::string& fileName);
	void GetGlobalPoseAtTime(std::vector<Matrix4>& outPoses,
		const class Skeleton* inSkeleton, float inTime) const;

	size_t GetNumBones() const;
	size_t GetNumFrames() const;
	float GetDuration() const;
	float GetFrameDuration() const;
};


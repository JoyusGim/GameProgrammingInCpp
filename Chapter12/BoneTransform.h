#pragma once
#include "Math.h"

class BoneTransform
{
public:
	Quaternion mRotation;
	Vector3 mTranslation;

	Matrix4 ToMatrix() const;

	static BoneTransform Interpolation(const BoneTransform& a, const BoneTransform& b, float f);
};


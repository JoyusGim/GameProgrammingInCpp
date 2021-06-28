#include "SplineCamera.h"

Vector3 Spline::Compute(size_t startIdx, float t) const
{
	if (startIdx >= mControlPoints.size())
		return mControlPoints.back();
	else if (startIdx == 0)
		return mControlPoints[startIdx];
	else if (startIdx + 2 >= mControlPoints.size())
		return mControlPoints[startIdx];

	Vector3 p0 = mControlPoints[startIdx - 1];
	Vector3 p1 = mControlPoints[startIdx];
	Vector3 p2 = mControlPoints[startIdx + 1];
	Vector3 p3 = mControlPoints[startIdx + 2];

	Vector3 position = 0.5f * ((2.f * p1) + (-1.f * p0 + p2) * t +
		(2.f * p0 - 5.f * p1 + 4.f * p2 - p3) * t * t +
		(-1.f * p0 + 3.f * p1 - 3.f * p2 + p3) * t * t * t);
	return position;
}

SplineCamera::SplineCamera(Actor* owner) :
	CameraComponent(owner),
	mIndex{ 1 },
	mT{ 0.f },
	mSpeed{ 0.5f },
	mPaused{ true }
{
}

void SplineCamera::Update(float deltaTime)
{
	CameraComponent::Update(deltaTime);

	if (!mPaused)
	{
		mT += mSpeed * deltaTime;
		if (mT >= 1.f)
		{
			if (mIndex < mPath.GetNumPoints() - 3)
			{
				mIndex++;
				mT = mT - 1.f;
			}
			else
			{
				mPaused = true;
			}
		}
	}

	Vector3 cameraPos = mPath.Compute(mIndex, mT);
	Vector3 target = mPath.Compute(mIndex, mT + 0.01f);
	const Vector3 up = Vector3::UnitZ;
	Matrix4 view = Matrix4::CreateLookAt(cameraPos, target, up);
	SetViewMatrix(view);
}

void SplineCamera::SetSpline(const Spline& path)
{
	mPath = path;
}

void SplineCamera::SetPaused(bool paused)
{
	mPaused = paused;
}

void SplineCamera::Restart()
{
	mIndex = 1;
	mT = 0.f;
	mPaused = false;
}

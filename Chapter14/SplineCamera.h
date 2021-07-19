#pragma once
#include "CameraComponent.h"
#include "Math.h"
#include <vector>


struct Spline
{
    std::vector<Vector3> mControlPoints;
    Vector3 Compute(size_t startIdx, float t) const;
    size_t GetNumPoints() const { return mControlPoints.size(); }
};

class SplineCamera :
    public CameraComponent
{
    Spline mPath;
    size_t mIndex;
    float mT;

    float mSpeed;
    float mPaused;

public:
    SplineCamera(class Actor* owner);

    void Update(float deltaTime) override;

    void SetSpline(const Spline& path);
    void SetPaused(bool paused);

    void Restart();
    TypeID GetType() const override { return TypeID::SplineCamera; }
};


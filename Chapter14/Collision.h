#pragma once
#include "Math.h"
#include <vector>

struct LineSegment
{
	Vector3 mStart;
	Vector3 mEnd;

	LineSegment(const Vector3& start, const Vector3& end);

	Vector3 PointOnSegment(float t) const;
	float MinDistSq(const Vector3& point) const;
	static float MinDistSq(const LineSegment& s1, const LineSegment& s2);
};

struct Plane
{
	Vector3 mNormal;
	float mD;

	Plane(const Vector3& normal, float d);
	Plane(const Vector3& a, const Vector3& b, const Vector3& c);

	float SignedDist(const Vector3& point) const;
};

struct Sphere
{
	Vector3 mCenter;
	float mRadius;

	Sphere(const Vector3& center, float radius);

	bool Contain(const Vector3& point) const;
};

struct AABB
{
	Vector3 mMin;
	Vector3 mMax;

	AABB(const Vector3& min, const Vector3& max);

	void UpdateMinMax(const Vector3& point);
	void Rotate(const Quaternion& q);

	bool Contain(const Vector3& point) const;
	float MinDistSq(const Vector3& point) const;
};

struct OBB
{
	Vector3 mCenter;
	Quaternion mRotation;
	Vector3 mExtents;
};

struct Capsule
{
	LineSegment mSegment;
	float mRadius;

	Capsule(const Vector3& start, const Vector3& end, float radius);
	bool Contain(const Vector3& point) const;
};

struct ConvexPolygon
{
	std::vector<Vector2> mVertices;

	bool Contain(const Vector2& point) const;
};

bool Intersect(const Sphere& a, const Sphere& b);
bool Intersect(const AABB& a, const AABB& b);
bool Intersect(const Sphere& s, const AABB& box);
bool Intersect(const Capsule& a, const Capsule& b);
bool Intersect(const LineSegment& l, const Plane& p, float& outT);
bool Intersect(const LineSegment& l, const Sphere& s, float& outT);
bool Intersect(const LineSegment& l, const AABB& b, float& outT, Vector3& outNorm);

bool SweptSphere(const Sphere& P0, const Sphere& P1, const Sphere& Q0, const Sphere& Q1, float& outT);
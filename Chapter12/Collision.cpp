#include "Collision.h"
#include <array>
#include <algorithm>

LineSegment::LineSegment(const Vector3& start, const Vector3& end)	:
	mStart{ start },
	mEnd{ end }
{
}

Vector3 LineSegment::PointOnSegment(float t) const
{
	return mStart + (mEnd - mStart) * t;
}

float LineSegment::MinDistSq(const Vector3& point) const
{
	Vector3 ab = mEnd - mStart;
	Vector3 ba = -1.f * ab;
	Vector3 ac = point - mStart;
	Vector3 bc = point - mEnd;

	if (Vector3::Dot(ab, ac) < 0.f)
	{
		return ac.SquareLength();
	}
	else if (Vector3::Dot(ba, bc) < 0.f)
	{
		return bc.SquareLength();
	}
	else
	{
		float scalar = Vector3::Dot(ac, ab) / Vector3::Dot(ab, ab);
		Vector3 p = scalar * ab;
		return (ac - p).SquareLength();
	}
}

float LineSegment::MinDistSq(const LineSegment& s1, const LineSegment& s2)
{
	Vector3   u = s1.mEnd - s1.mStart;
	Vector3   v = s2.mEnd - s2.mStart;
	Vector3   w = s1.mStart - s2.mStart;
	float    a = Vector3::Dot(u, u);         // always >= 0
	float    b = Vector3::Dot(u, v);
	float    c = Vector3::Dot(v, v);         // always >= 0
	float    d = Vector3::Dot(u, w);
	float    e = Vector3::Dot(v, w);
	float    D = a * c - b * b;        // always >= 0
	float    sc, sN, sD = D;       // sc = sN / sD, default sD = D >= 0
	float    tc, tN, tD = D;       // tc = tN / tD, default tD = D >= 0

								   // compute the line parameters of the two closest points
	if (Math::NearZero(D)) { // the lines are almost parallel
		sN = 0.0;         // force using point P0 on segment S1
		sD = 1.0;         // to prevent possible division by 0.0 later
		tN = e;
		tD = c;
	}
	else {                 // get the closest points on the infinite lines
		sN = (b * e - c * d);
		tN = (a * e - b * d);
		if (sN < 0.0) {        // sc < 0 => the s=0 edge is visible
			sN = 0.0;
			tN = e;
			tD = c;
		}
		else if (sN > sD) {  // sc > 1  => the s=1 edge is visible
			sN = sD;
			tN = e + b;
			tD = c;
		}
	}

	if (tN < 0.0) {            // tc < 0 => the t=0 edge is visible
		tN = 0.0;
		// recompute sc for this edge
		if (-d < 0.0)
			sN = 0.0;
		else if (-d > a)
			sN = sD;
		else {
			sN = -d;
			sD = a;
		}
	}
	else if (tN > tD) {      // tc > 1  => the t=1 edge is visible
		tN = tD;
		// recompute sc for this edge
		if ((-d + b) < 0.0)
			sN = 0;
		else if ((-d + b) > a)
			sN = sD;
		else {
			sN = (-d + b);
			sD = a;
		}
	}
	// finally do the division to get sc and tc
	sc = (Math::NearZero(sN) ? 0.0f : sN / sD);
	tc = (Math::NearZero(tN) ? 0.0f : tN / tD);

	// get the difference of the two closest points
	Vector3   dP = w + (sc * u) - (tc * v);  // =  S1(sc) - S2(tc)

	return dP.SquareLength();   // return the closest distance squared
}

Plane::Plane(const Vector3& normal, float d)	:
	mNormal{ normal },
	mD{ d }
{
}

Plane::Plane(const Vector3& a, const Vector3& b, const Vector3& c)
{
	Vector3 ab = b - a;
	Vector3 ac = c - a;

	mNormal = Vector3::Cross(ab, ac);
	mNormal.Normalize();

	mD = -Vector3::Dot(a, mNormal);
}

float Plane::SignedDist(const Vector3& point) const
{
	return Vector3::Dot(point, mNormal) - mD; 
}

AABB::AABB(const Vector3& min, const Vector3& max)	:
	mMin{ min },
	mMax{ max }
{
}

void AABB::UpdateMinMax(const Vector3& point)
{
	mMin.x = Math::Min(mMin.x, point.x);
	mMin.y = Math::Min(mMin.y, point.y);
	mMin.z = Math::Min(mMin.z, point.z);
	mMax.x = Math::Max(mMax.x, point.x);
	mMax.y = Math::Max(mMax.y, point.y);
	mMax.z = Math::Max(mMax.z, point.z);
}

void AABB::Rotate(const Quaternion& q)
{
	std::array<Vector3, 8> points;
	points[0] = Vector3(mMin);
	points[1] = Vector3(mMax.x, mMin.y, mMin.z);
	points[2] = Vector3(mMin.x, mMax.y, mMin.z);
	points[3] = Vector3(mMin.x, mMin.y, mMax.z);

	points[4] = Vector3(mMin.x, mMax.y, mMax.z);
	points[5] = Vector3(mMax.x, mMin.y, mMax.z);
	points[6] = Vector3(mMax.x, mMax.y, mMin.z);
	points[7] = Vector3(mMax);

	Vector3 p = Vector3::Transform(points[0], q);
	mMin = p;
	mMax = p;
	for (size_t i = 1; i < points.size(); i++)
	{
		p = Vector3::Transform(points[i], q);
		UpdateMinMax(p);
	}
}

bool AABB::Contain(const Vector3& point) const
{
	bool outside = point.x < mMin.x ||
		point.y < mMin.y ||
		point.z < mMin.z ||
		point.x > mMax.x ||
		point.y > mMax.y ||
		point.z > mMax.z;
	return !outside;
}

float AABB::MinDistSq(const Vector3& point) const
{
	float dx = Math::Max(mMin.x - point.x, 0.f);
	dx = Math::Max(dx, point.x - mMax.x);
	float dy = Math::Max(mMin.y - point.y, 0.f);
	dy = Math::Max(dy, point.y - mMax.y);
	float dz = Math::Max(mMin.z - point.z, 0.f);
	dz = Math::Max(dz, point.z - mMax.z);

	return dx * dx + dy * dy + dz * dz;
}

Sphere::Sphere(const Vector3& center, float radius)	:
	mCenter{ center },
	mRadius{ radius }
{
}

bool Sphere::Contain(const Vector3& point) const
{
	float distSq = (mCenter - point).SquareLength();
	return distSq <= (mRadius * mRadius);
}

Capsule::Capsule(const Vector3& start, const Vector3& end, float radius)	:
	mSegment{ LineSegment(start, end) },
	mRadius{ radius }
{
}

bool Capsule::Contain(const Vector3& point) const
{
	float distSq = mSegment.MinDistSq(point);
	return distSq <= (mRadius * mRadius);
}

bool ConvexPolygon::Contain(const Vector2& point) const
{
	float sum = 0.f;
	Vector2 a, b;

	for (size_t i = 0; i < mVertices.size() - 1; i++)
	{
		a = mVertices[i] - point;
		a.Normalize();
		b = mVertices[i + 1] - point;
		b.Normalize();
		sum += Math::Acos(Vector2::Dot(a, b));
	}

	a = mVertices.back() - point;
	a.Normalize();
	b = mVertices.front() - point;
	b.Normalize();
	sum += Math::Acos(Vector2::Dot(a, b));

	return Math::NearZero(sum - Math::TwoPi);
}

bool Intersect(const Sphere& a, const Sphere& b)
{
	float distSq = (a.mCenter - b.mCenter).SquareLength();
	float sumRad = a.mRadius + b.mRadius;
	return distSq <= (sumRad * sumRad);
}

bool Intersect(const AABB& a, const AABB& b)
{
	bool no = a.mMax.x < b.mMin.x ||
		a.mMax.y < b.mMin.y ||
		a.mMax.z < b.mMin.z ||
		a.mMin.x > b.mMax.x ||
		a.mMin.y > b.mMax.y ||
		a.mMin.z > b.mMax.z;
	return !no;
}

bool Intersect(const Sphere& s, const AABB& box)
{
	float distSq = box.MinDistSq(s.mCenter);
	return distSq <= (s.mRadius * s.mRadius);
}

bool Intersect(const Capsule& a, const Capsule& b)
{
	float distSq = a.mSegment.MinDistSq(a.mSegment, b.mSegment);
	float sumRad = a.mRadius + b.mRadius;
	return distSq <= (sumRad * sumRad);
}

bool Intersect(const LineSegment& l, const Plane& p, float& outT)
{
	float denom = Vector3::Dot(l.mEnd - l.mStart, p.mNormal);

	if (Math::NearZero(denom))
	{
		if (Math::NearZero(Vector3::Dot(l.mStart, p.mNormal) - p.mD))
		{
			outT = 0.f;
			return true;
		}
		else
			return false;
	}
	else
	{
		float numer = -Vector3::Dot(l.mStart, p.mNormal) - p.mD;
		outT = numer / denom;
		if (outT >= 0.f && outT <= 1.f)
			return true;
		else
			return false;
	}
	return false;
}

bool Intersect(const LineSegment& l, const Sphere& s, float& outT)
{
	Vector3 X = l.mStart - s.mCenter;
	Vector3 Y = l.mEnd - l.mStart;
	float a = Vector3::Dot(Y, Y);
	float b = 2.f * Vector3::Dot(X, Y);
	float c = Vector3::Dot(X, X) - s.mRadius * s.mRadius;

	float disc = b * b - 4.f * a * c;
	if (disc < 0.f)
		return false;
	else
	{
		disc = Math::Sqrt(disc);
		float tMin = (-b - disc) / (2.f * a);
		float tMax = (-b + disc) / (2.f * a);

		if (tMin >= 0.f && tMin <= 1.f)
		{
			outT = tMin;
			return true;
		}
		else if (tMax >= 0.f && tMax <= 1.f)
		{
			outT = tMax;
			return true;
		}
		else
			return false;
	}

	return false;
}

bool TestSidePlane(float start, float end, float negd,
	const Vector3& norm, std::vector<std::pair<float, Vector3>>& out)
{
	float denom = end - start;
	if (Math::NearZero(denom))
		return false;
	else
	{
		float numer = -start + negd;
		float t = numer / denom;

		if (t >= 0.f && t <= 1.f)
		{
			out.emplace_back(t, norm);
			return true;
		}
		else
			return false;
	}
}

bool Intersect(const LineSegment& l, const AABB& b, float& outT, Vector3& outNorm)
{
	std::vector<std::pair<float, Vector3>> tValues;
	TestSidePlane(l.mStart.x, l.mEnd.x, b.mMin.x, Vector3::NegUnitX, tValues);
	TestSidePlane(l.mStart.x, l.mEnd.x, b.mMax.x, Vector3::UnitX, tValues);

	TestSidePlane(l.mStart.y, l.mEnd.y, b.mMin.y, Vector3::NegUnitY, tValues);
	TestSidePlane(l.mStart.y, l.mEnd.y, b.mMax.y, Vector3::UnitY, tValues);

	TestSidePlane(l.mStart.z, l.mEnd.z, b.mMin.z, Vector3::NegUnitZ, tValues);
	TestSidePlane(l.mStart.z, l.mEnd.z, b.mMax.z, Vector3::UnitZ, tValues);

	std::sort(tValues.begin(), tValues.end(), [](
		const std::pair<float, Vector3>& a,
		const std::pair<float, Vector3>& b)	{
			return a.first < b.first;
	});

	Vector3 point;
	for (auto& t : tValues)
	{
		point = l.PointOnSegment(t.first);
		if (b.Contain(point))
		{
			outT = t.first;
			outNorm = t.second;
			return true;
		}
	}
	return false;
}

bool SweptSphere(const Sphere& P0, const Sphere& P1, const Sphere& Q0, const Sphere& Q1, float& outT)
{
	Vector3 X = P0.mCenter - Q0.mCenter;
	Vector3 Y = P1.mCenter - P0.mCenter - 
		(Q1.mCenter - Q0.mCenter);
	float a = Vector3::Dot(Y, Y);
	float b = 2.f * Vector3::Dot(X, Y);
	float sumRad = P0.mRadius + Q0.mRadius;
	float c = Vector3::Dot(X, X) - sumRad * sumRad;

	float disc = b * b - 4.f * a * c;
	if (disc < 0.f)
		return false;
	else
	{
		disc = Math::Sqrt(disc);
		outT = (-b - disc) / (2.f * a);
		if (outT >= 0.f && outT <= 0.f)
			return true;
		else
			return false;
	}
}

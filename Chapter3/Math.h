#pragma once

#include <cmath>
#include <limits>

namespace Math {

const float PI = 3.1415926535f;

inline float DEG2RAD(float degree)
{
	return degree * PI / 180.f;
}

inline float RAD2DEG(float radian)
{
	return radian * 180.f / PI;
}

inline bool NearZero(float val, float epsilon = 0.001f)
{
	if (fabs(val) <= epsilon)
	{
		return true;
	}
	else
	{
		return false;
	}
}

class Vector2
{
public:
	float x;
	float y;
	
	Vector2() : x{ 0.f }, y{ 0.f } {}
	explicit Vector2(float x, float y) : x{ x }, y{ y } {}

	friend Vector2 operator+(const Vector2& a, const Vector2& b)
	{
		return Vector2(a.x + b.x, a.y + b.y);
	}

	friend Vector2 operator-(const Vector2& a, const Vector2& b)
	{
		return Vector2(a.x - b.x, a.y - b.y);
	}

	friend Vector2 operator*(const Vector2& a, const Vector2& b)
	{
		return Vector2(a.x * b.x, a.y * b.y);
	}

	friend Vector2 operator*(const Vector2& vec, float scalar)
	{
		return Vector2(vec.x * scalar, vec.y * scalar);
	}

	friend Vector2 operator*(float scalar, const Vector2& vec)
	{
		return Vector2(vec.x * scalar, vec.y * scalar);
	}

	Vector2& operator*=(float scalar)
	{
		x *= scalar;
		y *= scalar;
		return *this;
	}

	Vector2& operator+=(const Vector2& right)
	{
		x += right.x;
		y += right.y;
		return *this;
	}

	Vector2& operator-=(const Vector2& right)
	{
		x -= right.x;
		y -= right.y;
		return *this;
	}

	void Normalize()
	{
		float len = Length();
		if (!NearZero(len))
		{
			x /= len;
			y /= len;
		}
	}

	static Vector2 Normalize(const Vector2& vec)
	{
		Vector2 temp = vec;
		temp.Normalize();
		return temp;
	}

	static float Dot(const Vector2& a, const Vector2& b)
	{
		return a.x * b.x + a.x * b.x;
	}

	float Length() const
	{
		return std::sqrtf(x * x + y * y);
	}

	static const Vector2 Zero;
	static const Vector2 UnitX;
	static const Vector2 UnitY;
	static const Vector2 NegUnitX;
	static const Vector2 NegUnitY;
};

}


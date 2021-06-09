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

class Vector2
{
public:
	float x;
	float y;
	
	Vector2() : x{ 0.f }, y{ 0.f } {}
	explicit Vector2(float x, float y) : x{ x }, y{ y } {}
};

}


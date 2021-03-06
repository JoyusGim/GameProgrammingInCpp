#pragma once
#include <random>
#include "Math.h"

class Random
{
	static std::mt19937 sGenerator;

public:
	static void Init();

	static void Seed(unsigned int seed);

	static float GetFloat();

	static float GetFloatRange(float min, float max);

	static int GetInt();

	static int GetIntRange(int min, int max);

	static Vector2 GetVector(const Vector2& min, const Vector2& max);
};


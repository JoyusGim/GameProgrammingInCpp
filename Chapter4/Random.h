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

	static Math::Vector2 GetVector(const Math::Vector2& min, const Math::Vector2& max);
};


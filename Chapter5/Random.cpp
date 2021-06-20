#include "Random.h"

std::mt19937 Random::sGenerator;

void Random::Init()
{
	std::random_device rd;
	Random::Seed(rd());
}

void Random::Seed(unsigned int seed)
{
	sGenerator.seed(seed);
}

float Random::GetFloat()
{
	return GetFloatRange(0.f, 1.f);
}

float Random::GetFloatRange(float min, float max)
{
	std::uniform_real_distribution<float> dist(min, max);
	return dist(sGenerator);
}

int Random::GetInt()
{
	return GetIntRange(0, 1);
}

int Random::GetIntRange(int min, int max)
{
	std::uniform_int_distribution<int> dist(min, max);
	return dist(sGenerator);
}

Vector2 Random::GetVector(const Vector2& min, const Vector2& max)
{
	Vector2 v(GetFloat(), GetFloat());
	return min + (max - min) * v;
}

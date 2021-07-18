#pragma once
#include <vector>
#include <string>
#include "Collision.h"

class Mesh
{
	std::vector<class Texture*> mTextures;
	class VertexArray* mVertexArray;
	std::string mShaderName;
	float mRadius;
	float mSpecPower;
	AABB mBox;

public:
	Mesh();
	~Mesh();

	bool Load(const std::string& fileName, class Renderer* game);
	void Unload();

	class VertexArray* GetVertexArray() const;
	class Texture* GetTexture(size_t index) const;
	const std::string& GetShaderName() const;
	float GetRadius() const;
	float GetSpecPower() const;
	const AABB& GetBox() const;
};


#pragma once
#include <vector>
#include <string>
#include "Collision.h"
#include "VertexArray.h"

class Mesh
{
	std::string mFileName;
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

	void SaveBinary(const std::string& fileName, const void* verts,
		const uint32_t numVerts, VertexArray::Layout layout,
		const uint32_t* indices, uint32_t numIndices,
		const std::vector<std::string>& textureNames,
		const AABB& box, float radius, float specPower);
	bool LoadBinary(const std::string& fileName, class Renderer* renderer);

	class VertexArray* GetVertexArray() const;
	class Texture* GetTexture(size_t index) const;
	const std::string& GetShaderName() const;
	float GetRadius() const;
	float GetSpecPower() const;
	const AABB& GetBox() const;
};


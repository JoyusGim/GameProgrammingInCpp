#pragma once
#include <vector>
#include <string>

class Mesh
{
	std::vector<class Texture*> mTextures;
	class VertexArray* mVertexArray;
	std::string mShaderName;
	float mRadius;
	float mSpecPower;

public:
	Mesh();
	~Mesh();

	bool Load(const std::string& fileName, class Game* game);
	void Unload();

	class VertexArray* GetVertexArray() const;
	class Texture* GetTexture(size_t index) const;
	const std::string& GetShaderName() const;
	float GetRadius() const;
	float GetSpecPower() const;
};


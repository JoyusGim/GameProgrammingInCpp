#pragma once
#include <vector>

class GBuffer
{
public:
	enum Type
	{
		Diffuse = 0,
		Normal,
		WorldPos,
		Specular,
		NUM_GBUFFER_TEXTURES
	};

private:
	std::vector<class Texture*> mTextures;
	unsigned int mBufferID;

public:	
	bool Create(int width, int height);
	void Destroy();

	class Texture* GetTexture(Type type);
	unsigned int GetBufferID() const;
	void SetTexturesActive();
};


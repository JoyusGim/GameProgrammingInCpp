#pragma once
#include <string>

class Texture
{
	unsigned int mTextureID;
	int mWidth;
	int mHeight;

public:
	Texture();
	~Texture();

	bool Load(const std::string& fileName);
	void Unload();

	void SetActive();
	int GetWidth() const;
	int GetHeight() const;
	unsigned int GetTextureID() const;

	void CreateFromSurface(struct SDL_Surface* surface);
	void CreateForRendering(int width, int height, unsigned int format);
};


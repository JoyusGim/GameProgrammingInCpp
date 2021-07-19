#pragma once
#include <string>

class Texture
{
	unsigned int mTextureID;
	int mWidth;
	int mHeight;
	std::string mFileName;

public:
	Texture();
	~Texture();

	bool Load(const std::string& fileName);
	void Unload();

	void SetActive(int index = 0);
	int GetWidth() const;
	int GetHeight() const;
	unsigned int GetTextureID() const;

	void CreateFromSurface(struct SDL_Surface* surface);
	void CreateForRendering(int width, int height, unsigned int format);

	const std::string& GetFileName() const;
};


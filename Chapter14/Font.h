#pragma once
#include <SDL_ttf.h>
#include <unordered_map>
#include <string>
#include "Math.h"

class Font
{
	class Game* mGame;
	std::unordered_map<int, TTF_Font*> mFontData;

public:
	Font(class Game* game);
	~Font();

	bool Load(const std::string& fileName);
	void Unload();

	class Texture* RenderText(const std::string& textKey, const Vector3& color = Color::White, int pointSize = 30);
};


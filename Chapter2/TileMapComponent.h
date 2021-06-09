#pragma once
#include "SpriteComponent.h"
#include "Math.h"
#include <string>
#include <vector>

class TileMapComponent :
    public SpriteComponent
{
private:
	std::vector<int> mTileMapIdx;
	int mTileMapWidth;
	int mTileMapHeight;
	int mAtlasTileNumX;
	int mAtlasTileNumY;
	float mTileWidth;
	float mTileHeight;
	Math::Vector2 mScreeSize;

public:
	TileMapComponent(class Actor* owner, int drawOrder = 100);
	~TileMapComponent();

	void Draw(struct SDL_Renderer* renderer) override;

	void SetScreenSize(const Math::Vector2& screenSize);
	void LoadCSV(const std::string& fileName);
	void SetAtlasTexture(SDL_Texture* texture, int numX, int numY);
};


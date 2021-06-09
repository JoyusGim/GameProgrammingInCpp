#include "TileMapComponent.h"
#include "Actor.h"
#include <iostream>
#include <fstream>
#include <sstream>


TileMapComponent::TileMapComponent(Actor* owner, int drawOrder)	:
	SpriteComponent(owner, drawOrder),
	mTileMapWidth{ 0 },
	mTileMapHeight{ 0 },
	mAtlasTileNumX{ 0 },
	mAtlasTileNumY{ 0 }
{
}

TileMapComponent::~TileMapComponent()
{
}

void TileMapComponent::Draw(SDL_Renderer* renderer)
{
	for (int h = 0; h < mTileMapHeight; h++)
	{
		for (int w = 0; w < mTileMapWidth; w++)
		{
			int idx = mTileMapIdx[h * mTileMapWidth + w];
			if (idx == -1)
				continue;

			SDL_Rect rc;
			rc.w = static_cast<int>(mTileWidth);
			rc.h = static_cast<int>(mTileHeight);
			rc.x = static_cast<int>(w * rc.w);
			rc.y = static_cast<int>(h * rc.h);

			SDL_Rect tileRc;
			tileRc.w = mTileWidth;
			tileRc.h = mTileHeight;
			tileRc.x = static_cast<int>((idx % mAtlasTileNumX) * tileRc.w);
			tileRc.y = static_cast<int>((idx / mAtlasTileNumX) * tileRc.h);


			SDL_RenderCopy(renderer,
				mTexture,
				&tileRc,
				&rc
			);
		}
	}
}

void TileMapComponent::SetScreenSize(const Math::Vector2& screenSize)
{
	mScreeSize = screenSize;
}

void TileMapComponent::LoadCSV(const std::string& fileName)
{
	mTileMapWidth = 0;
	mTileMapHeight = 0;

	std::ifstream ifs(fileName);
	if (!ifs.is_open())
	{
		std::cerr << "Failed to load CSV file " << fileName;
		std::terminate();
	}

	std::string line;
	while (std::getline(ifs, line))
	{
		mTileMapWidth = 0;
		std::stringstream ss(line);
		std::string num;
		while (std::getline(ss, num, ','))
		{
			mTileMapIdx.push_back(std::stoi(num));
			mTileMapWidth++;
		}
		mTileMapHeight++;
	}
}

void TileMapComponent::SetAtlasTexture(SDL_Texture* texture, int numX, int numY)
{
	SetTexture(texture);
	mAtlasTileNumX = numX;
	mAtlasTileNumY = numY;

	mTileWidth = mTexWidth / mAtlasTileNumX;
	mTileHeight = mTexHeight / mAtlasTileNumY;
}
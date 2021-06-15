#include "Tile.h"
#include "Game.h"
#include "SpriteComponent.h"
#include <string>

Tile::Tile(class Game* game) :
	Actor(game),
	mParent{ nullptr },
	f{ 0.f }, g{ 0.f }, h{ 0.f },
	mInOpenSet{ false }, mInClosedSet{ false }, mBlocked{ false },
	mState{ TileState::Default }, mSelected{ false }
{
	mSprite = new SpriteComponent(this);
	UpdateTexture();
}

void Tile::UpdateTexture()
{
	std::string file;
	switch (mState)
	{
	case Tile::Path:
		if (mSelected)
			file = "Assets/TileGreySelected.png";
		else
			file = "Assets/TileGrey.png";
		break;
	case Tile::Start:
		file = "Assets/TileTan.png";
		break;
	case Tile::End:
		file = "Assets/TileGreen.png";
		break;
	case Tile::Default:
	default:
		if (mSelected)
			file = "Assets/TileBrownSelected.png";
		else
			file = "Assets/TileBrown.png";
		break;
	}
	mSprite->SetTexture(GetGame()->GetTexture(file));
}

void Tile::SetTileState(TileState state)
{
	mState = state;
	UpdateTexture();
}

Tile::TileState Tile::GetTileState() const
{
	return mState;
}

void Tile::ToggleSelect()
{
	mSelected = !mSelected;
	UpdateTexture();
}

const Tile* Tile::GetParent() const
{
	return mParent;
}

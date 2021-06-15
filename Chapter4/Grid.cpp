#include "Grid.h"
#include "Tile.h"
#include "Enemy.h"
#include "Tower.h"
#include <algorithm>
#include <SDL.h>

Grid::Grid(class Game* game)	:
	Actor(game),
	mSelectedTile{ nullptr }
{
	mTiles.resize(NumRows);

	for (size_t i = 0; i < mTiles.size(); i++)
	{
		mTiles[i].resize(NumCols);
		for (size_t j = 0; j < mTiles[i].size(); j++)
		{
			mTiles[i][j] = new Tile(GetGame());
			mTiles[i][j]->SetPosition(Math::Vector2(TileSize / 2.f + j * TileSize, StartY + i * TileSize));
		}
	}

	GetStartTile()->SetTileState(Tile::TileState::Start);
	GetEndTile()->SetTileState(Tile::TileState::End);

	for (size_t i = 0; i < mTiles.size(); i++)
	{
		for (size_t j = 0; j < mTiles[i].size(); j++)
		{
			if (i > 0)
			{
				mTiles[i][j]->mAdjacent.push_back(mTiles[i - 1][j]);
			}
			if (i < NumRows - 1)
			{
				mTiles[i][j]->mAdjacent.push_back(mTiles[i + 1][j]);
			}
			if (j > 0)
			{
				mTiles[i][j]->mAdjacent.push_back(mTiles[i][j - 1]);
			}
			if (j < NumCols - 1)
			{
				mTiles[i][j]->mAdjacent.push_back(mTiles[i][j + 1]);
			}
		}
	}

	FindPath(GetEndTile(), GetStartTile());
	UpdatePathTiles();
}

bool Grid::FindPath(Tile* start, Tile* end)
{
	for(auto r : mTiles)
		for (auto tile : r)
		{
			tile->g = 0.f;
			tile->mInOpenSet = false;
			tile->mInClosedSet = false;
		}

	std::vector<Tile*> openSet;
	Tile* current = start;
	current->mInClosedSet = true;

	do
	{
		for (Tile* neighbor : current->mAdjacent)
		{
			if (neighbor->mBlocked)
				continue;

			if (!neighbor->mInClosedSet)
			{
				if (!neighbor->mInOpenSet)
				{
					neighbor->mParent = current;
					neighbor->h = (neighbor->GetPosition() - end->GetPosition()).Length();
					neighbor->g = current->g + TileSize;
					neighbor->f = neighbor->h + neighbor->g;
					neighbor->mInOpenSet = true;
					openSet.emplace_back(neighbor);
				}
				else
				{
					float newG = current->g + TileSize;
					if (newG < neighbor->g)
					{
						neighbor->mParent = current;
						neighbor->g = newG;
						neighbor->f = neighbor->h + neighbor->g;
					}
				}
			}
		}

		if (openSet.empty())
			break;

		auto iter = std::min_element(openSet.begin(), openSet.end(),
			[](Tile* a, Tile* b) {
				return a->f < b->f;
			});

		current = *iter;
		openSet.erase(iter);
		current->mInOpenSet = false;
		current->mInClosedSet = true;
	} while (current != end);

	return (current == end);
}

void Grid::UpdatePathTiles()
{
	for (size_t i = 0; i < mTiles.size(); i++)
	{
		for (size_t j = 0; j < mTiles[i].size(); j++)
		{
			if(!(i == 3 && j == 0) && !(i == 3 && j == 15))
				mTiles[i][j]->SetTileState(Tile::TileState::Default);
		}
	}

	Tile* tile = GetStartTile()->mParent;
	while (tile != GetEndTile())
	{
		tile->SetTileState(Tile::TileState::Path);
		tile = tile->mParent;
	}
}

void Grid::UpdateActor(float deltaTime)
{
	static float enemyTimer = EnemyTime;
	Actor::UpdateActor(deltaTime);
	
	enemyTimer -= deltaTime;
	if (enemyTimer <= 0.f)
	{
		Enemy* enemy = new Enemy(GetGame());
		//enemy->SetPosition(GetStartTile()->GetPosition());

		enemyTimer += EnemyTime;
	}
}

void Grid::ActorInput(const uint8_t* keyState)
{
	Actor::ActorInput(keyState);

	int x, y;
	Uint32 buttons = SDL_GetMouseState(&x, &y);
	if (SDL_BUTTON(buttons) & SDL_BUTTON_LEFT)
	{
		ProcessClick(x, y);
	}

	if (keyState[SDL_SCANCODE_B])
	{
		BuildTower();
	}
}

void Grid::BuildTower()
{
	if (mSelectedTile && !mSelectedTile->mBlocked)
	{
		mSelectedTile->mBlocked = true;
		if (FindPath(GetEndTile(), GetStartTile()))
		{
			Tower* t = new Tower(GetGame());
			t->SetPosition(mSelectedTile->GetPosition());
		}
		else
		{
			mSelectedTile->mBlocked = false;
			FindPath(GetEndTile(), GetStartTile());
		}
		UpdatePathTiles();
	}	
}

void Grid::SelectTile(size_t row, size_t col)
{
	if (col < 0 || col >= NumCols || row < 0 || row >= NumRows)
		return;
	Tile::TileState state = mTiles[row][col]->GetTileState();
	if (state != Tile::TileState::Start && state != Tile::TileState::End)
	{
		if (mSelectedTile)
		{
			mSelectedTile->ToggleSelect();
		}
		mSelectedTile = mTiles[row][col];
		mSelectedTile->ToggleSelect();
	}
}

void Grid::ProcessClick(int x, int y)
{
	y -= (StartY - TileSize / 2.f);	

	size_t row = y / TileSize;
	size_t col = x / TileSize;
	SelectTile(row, col);
}

Tile* Grid::GetStartTile() const
{
	return mTiles[3][0];
}

Tile* Grid::GetEndTile() const
{
	return mTiles[3][15];
}
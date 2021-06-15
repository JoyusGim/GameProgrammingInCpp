#pragma once
#include "Actor.h"
#include <vector>

class Grid :
    public Actor
{
    const size_t NumRows = 7;       //
    const size_t NumCols = 16;      // 타일 개수
    const float StartY = 192.f;     // 화면에서 타일맵의 위치
    const float TileSize = 64.f;    // 타일 사이즈
    const float EnemyTime = 1.5f;   // 적 생성 간격

    std::vector<std::vector<class Tile*>> mTiles;
    class Tile* mSelectedTile;

    bool FindPath(Tile* start, Tile* end);
    void UpdatePathTiles();

public:
    Grid(class Game* game);

    void UpdateActor(float deltaTime) override;
    void ActorInput(const uint8_t* keyState) override;
    void BuildTower();
    void SelectTile(size_t row, size_t col);
    void ProcessClick(int x, int y);

    Tile* GetStartTile() const;
    Tile* GetEndTile() const;

    std::vector<std::vector<class Tile*>>& GetTiles() { return mTiles; }
};


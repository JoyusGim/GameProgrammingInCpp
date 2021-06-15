#pragma once
#include "Actor.h"
#include <vector>

class Grid :
    public Actor
{
    const size_t NumRows = 7;       //
    const size_t NumCols = 16;      // Ÿ�� ����
    const float StartY = 192.f;     // ȭ�鿡�� Ÿ�ϸ��� ��ġ
    const float TileSize = 64.f;    // Ÿ�� ������
    const float EnemyTime = 1.5f;   // �� ���� ����

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


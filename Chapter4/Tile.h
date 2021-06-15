#pragma once
#include "Actor.h"
#include <vector>

class Tile :
    public Actor
{
public:
    enum TileState
    {
        Default,
        Path,
        Start,
        End
    };

private:
    friend class Grid;
    std::vector<Tile*> mAdjacent;
    Tile* mParent;
    float f;
    float g;
    float h;
    bool mInOpenSet;
    bool mInClosedSet;
    bool mBlocked;
    TileState mState;
    bool mSelected;
    class SpriteComponent* mSprite;

    void UpdateTexture();

public:
    Tile(class Game* game);

    void SetTileState(TileState state);
    TileState GetTileState() const;
    void ToggleSelect();
    const Tile* GetParent() const;
};


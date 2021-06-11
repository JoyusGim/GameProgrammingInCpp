#pragma once
#include "SpriteComponent.h"
#include "Math.h"
#include <vector>

class BGSpriteComponent :
    public SpriteComponent
{
    struct BGTexture
    {
        SDL_Texture* mTexture;
        Math::Vector2 mOffset;
        BGTexture() : mTexture{ nullptr } {}
    };
    std::vector<BGTexture> mBGTextures;
    Math::Vector2 mScreenSize;
    float mScrollSpeed;

public:
    BGSpriteComponent(class Actor* owner, int draworder = 10);
    void Update(float deltaTime) override;
    void Draw(SDL_Renderer* renderer) override;

    void SetBGTextures(const std::vector<SDL_Texture*>& textures);
    void SetScreenSize(const Math::Vector2& size);
    void SetScrollSpeed(float speed);
    float GetScrollSpeed() const;
};


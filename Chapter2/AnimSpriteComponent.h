#pragma once
#include "SpriteComponent.h"
#include <vector>

class AnimSpriteComponent :
    public SpriteComponent
{
    std::vector<struct SDL_Texture*> mAnimTextures;
    float mCurrFrame;
    float mAnimFPS;

public:
    AnimSpriteComponent(class Actor* owner, int drawOrder = 100);
    virtual ~AnimSpriteComponent();

    void Update(float deltaTime);

    void SetAnimTextures(const std::vector<struct SDL_Texture*>& textures);
    float GetAnimFPS() const;
    float SetAnimFPS(float fps);
};


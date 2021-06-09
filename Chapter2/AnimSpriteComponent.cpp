#include "AnimSpriteComponent.h"

AnimSpriteComponent::AnimSpriteComponent(Actor* owner, int drawOrder)   :
    SpriteComponent(owner, drawOrder),
    mCurrFrame{ 0.f },
    mAnimFPS{ 24.f }
{
}

AnimSpriteComponent::~AnimSpriteComponent()
{
}

void AnimSpriteComponent::Update(float deltaTime)
{
    SpriteComponent::Update(deltaTime);

    if (mAnimTextures.size() > 0)
    {
        // 애니메이션 프레임 레이트와 델타 시간을 기반으로
        // 현재 프레임 갱신
        mCurrFrame += mAnimFPS * deltaTime;

        // 애니메이션 텍스처 수를 추과하면 현재 프레임을 래핑한다.
        while (mCurrFrame >= mAnimTextures.size())
        {
            mCurrFrame -= mAnimTextures.size();
        }

        SpriteComponent::SetTexture(mAnimTextures[static_cast<int>(mCurrFrame)]);
    }
}

void AnimSpriteComponent::SetAnimTextures(const std::vector<struct SDL_Texture*>& textures)
{
    mAnimTextures = textures;
}

float AnimSpriteComponent::GetAnimFPS() const
{
    return 0.0f;
}

float AnimSpriteComponent::SetAnimFPS(float fps)
{
    return 0.0f;
}

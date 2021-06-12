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
        // �ִϸ��̼� ������ ����Ʈ�� ��Ÿ �ð��� �������
        // ���� ������ ����
        mCurrFrame += mAnimFPS * deltaTime;

        // �ִϸ��̼� �ؽ�ó ���� �߰��ϸ� ���� �������� �����Ѵ�.
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

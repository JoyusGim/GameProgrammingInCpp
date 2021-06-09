#include "SpriteComponent.h"
#include "Actor.h"
#include "Game.h"

SpriteComponent::SpriteComponent(Actor* owner, int drawOrder)	:
	Component(owner),
	mDrawOrder{ drawOrder },
	mTexture{ nullptr },
	mTexWidth{ 0 },
	mTexHeight{ 0 }
{
	mOwner->GetGame()->AddSpriteComponent(this);
}

SpriteComponent::~SpriteComponent()
{
	mOwner->GetGame()->RemoveSpriteComponent(this);
}

void SpriteComponent::Draw(SDL_Renderer* renderer)
{
	if (mTexture)
	{
		SDL_Rect r;

		// owner�� �����Ͽ� �̹��� ũ�⸦ �����.
		r.w = static_cast<int>(mTexWidth * mOwner->GetScale());
		r.h = static_cast<int>(mTexHeight * mOwner->GetScale());

		// owner�� ��ġ�� �߽����� �簢�� �̵�
		r.x = static_cast<int>(mOwner->GetPosition().x - r.w / 2);
		r.y = static_cast<int>(mOwner->GetPosition().y - r.h / 2);

		// ��������Ʈ�� �׸���.
		SDL_RenderCopyEx(renderer,
			mTexture,
			nullptr,
			&r,
			Math::RAD2DEG(mOwner->GetRotate()),
			nullptr,
			SDL_FLIP_NONE
		);
	}
}

void SpriteComponent::SetTexture(SDL_Texture* texture)
{
	mTexture = texture;
	SDL_QueryTexture(texture, nullptr, nullptr, &mTexWidth, &mTexHeight);
}

int SpriteComponent::GetDrawOrder() const
{
	return mDrawOrder;
}

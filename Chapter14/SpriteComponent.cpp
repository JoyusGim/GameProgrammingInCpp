#include "SpriteComponent.h"
#include "Shader.h"
#include "Actor.h"
#include "Game.h"
#include "Renderer.h"
#include "Texture.h"

SpriteComponent::SpriteComponent(Actor* owner, int drawOrder)	:
	Component(owner),
	mDrawOrder{ drawOrder },
	mTexture{ nullptr },
	mTexWidth{ 0 },
	mTexHeight{ 0 }
{
	mOwner->GetGame()->GetRenderer()->AddSpriteComponent(this);
}

SpriteComponent::~SpriteComponent()
{
	mOwner->GetGame()->GetRenderer()->RemoveSpriteComponent(this);
}

void SpriteComponent::Draw(Shader* shader)
{
	if (mTexture)
	{
		Matrix4 scaleMat = Matrix4::CreateScale(
			static_cast<float>(mTexWidth),
			static_cast<float>(mTexHeight), \
			1.f
		);

		Matrix4 world = scaleMat * mOwner->GetWorldTransform();
		shader->SetMatrixUniform("uWorldTransform", world);

		mTexture->SetActive();
		glDrawElements(
			GL_TRIANGLES,
			6,
			GL_UNSIGNED_INT,
			nullptr
		);
	}
}

void SpriteComponent::SetTexture(Texture* texture)
{
	mTexture = texture;
	mTexWidth = mTexture->GetWidth();
	mTexHeight = mTexture->GetHeight();
}

int SpriteComponent::GetDrawOrder() const
{
	return mDrawOrder;
}

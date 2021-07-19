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

void SpriteComponent::LoadProperties(const rapidjson::Value& inObj)
{
	Component::LoadProperties(inObj);

	std::string texFile;
	if (JsonHelper::Get<std::string>(inObj, "textureFile", texFile))
	{
		SetTexture(mOwner->GetGame()->GetRenderer()->GetTexture(texFile));
	}

	JsonHelper::Get<int>(inObj, "drawOrder", mDrawOrder);
}

void SpriteComponent::SaveProperties(rapidjson::Document::AllocatorType& alloc, rapidjson::Value& inObj) const
{
	Component::SaveProperties(alloc, inObj);

	if (mTexture)
	{
		JsonHelper::Add<std::string>(alloc, inObj, "textureFile", mTexture->GetFileName());
	}

	JsonHelper::Add<int>(alloc, inObj, "drawOrder", mDrawOrder);
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

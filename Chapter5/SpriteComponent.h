#pragma once
#include "Component.h"
#include <SDL.h>

class SpriteComponent : public Component
{
protected:
	class Texture* mTexture;
	int mDrawOrder;
	int mTexWidth;
	int mTexHeight;

public:
	SpriteComponent(class Actor* owner, int drawOrder = 100);
	virtual ~SpriteComponent();

	virtual void Draw(class Shader* shader);

	void SetTexture(class Texture* texture);
	int GetDrawOrder() const;
};


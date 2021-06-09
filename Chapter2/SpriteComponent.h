#pragma once
#include "Component.h"
#include <SDL.h>

class SpriteComponent : public Component
{
protected:
	struct SDL_Texture* mTexture;
	int mDrawOrder;
	int mTexWidth;
	int mTexHeight;

public:
	SpriteComponent(class Actor* owner, int drawOrder = 100);
	~SpriteComponent();

	virtual void Draw(struct SDL_Renderer* renderer);

	void SetTexture(struct SDL_Texture* texture);
	int GetDrawOrder() const;
};


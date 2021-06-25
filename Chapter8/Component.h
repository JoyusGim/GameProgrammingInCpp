#pragma once
#include <SDL.h>

class Component
{
protected:
	class Actor* mOwner;
	int mUpdateOrder;

public:
	Component(class Actor* owner, int updateOrder = 100);
	virtual ~Component();

	virtual void Update(float deltaTime) {}
	virtual void ProcessInput(const struct InputState& keyState) {}
	virtual void OnUpdateWorldTransform() {}

	int GetUpdateOrder() const;
};


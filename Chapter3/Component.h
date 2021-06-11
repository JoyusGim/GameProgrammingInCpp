#pragma once
class Component
{
protected:
	class Actor* mOwner;
	int mUpdateOrder;

public:
	Component(class Actor* owner, int updateOrder = 100);
	virtual ~Component();

	virtual void Update(float deltaTime) {}

	int GetUpdateOrder() const;
};


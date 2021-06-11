#pragma once

#include <vector>
#include "Math.h"



class Actor
{
public:
	enum class State
	{
		ACTIVE,
		PAUSE,
		DEAD
	};

private:
	Math::Vector2 mPosition;
	float mScale;
	float mRotate;
	State mState;
	std::vector<class Component*> mComponents;
	class Game* mGame;

public:
	Actor(class Game* game);
	virtual ~Actor();

	void Update(float deltaTime);
	void UpdateComponents(float deltaTime);
	virtual void UpdateActor(float deltaTime) {}
	void ProcessInput(const uint8_t* keyState);
	virtual void ActorInput(const uint8_t* keyState) {}

	void AddComponent(class Component* component);
	void RemoveComponent(class Component* component);

	void SetPosition(float x, float y);
	void SetPosition(const Math::Vector2& position);
	Math::Vector2 GetPosition() const;
	void SetScale(float scale);
	float GetScale() const;
	void SetRotate(float rotate);
	float GetRotate() const;
	void SetState(State state);
	State GetState() const;
	Math::Vector2 GetForward() const;
	class Game* GetGame() const;
};
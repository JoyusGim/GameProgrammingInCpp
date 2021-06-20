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
	Vector2 mPosition;
	float mScale;
	float mRotate;
	Matrix4 mWorldTransform;
	bool mRecomputeWorldTransform;
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

	void ComputeWorldTransform();

	void AddComponent(class Component* component);
	void RemoveComponent(class Component* component);

	void SetPosition(float x, float y);
	void SetPosition(const Vector2& position);
	const Vector2& GetPosition() const;
	void SetScale(float scale);
	float GetScale() const;
	void SetRotate(float rotate);
	float GetRotate() const;
	const Matrix4& GetWorldTransform() const;
	void SetState(State state);
	State GetState() const;
	Vector2 GetForward() const;
	class Game* GetGame() const;
};
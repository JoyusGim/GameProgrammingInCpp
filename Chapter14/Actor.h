#pragma once

#include <vector>
#include "Math.h"
#include <rapidjson\document.h>
#include "Component.h"

class Actor
{
public:
	enum class State
	{
		ACTIVE,
		PAUSE,
		DEAD
	};

	enum TypeID
	{
		ACTOR = 0,
		BALLACTOR,
		CAMERAACTOR,
		FOLLOWACTOR,
		FPSACTOR,
		ORBITACTOR,
		PLANEACTOR,
		SPLINEACTOR,
		TARGETACTOR,
		NUM_ACTOR_TYPES
	};

	static const char* sTypeNames[NUM_ACTOR_TYPES];

private:
	Vector3 mPosition;
	Quaternion mRotation;
	float mScale;
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
	void ProcessInput(const struct InputState& keyState);
	virtual void ActorInput(const struct InputState& keyState) {}

	void ComputeWorldTransform();

	void AddComponent(class Component* component);
	const std::vector<class Component*>& GetComponents() const;
	void RemoveComponent(class Component* component);

	void SetPosition(float x, float y);
	void SetPosition(const Vector3& position);
	const Vector3& GetPosition() const;
	void SetScale(float scale);
	float GetScale() const;
	void SetRotate(const Quaternion& rotate);
	const Quaternion& GetRotate() const;
	const Matrix4& GetWorldTransform() const;
	void SetState(State state);
	State GetState() const;
	Vector3 GetForward() const;
	Vector3 GetRight() const;
	class Game* GetGame() const;

	void RotateToNewForward(const Vector3& forward);

	virtual void LoadProperties(const rapidjson::Value& inObj);
	virtual void SaveProperties(rapidjson::Document::AllocatorType& alloc, rapidjson::Value& inObj) const;

	template <typename T>
	static Actor* Create(class Game* game, const rapidjson::Value& inObj)
	{
		T* t = new T(game);
		t->LoadProperties(inObj);
		return t;
	}

	Component* GetComponentOfType(Component::TypeID type);

	virtual TypeID GetType() const { return ACTOR; }
};
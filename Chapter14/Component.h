#pragma once
#include <SDL.h>
#include <rapidjson\document.h>
#include "LevelLoader.h"

class Component
{
protected:
	class Actor* mOwner;
	int mUpdateOrder;

public:
	enum TypeID
	{
		COMPONENT = 0,
		AudioComponent,
		BallMove,
		BoxComponent,
		CameraComponent,
		FollowCamera,
		FPSCamera,
		MeshComponent,
		MirrorCamera,
		MoveComponent,
		OrbitCamera,
		PointLightComponent,
		SkeletalMeshComponent,
		SplineCamera,
		SpriteComponent,
		TargetComponent,
		NUM_COMPONENT_TYPES
	};

	static const char* sTypeNames[NUM_COMPONENT_TYPES];

	Component(class Actor* owner, int updateOrder = 100);
	virtual ~Component();

	virtual void Update(float deltaTime) {}
	virtual void ProcessInput(const struct InputState& keyState) {}
	virtual void OnUpdateWorldTransform() {}
	virtual void LoadProperties(const rapidjson::Value& inObj);
	virtual void SaveProperties(rapidjson::Document::AllocatorType& alloc, rapidjson::Value& inObj) const;

	int GetUpdateOrder() const;

	class Actor* GetOwner() const;
	virtual	TypeID GetType() const { return COMPONENT; }

	template <typename T>
	static Component* Create(class Actor* owner, const rapidjson::Value& inObj)
	{
		T* t = new T(owner);
		t->LoadProperties(inObj);
		return t;
	}
};


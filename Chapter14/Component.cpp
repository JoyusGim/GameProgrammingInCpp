#include "Component.h"
#include "Actor.h"

const char* Component::sTypeNames[Component::NUM_COMPONENT_TYPES] = {
	"Component",
	"AudioComponent",
	"BallMove",
	"BoxComponent",
	"CameraComponent",
	"FollowCamera",
	"FPSCamera",
	"MeshComponent",
	"MirrorCamera",
	"MoveComponent",
	"OrbitCamera",
	"PointLightComponent",
	"SkeletalMeshComponent",
	"SplineCamera",
	"SpriteComponent",
	"TargetComponent"
};

Component::Component(Actor* owner, int updateOrder)	:
	mOwner{ owner },
	mUpdateOrder{ updateOrder }
{
	mOwner->AddComponent(this);
}

Component::~Component()
{
	mOwner->RemoveComponent(this);
}


void Component::LoadProperties(const rapidjson::Value& inObj)
{
	JsonHelper::Get<int>(inObj, "updateOrder", mUpdateOrder);
}

void Component::SaveProperties(rapidjson::Document::AllocatorType& alloc, rapidjson::Value& inObj) const
{
	JsonHelper::Add<int>(alloc, inObj, "updateOrder", mUpdateOrder);
}

int Component::GetUpdateOrder() const
{
	return mUpdateOrder;
}

Actor* Component::GetOwner() const
{
	return mOwner;
}

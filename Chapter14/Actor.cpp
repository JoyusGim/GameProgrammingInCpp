#include "Actor.h"
#include "Game.h"
#include "Component.h"
#include "InputSystem.h"
#include "LevelLoader.h"

const char* Actor::sTypeNames[NUM_ACTOR_TYPES] = {
	"Actor",
	"BallActor",
	"CameraActor",
	"FollowActor",
	"FPSActor",
	"OrbitActor",
	"PlaneActor",
	"SplineActor",
	"TargetActor"
};

Actor::Actor(Game* game)	:
	mGame{ game },
	mScale{ 1.f },
	mPosition{ Vector3::Zero },
	mRotation{ Quaternion::Identity },
	mRecomputeWorldTransform{ true },
	mState{ State::ACTIVE }
{
	mGame->AddActor(this);
}

Actor::~Actor()
{
	mGame->RemoveActor(this);

	while (!mComponents.empty())
	{
		delete mComponents.back();
	}
}

void Actor::Update(float deltaTime)
{
	if (mState == State::ACTIVE)
	{
		ComputeWorldTransform();

		UpdateComponents(deltaTime);
		UpdateActor(deltaTime);

		ComputeWorldTransform();
	}
}

void Actor::UpdateComponents(float deltaTime)
{
	for (auto comp : mComponents)
	{
		comp->Update(deltaTime);
	}
}

void Actor::ProcessInput(const InputState& keyState)
{
	if (mState == State::ACTIVE)
	{
		for (auto comp : mComponents)
		{
			comp->ProcessInput(keyState);
		}
		ActorInput(keyState);
	}
}

void Actor::ComputeWorldTransform()
{
	if (mRecomputeWorldTransform)
	{
		mRecomputeWorldTransform = false;
		mWorldTransform = Matrix4::CreateScale(mScale);
		mWorldTransform *= Matrix4::CreateFromQuaternion(mRotation);
		mWorldTransform *= Matrix4::CreateTranslation(mPosition);

		for (auto comp : mComponents)
		{
			comp->OnUpdateWorldTransform();
		}
	}
}

void Actor::AddComponent(class Component* component)
{
	int order = component->GetUpdateOrder();
	auto iter = mComponents.begin();
	for (; iter != mComponents.end(); ++iter)
	{
		if (order < (*iter)->GetUpdateOrder())
			break;
	}

	mComponents.insert(iter, component);
}

const std::vector<class Component*>& Actor::GetComponents() const
{
	return mComponents;
}

void Actor::RemoveComponent(class Component* component)
{
	auto iter = std::find(mComponents.begin(), mComponents.end(), component);
	if (iter != mComponents.end())
	{
		mComponents.erase(iter);
	}
}

void Actor::SetPosition(float x, float y)
{
	mPosition.x = x;
	mPosition.y = y;
	mRecomputeWorldTransform = true;
}

void Actor::SetPosition(const Vector3& position)
{
	mPosition = position;
	mRecomputeWorldTransform = true;
}

const Vector3& Actor::GetPosition() const
{
	return mPosition;
}

void Actor::SetScale(float scale)
{
	mScale = scale;
	mRecomputeWorldTransform = true;
}

float Actor::GetScale() const
{
	return mScale;
}

void Actor::SetRotate(const Quaternion& rotate)
{
	mRotation = rotate;
	mRecomputeWorldTransform = true;
}

const Quaternion& Actor::GetRotate() const
{
	return mRotation;
}

const Matrix4& Actor::GetWorldTransform() const
{
	return mWorldTransform;
}

void Actor::SetState(State state)
{
	this->mState = state;
}

Actor::State Actor::GetState() const
{
	return mState;
}

Vector3 Actor::GetForward() const
{
	return Vector3::Transform(Vector3::UnitX, Matrix4::CreateFromQuaternion(mRotation));
}

Vector3 Actor::GetRight() const
{
	return Vector3::Transform(Vector3::UnitY, mRotation);
}

class Game* Actor::GetGame() const
{
	return mGame;
}

void Actor::RotateToNewForward(const Vector3& forward)
{
	float dot = Vector3::Dot(Vector3::UnitX, forward);
	float angle = Math::Acos(dot);

	if (dot > 0.9999f)
		SetRotate(Quaternion::Identity);
	else if (dot < -0.9999f)
		SetRotate(Quaternion(Vector3::UnitZ, Math::Pi));
	else
	{
		Vector3 axis = Vector3::Cross(Vector3::UnitX, forward);
		axis.Normalize();
		SetRotate(Quaternion(axis, angle));
	}
}

void Actor::LoadProperties(const rapidjson::Value& inObj)
{
	std::string state;
	if (JsonHelper::Get<std::string>(inObj, "state", state))
	{
		if (state == "active")
			SetState(State::ACTIVE);
		else if (state == "paused")
			SetState(State::PAUSE);
		else if (state == "dead")
			SetState(State::DEAD);
		else
			SDL_Log("Unknown Actor state %s", state.c_str());
	}

	JsonHelper::Get<Vector3>(inObj, "position", mPosition);
	JsonHelper::Get<Quaternion>(inObj, "rotation", mRotation);
	JsonHelper::Get<float>(inObj, "scale", mScale);
	ComputeWorldTransform();
}

void Actor::SaveProperties(rapidjson::Document::AllocatorType& alloc, rapidjson::Value& inObj) const
{
	std::string state = "active";
	if (mState == State::PAUSE)
		state = "paused";
	else if (mState == State::DEAD)
		state = "dead";

	JsonHelper::Add<std::string>(alloc, inObj, "state", state);
	JsonHelper::Add<Vector3>(alloc, inObj, "position", mPosition);
	JsonHelper::Add<Quaternion>(alloc, inObj, "rotation", mRotation);
	JsonHelper::Add<float>(alloc, inObj, "scale", mScale);
}

Component* Actor::GetComponentOfType(Component::TypeID type)
{
	Component* comp = nullptr;
	for (Component* c : mComponents)
	{
		if (c->GetType() == type)
		{
			comp = c;
			break;
		}
	}

	return comp;
}

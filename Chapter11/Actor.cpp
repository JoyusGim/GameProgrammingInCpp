#include "Actor.h"
#include "Game.h"
#include "Component.h"
#include "InputSystem.h"

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

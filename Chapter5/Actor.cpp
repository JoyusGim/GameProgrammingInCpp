#include "Actor.h"
#include "Game.h"
#include "Component.h"

Actor::Actor(Game* game)	:
	mGame{ game },
	mScale{ 1.f },
	mRotate{ 0.f },
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

void Actor::ProcessInput(const uint8_t* keyState)
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
		mWorldTransform = Matrix4::CreateScale(mScale);
		mWorldTransform *= Matrix4::CreateRotationZ(mRotate);
		mWorldTransform *= Matrix4::CreateTranslation(Vector3(mPosition.x, mPosition.y, 0.f));

		for (auto comp : mComponents)
		{
			comp->OnUpdateWorldTransform();
		}

		mRecomputeWorldTransform = false;
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

void Actor::SetPosition(const Vector2& position)
{
	mPosition = position;
	mRecomputeWorldTransform = true;
}

const Vector2& Actor::GetPosition() const
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

void Actor::SetRotate(float rotate)
{
	mRotate = rotate;
	mRecomputeWorldTransform = true;
}

float Actor::GetRotate() const
{
	return mRotate;
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

Vector2 Actor::GetForward() const
{
	Vector2 forward(cosf(mRotate), sinf(mRotate));

	return forward;
}

class Game* Actor::GetGame() const
{
	return mGame;
}

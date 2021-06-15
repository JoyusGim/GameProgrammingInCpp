#include "Actor.h"
#include "Game.h"
#include "Component.h"

Actor::Actor(Game* game)	:
	mGame{ game },
	mScale{ 1.f },
	mRotate{ 0.f },
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
	UpdateComponents(deltaTime);
	UpdateActor(deltaTime);
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
}

void Actor::SetPosition(const Math::Vector2& position)
{
	mPosition = position;
}

const Math::Vector2& Actor::GetPosition() const
{
	return mPosition;
}

void Actor::SetScale(float scale)
{
	mScale = scale;
}

float Actor::GetScale() const
{
	return mScale;
}

void Actor::SetRotate(float rotate)
{
	mRotate = rotate;
}

float Actor::GetRotate() const
{
	return mRotate;
}

void Actor::SetState(State state)
{
	this->mState = state;
}

Actor::State Actor::GetState() const
{
	return mState;
}

Math::Vector2 Actor::GetForward() const
{
	Math::Vector2 forward(cosf(mRotate), -sinf(mRotate));

	return forward;
}

class Game* Actor::GetGame() const
{
	return mGame;
}

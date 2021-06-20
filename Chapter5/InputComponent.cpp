#include "InputComponent.h"
#include "Actor.h"

InputComponent::InputComponent(Actor* actor)	:
	MoveComponent(actor),
	mMaxForwardSpeed{ 0.f },
	mMaxAngularSpeed{ 0.f },
	mForwardKey{ -1 },
	mBackKey{ -1 },
	mClockWiseKey{ -1 },
	mCounterClockWiseKey{ -1 }
{
}

void InputComponent::ProcessInput(const uint8_t* keyState)
{
	float forward = 0.f;
	if(keyState[mForwardKey])
	{
		forward += mMaxForwardSpeed;
	}
	if (keyState[mBackKey])
	{
		forward -= mMaxForwardSpeed;
	}
	AddForce(mOwner->GetForward() * forward);

	float angular = 0.f;
	if (keyState[mClockWiseKey])
	{
		angular -= mMaxAngularSpeed;
	}
	if (keyState[mCounterClockWiseKey])
	{
		angular += mMaxAngularSpeed;
	}
	SetAngularSpeed(angular);
}

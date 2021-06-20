#pragma once
#include "MoveComponent.h"
class InputComponent :
    public MoveComponent
{
    float mMaxForwardSpeed;
    float mMaxAngularSpeed;
    int mForwardKey;
    int mBackKey;
    int mClockWiseKey;
    int mCounterClockWiseKey;

public:
    InputComponent(class Actor* actor);

    void ProcessInput(const uint8_t* keyState) override;

    float GetMaxForwardSpeed() const { return mMaxForwardSpeed; }
    void SetMaxForwardSpeed(float speed) { mMaxForwardSpeed = speed; }
    float GetMaxAngularSpeed() const { return mMaxAngularSpeed; }
    void SetMaxAngularSpeed(float speed) { mMaxAngularSpeed = speed; }

    void SetForwardKey(int key) { mForwardKey = key; }
    void SetBackKey(int key) { mBackKey = key; }
    void SetClockWiseKey(int key) { mClockWiseKey = key; }
    void SetCounterClockWiseKey(int key) { mCounterClockWiseKey = key; }
    
};


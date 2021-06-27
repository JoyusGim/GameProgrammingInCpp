#include "InputSystem.h"
#include <SDL.h>
#include <string>

float InputSystem::Filter1D(int input)
{
    const int deadZone = 250;
    const int maxValue = 30000;

    float retVal = 0.f;

    int absValue = input > 0 ? input : -input;

    if (absValue > deadZone)
    {
        retVal = static_cast<float>(absValue - deadZone) / (maxValue - deadZone);

        retVal = input > 0 ? retVal : 1.f * retVal;

        retVal = Math::Clamp(retVal, -1.f, 1.f);
    }

    return retVal;
}

Vector2 InputSystem::Filter2D(int inputX, int inputY)
{
    const float deadZone = 8000.f;
    const float maxValue = 30000.f;

    Vector2 dir;
    dir.x = static_cast<float>(inputX);
    dir.y = static_cast<float>(inputY);

    float length = dir.Length();

    if (length < deadZone)
        dir = Vector2::Zero;
    else
    {
        float f = (length - deadZone) / (maxValue - deadZone);
        f = Math::Clamp(f, 0.f, 1.f);
        dir *= f / length;
    }

    return dir;
}

void InputSystem::LoadControllers()
{
    for (auto controller : mState.Controller.mControllers)
    {
        SDL_GameControllerClose(controller.second);
    }
    mState.Controller.mControllers.clear();

    for (int i = 0; i < SDL_NumJoysticks(); i++)
    {
        if (SDL_IsGameController(i))
        {
            SDL_GameController* controller = SDL_GameControllerOpen(i);
            mState.Controller.mControllers.emplace(i, controller);

            printf("%s connected\n", SDL_GameControllerName(controller));
        }
    }
    if(!mState.Controller.mControllers.empty())
        mState.Controller.mCurrController = mState.Controller.mControllers[0];

    mState.Controller.mIsConnected = (mState.Controller.mCurrController != nullptr);
    memset(mState.Controller.mCurrButtons, 0, SDL_CONTROLLER_BUTTON_MAX);
    memset(mState.Controller.mPrevButtons, 0, SDL_CONTROLLER_BUTTON_MAX);
}

void InputSystem::AddController(int id)
{
    ControllerState& cs = mState.Controller;

    if (cs.mControllers.find(id) != cs.mControllers.end())
    {
        printf("Already exist game controller: %d\n", id);
        return;
    }

    SDL_GameController* controller = SDL_GameControllerOpen(id);

    if (!controller)
    {
        SDL_Log("Failed to open game controller: %d", id);
        return;
    }

    cs.mCurrController = controller;
    cs.mControllers.emplace(id, controller);
    cs.mIsConnected = true;

    memset(cs.mCurrButtons, 0, SDL_CONTROLLER_BUTTON_MAX);
    memset(cs.mPrevButtons, 0, SDL_CONTROLLER_BUTTON_MAX);

    printf("Controller %s is connected.\n",
        SDL_GameControllerName(controller));
}

void InputSystem::RemoveController(int id)
{
    ControllerState& cs = mState.Controller;

    auto iter = cs.mControllers.find(id);
    if (iter != cs.mControllers.end())
    {
        printf("Controller %s is removed.\n",
            SDL_GameControllerName(iter->second));

        if (cs.mCurrController == iter->second)
            cs.mCurrController = nullptr;

        cs.mControllers.erase(iter);
    }
}

bool InputSystem::Initialize()
{
    mState.Keyboard.mCurrState = SDL_GetKeyboardState(NULL);
    memset(mState.Keyboard.mPrevState, 0, SDL_NUM_SCANCODES);

    mState.Mouse.mCurrButtons = 0;
    mState.Mouse.mPrevButtons = 0;

    LoadControllers();
    return true;
}

void InputSystem::Shutdown()
{
}

void InputSystem::PrepareForUpdate()
{
    memcpy(mState.Keyboard.mPrevState, mState.Keyboard.mCurrState, SDL_NUM_SCANCODES);
    mState.Mouse.mPrevButtons = mState.Mouse.mCurrButtons;
    mState.Mouse.mScrollWheel = Vector2::Zero;

    memcpy(mState.Controller.mPrevButtons,
        mState.Controller.mCurrButtons,
        SDL_CONTROLLER_BUTTON_MAX);
}

void InputSystem::Update()
{
    int x = 0; int y = 0;
    if (mState.Mouse.mIsRelative)
    {
        mState.Mouse.mCurrButtons = SDL_GetRelativeMouseState(&x, &y);
    }
    else
    {
        mState.Mouse.mCurrButtons = SDL_GetMouseState(&x, &y);
    }
    mState.Mouse.mMousePos.x = static_cast<float>(x);
    mState.Mouse.mMousePos.y = static_cast<float>(y);

    for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++)
    {
        mState.Controller.mCurrButtons[i] =
            SDL_GameControllerGetButton(mState.Controller.mCurrController,
                SDL_GameControllerButton(i));
    }

    mState.Controller.mLeftTrigger = 
        Filter1D(SDL_GameControllerGetAxis(mState.Controller.mCurrController,
            SDL_CONTROLLER_AXIS_TRIGGERLEFT));

    mState.Controller.mRightTrigger =
        Filter1D(SDL_GameControllerGetAxis(mState.Controller.mCurrController,
            SDL_CONTROLLER_AXIS_TRIGGERRIGHT));

    x = SDL_GameControllerGetAxis(mState.Controller.mCurrController,
        SDL_CONTROLLER_AXIS_LEFTX);
    y = -SDL_GameControllerGetAxis(mState.Controller.mCurrController,
        SDL_CONTROLLER_AXIS_LEFTY);
    mState.Controller.mLeftStick = Filter2D(x, y);

    x = SDL_GameControllerGetAxis(mState.Controller.mCurrController,
        SDL_CONTROLLER_AXIS_RIGHTX);
    y = -SDL_GameControllerGetAxis(mState.Controller.mCurrController,
        SDL_CONTROLLER_AXIS_RIGHTY);
    mState.Controller.mRightStick = Filter2D(x, y);
}

void InputSystem::ProcessEvent(SDL_Event& event)
{
    switch (event.type)
    {
    case SDL_MOUSEWHEEL:
        mState.Mouse.mScrollWheel = Vector2(
            static_cast<float>(event.wheel.x),
            static_cast<float>(event.wheel.y)
        );
        break;

    case SDL_CONTROLLERDEVICEADDED:
        AddController(event.jdevice.which);
        break;
    case SDL_CONTROLLERDEVICEREMOVED:
        RemoveController(event.jdevice.which);
        break;
    default:
        break;
    }
}

void InputSystem::SetRelativeMouseMode(bool value)
{
    SDL_bool set = value ? SDL_TRUE : SDL_FALSE;
    SDL_SetRelativeMouseMode(set);

    mState.Mouse.mIsRelative = value;
}

const InputState& InputSystem::GetState() const
{
    return mState;
}

bool KeyboardState::GetKeyValue(SDL_Scancode keyCode) const
{
    return mCurrState[keyCode] == 1;
}

ButtonState KeyboardState::GetKeyState(SDL_Scancode keyCode) const
{
    if (mPrevState[keyCode] == 0)
    {
        if (mCurrState[keyCode] == 0)
            return ButtonState::None;
        else
            return ButtonState::Pressed;
    }
    else
    {
        if (mCurrState[keyCode] == 0)
            return ButtonState::Released;
        else
            return ButtonState::Held;
    }
}

bool MouseState::GetButtonValue(int button) const
{
    return mCurrButtons & SDL_BUTTON(button);
}

ButtonState MouseState::GetButtonState(int button) const
{
    if ((mPrevButtons & SDL_BUTTON(button)) == 0)
    {
        if ((mCurrButtons & SDL_BUTTON(button)) == 0)
            return ButtonState::None;
        else
            return ButtonState::Pressed;
    }
    else
    {
        if ((mCurrButtons & SDL_BUTTON(button)) == 0)
            return ButtonState::Released;
        else
            return ButtonState::Held;
    }
}

const Vector2& MouseState::GetScrollWheel() const
{
    return mScrollWheel;
}

const Vector2& MouseState::GetMousePos() const
{
    return mMousePos;
}

bool ControllerState::IsConnected() const
{
    return mIsConnected;
}

bool ControllerState::GetButtonValue(SDL_GameControllerButton button) const
{
    return mCurrButtons[button] == 1;
}

ButtonState ControllerState::GetButtonState(SDL_GameControllerButton button) const
{
    if (mPrevButtons[button] == 0)
    {
        if (mCurrButtons[button] == 0)
            return ButtonState::None;
        else
            return ButtonState::Pressed;
    }
    else
    {
        if (mCurrButtons[button] == 0)
            return ButtonState::Released;
        else
            return ButtonState::Held;
    }
}

float ControllerState::GetLeftTrigger() const
{
    return mLeftTrigger;
}

float ControllerState::GetRightTrigger() const
{
    return mRightTrigger;
}

const Vector2& ControllerState::GetLeftStick() const
{
    return mLeftStick;
}

const Vector2& ControllerState::GetRightStick() const
{
    return mRightStick;
}

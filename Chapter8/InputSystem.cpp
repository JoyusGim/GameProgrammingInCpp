#include "InputSystem.h"
#include <SDL.h>
#include <string>

bool InputSystem::Initialize()
{
    mState.Keyboard.mCurrState = SDL_GetKeyboardState(NULL);
    memset(mState.Keyboard.mPrevState, 0, SDL_NUM_SCANCODES);

    mState.Mouse.mCurrButtons = 0;
    mState.Mouse.mPrevButtons = 0;

    mController = SDL_GameControllerOpen(0);
    
    mState.Controller.mIsConnected = (mController != nullptr);
    memset(mState.Controller.mCurrButtons, 0, SDL_CONTROLLER_BUTTON_MAX);
    memset(mState.Controller.mPrevButtons, 0, SDL_CONTROLLER_BUTTON_MAX);

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
            SDL_GameControllerGetButton(mController,
                SDL_GameControllerButton(i));
        //printf("%d ", mState.Controller.mCurrButtons[i]);
    }
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

#pragma once
#include <SDL_scancode.h>
#include <SDL_gamecontroller.h>
#include <SDL_mouse.h>
#include "Math.h"

enum ButtonState
{
	None,
	Pressed,
	Released,
	Held
};

class KeyboardState
{
	friend class InputSystem;
	const Uint8* mCurrState;
	Uint8 mPrevState[SDL_NUM_SCANCODES];

public:
	bool GetKeyValue(SDL_Scancode keyCode) const;
	ButtonState GetKeyState(SDL_Scancode keyCode) const;
};

class MouseState
{
	friend class InputSystem;
	Vector2 mMousePos;
	Vector2 mScrollWheel;
	Uint32 mCurrButtons;
	Uint32 mPrevButtons;
	bool mIsRelative;

public:
	bool GetButtonValue(int button) const;
	ButtonState GetButtonState(int button) const;
	const Vector2& GetScrollWheel() const;
};

class ControllerState
{
	friend class InputSystem;
	Uint8 mCurrButtons[SDL_CONTROLLER_BUTTON_MAX];
	Uint8 mPrevButtons[SDL_CONTROLLER_BUTTON_MAX];
	bool mIsConnected;

public:
	bool IsConnected() const;

	bool GetButtonValue(SDL_GameControllerButton button) const;
	ButtonState GetButtonState(SDL_GameControllerButton button) const;
};

struct InputState
{
	KeyboardState Keyboard;
	MouseState Mouse;
	ControllerState Controller;
};

class InputSystem
{
	SDL_GameController* mController;
	InputState mState;

public:
	bool Initialize();
	void Shutdown();

	void PrepareForUpdate();
	void Update();
	void ProcessEvent(union SDL_Event& event);

	void SetRelativeMouseMode(bool value);
	const InputState& GetState() const;
};

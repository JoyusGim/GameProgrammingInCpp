#include "PauseMenu.h"
#include "Game.h"
#include "InputSystem.h"
#include "DialogBox.h"

PauseMenu::PauseMenu(Game* game)	:
	UIScreen(game)
{
	mGame->SetGameState(Game::GamePause);
	SetTitle("PauseTitle");
	
	AddButton("ResumeButton", [this]() {
		Close();
	});
	AddButton("QuitButton", [this]() {
		new DialogBox(mGame, "QuitText",
			[this]() {
				mGame->SetGameState(Game::GameExit);
			});
	});
	mGame->GetInputSystem()->SetRelativeMouseMode(false);
}

PauseMenu::~PauseMenu()
{
	mGame->SetGameState(Game::GamePlay);
	mGame->GetInputSystem()->SetRelativeMouseMode(true);
}

void PauseMenu::ProcessInput(const InputState& state)
{
	UIScreen::ProcessInput(state);

	if (state.Keyboard.GetKeyState(SDL_SCANCODE_BACKSPACE) == Pressed)
	{
		Close();
	}
	else if (state.Keyboard.GetKeyState(SDL_SCANCODE_ESCAPE) == Pressed)
	{
		mGame->SetGameState(Game::GameExit);
	}
}

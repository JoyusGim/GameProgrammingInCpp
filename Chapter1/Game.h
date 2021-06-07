#pragma once
#include <SDL.h>

struct Vector2
{
	float x;
	float y;
};

class Game
{
	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;
	bool mIsRunning;

	Vector2 mBallPos;
	Vector2 mBallVelocity;

	Vector2 mPaddlePos;
	int mPaddleDir;

	Uint32 mTickCount;

	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();

public:
	Game();

	bool Initialize();
	void RunLoop();
	void Shutdown();
};


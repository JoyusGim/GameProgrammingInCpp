#include "Game.h"

static const int thickness = 15;
static const float paddleH = 100.f;

void Game::ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            mIsRunning = false;
            break;
        }
    }

    const Uint8* state = SDL_GetKeyboardState(nullptr);
    if (state[SDL_SCANCODE_ESCAPE])
    {
        mIsRunning = false;
    }
    mPaddleDir = 0;
    if (state[SDL_SCANCODE_W])
    {
        mPaddleDir = -1;
    }
    if (state[SDL_SCANCODE_S])
    {
        mPaddleDir = 1;
    }
}

void Game::UpdateGame()
{
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTickCount + 16));

    float deltaTime = (SDL_GetTicks() - mTickCount) / 1000.f;
    mTickCount = SDL_GetTicks();

    if (deltaTime > 0.05f)
        deltaTime = 0.05f;

    // update ball
    mBallPos.x += mBallVelocity.x * deltaTime;
    mBallPos.y += mBallVelocity.y * deltaTime;

    if (mBallPos.y - (thickness / 2.f) <= thickness && mBallVelocity.y < 0.f)
        mBallVelocity.y *= -1;
    if (mBallPos.y + (thickness / 2.f) >= 768 - thickness && mBallVelocity.y > 0.f)
        mBallVelocity.y *= -1;
    if (mBallPos.x + (thickness / 2.f) >= 1024 - thickness && mBallVelocity.x > 0.f)
        mBallVelocity.x *= -1;

    if (mBallPos.x - mPaddlePos.x <= thickness / 2.f &&
        mBallPos.y - (thickness / 2.f) >= mPaddlePos.y - (paddleH / 2.f) &&
        mBallPos.y + (thickness / 2.f) <= mPaddlePos.y + (paddleH / 2.f) &&
        mBallVelocity.x < 0.f)
    {
        mBallVelocity.x *= -1;
    }


    // update paddle
    mPaddlePos.y += mPaddleDir * 300.f * deltaTime;

    if (mPaddlePos.y < (paddleH / 2.f) + thickness)
    {
        mPaddlePos.y = paddleH / 2.f + thickness;
    }
    else if (mPaddlePos.y > 768.f - (paddleH / 2.f + thickness))
    {
        mPaddlePos.y = 768.f - (paddleH / 2.f + thickness);
    }
}

void Game::GenerateOutput()
{
    SDL_SetRenderDrawColor(mRenderer, 0, 0, 255, 255);
    SDL_RenderClear(mRenderer);

    SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);

    // draw walls
    SDL_Rect wall_top{ 0, 0, 1024, thickness };
    SDL_Rect wall_bottom{ 0, 768 - thickness, 1024, thickness };
    SDL_Rect wall_right{ 1024 - thickness, 0, 100, 768 };

    SDL_RenderFillRect(mRenderer, &wall_top);
    SDL_RenderFillRect(mRenderer, &wall_bottom);
    SDL_RenderFillRect(mRenderer, &wall_right);

    // draw ball
    SDL_Rect ball{
        static_cast<int>(mBallPos.x - thickness / 2),
        static_cast<int>(mBallPos.y - thickness / 2),
        thickness,
        thickness
    };
    SDL_RenderFillRect(mRenderer, &ball);

    // draw paddle
    SDL_Rect paddle{
        static_cast<int>(mPaddlePos.x - thickness),
        static_cast<int>(mPaddlePos.y - paddleH / 2),
        thickness,
        static_cast<int>(paddleH)
    };
    SDL_RenderFillRect(mRenderer, &paddle);

    SDL_RenderPresent(mRenderer);    
}

Game::Game()    :
    mWindow{ nullptr },
    mRenderer{ nullptr },
    mIsRunning{ true },
    mBallPos{ 0 },
    mBallVelocity{ 0 },
    mPaddleDir{ 0 },
    mPaddlePos{ 0 },
    mTickCount{ 0 }
{
}

bool Game::Initialize()
{
    int sdlResult = SDL_Init(SDL_INIT_VIDEO);
    if (sdlResult != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    mWindow = SDL_CreateWindow(
        "Game Programming in C++ (Chapter 1)",
        100, 100,
        1024, 768,
        0
    );

    if (!mWindow)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mRenderer = SDL_CreateRenderer(
        mWindow,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!mRenderer)
    {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        return false;
    }

    // initialize ball
    mBallPos.x = 1024.f / 2.f;
    mBallPos.y = 768.f / 2.f;
    mBallVelocity.x = -200.f;
    mBallVelocity.y = 235.f;

    // initialize paddle
    mPaddlePos.x = 20.f;
    mPaddlePos.y = 768.f / 2.f;

    return true;
}

void Game::RunLoop()
{
    while (mIsRunning)
    {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}

void Game::Shutdown()
{
    SDL_DestroyWindow(mWindow);
    SDL_DestroyRenderer(mRenderer);
    SDL_Quit();
}

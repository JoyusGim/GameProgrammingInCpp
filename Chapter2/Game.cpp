#include "Game.h"
#include "Ship.h"
#include "TileMapComponent.h"
#include <SDL_image.h>

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

}

void Game::UpdateGame()
{
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTickCount + 16));

    float deltaTime = (SDL_GetTicks() - mTickCount) / 1000.f;
    mTickCount = SDL_GetTicks();

    if (deltaTime > 0.05f)
        deltaTime = 0.05f;

    mUpdatingActor = true;
    for (auto actor : mActors)
    {
        actor->Update(deltaTime);
    }

    mUpdatingActor = false;
    for (auto pending : mPendingActors)
    {
        mActors.emplace_back(pending);
    }
    mPendingActors.clear();

    std::vector<Actor*> deadActors;
    for (auto actor : mActors)
    {
        if (actor->GetState() == Actor::State::DEAD)
        {
            deadActors.emplace_back(actor);
        }
    }

    for (auto actor : deadActors)
    {
        delete actor;
    }
}

void Game::GenerateOutput()
{
    SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);
    SDL_RenderClear(mRenderer);

    for (auto sprite : mSprites)
    {
        sprite->Draw(mRenderer);
    }

    SDL_RenderPresent(mRenderer);    
}

void Game::LoadData()
{
    Actor* tmp = new Actor(this);
    tmp->SetPosition(Math::Vector2( 1024 / 2, 768 / 2 ));

    SDL_Texture* atlasTexture = GetTexture("Assets/Tiles.png");

    TileMapComponent* tmc = new TileMapComponent(tmp);
    tmc->SetAtlasTexture(atlasTexture, 8, 24);
    tmc->LoadCSV("Assets/MapLayer1.csv");
    
    tmc = new TileMapComponent(tmp, 90);
    tmc->SetAtlasTexture(atlasTexture, 8, 24);
    tmc->LoadCSV("Assets/MapLayer2.csv");

    tmc = new TileMapComponent(tmp, 80);
    tmc->SetAtlasTexture(atlasTexture, 8, 24);
    tmc->LoadCSV("Assets/MapLayer3.csv");
    
}

void Game::UnloadData()
{
    while (!mActors.empty())
    {
        delete mActors.back();
    }

    for (auto tex : mTextures)
    {
        SDL_DestroyTexture(tex.second);
    }
    mTextures.clear();
}

Game::Game()    :
    mWindow{ nullptr },
    mRenderer{ nullptr },
    mIsRunning{ true },
    mTickCount{ 0 },
    mUpdatingActor{ false }
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
        "Game Programming in C++ (Chapter 2 Exercise 3)",
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

    if (IMG_Init(IMG_INIT_PNG) == 0)
    {
        SDL_Log("Unable to initialize SDL_image: %s", SDL_GetError());
        return false;
    }

    LoadData();

    mTickCount = SDL_GetTicks();

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
    UnloadData();
    IMG_Quit();
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

void Game::AddActor(Actor* actor)
{
    if (mUpdatingActor)
    {
        mPendingActors.emplace_back(actor);
    }
    else
    {
        mActors.emplace_back(actor);
    }
}

void Game::RemoveActor(Actor* actor)
{
    auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
    if (iter != mPendingActors.end())
    {
        // 삭제할 액터와 대기목록의 끝에있는 액터를 서로 스왑후 pop_back한다.
        std::iter_swap(iter, mPendingActors.end() - 1);
        mPendingActors.pop_back();
    }

    iter = std::find(mActors.begin(), mActors.end(), actor);
    if (iter != mActors.end())
    {
        std::iter_swap(iter, mActors.end() - 1);
        mActors.pop_back();
    }
}

void Game::AddSpriteComponent(SpriteComponent* sprite)
{
    int drawOrder = sprite->GetDrawOrder();
    auto iter = mSprites.begin();
    for (; iter != mSprites.end(); ++iter)
    {
        if (drawOrder < (*iter)->GetDrawOrder())
            break;
    }

    mSprites.insert(iter, sprite);
}

void Game::RemoveSpriteComponent(class SpriteComponent* sprite)
{
    // drawOrder를 망치지 않기위해 swap을 하지 않는다.
    auto iter = std::find(mSprites.begin(), mSprites.end(), sprite);
    mSprites.erase(iter);
}

SDL_Texture* Game::GetTexture(const std::string& fileName)
{
    SDL_Texture* texture = nullptr;
    
    auto iter = mTextures.find(fileName);
    if (iter != mTextures.end())
    {
        texture = iter->second;
    }
    else
    {
        SDL_Surface* surf = IMG_Load(fileName.c_str());
        if (!surf)
        {
            SDL_Log("Failed to load texture file %s", fileName.c_str());
            return nullptr;
        }

        texture = SDL_CreateTextureFromSurface(mRenderer, surf);
        SDL_FreeSurface(surf);
        if (!texture)
        {
            SDL_Log("Failed to convert surface to texture for %s", fileName.c_str());
            return nullptr;
        }

        mTextures.emplace(fileName.c_str(), texture);
    }
        
    return texture;
}
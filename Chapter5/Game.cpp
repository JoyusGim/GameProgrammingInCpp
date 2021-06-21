#include "Game.h"
#include "Actor.h"
#include "Asteroid.h"
#include "Ship.h"
#include "SpriteComponent.h"
#include "Shader.h"
#include "VertexArray.h"
#include "Texture.h"
#include <SDL_image.h>
#include <glew.h>

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

    mUpdatingActor = true;
    for (auto actor : mActors)
    {
        actor->ProcessInput(state);
    }
    mUpdatingActor = false;
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
        pending->ComputeWorldTransform();
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
    glClearColor(0.86f, 0.86f, 0.86f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    mSpriteShader->SetActive();
    mSpriteVerts->SetActive();

    for (auto sprite : mSprites)
    {
        sprite->Draw(mSpriteShader);
    }

    SDL_GL_SwapWindow(mWindow);
}

void Game::LoadData()
{
    new Ship(this);

    for (int i = 0; i < 20; i++)
    {
        new Asteroid(this);
    }
}

void Game::UnloadData()
{
    while (!mActors.empty())
    {
        delete mActors.back();
    }

    for (auto tex : mTextures)
    {
        tex.second->Unload();
    }
    mTextures.clear();
}

bool Game::LoadShaders()
{
    mSpriteShader = new Shader();
    if(!mSpriteShader->Load("Shaders/Sprite.vert", "Shaders/Sprite.frag"))
    {
        return false;
    }

    mSpriteShader->SetActive();

    Matrix4 viewProj = Matrix4::CreateSimpleViewProj(1024.f, 768.f);
    mSpriteShader->SetMatrixUniform("uViewProj", viewProj);

    return true;
}

bool Game::InitSpriteVerts()
{
    float vertexBuffer[] = {
        -0.5f, 0.5f, 0.f, 0.f, 0.f,
        0.5f, 0.5f, 0.f, 1.f, 0.f,
        0.5f, -0.5f, 0.f, 1.f, 1.f,
        -0.5f, -0.5f, 0.f, 0.f, 1.f
    };

    unsigned int indexBuffer[] = {
        0, 1, 2,
        2, 3, 0
    };

    mSpriteVerts = new VertexArray(vertexBuffer, 4, indexBuffer, 6);

    return true;
}

Game::Game()    :
    mWindow{ nullptr },
    mContext{ },
    mIsRunning{ true },
    mTickCount{ 0 },
    mUpdatingActor{ false },
    mSpriteShader{ nullptr },
    mSpriteVerts{ nullptr }
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
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    mWindow = SDL_CreateWindow(
        "Game Programming in C++ Chapter 6",
        100, 100,
        1024, 768,
        SDL_WINDOW_OPENGL
    );

    if (!mWindow)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mContext = SDL_GL_CreateContext(mWindow);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        SDL_Log("Failed to initialize GLEW.");
        return false;
    }
    glGetError();

    if (IMG_Init(IMG_INIT_PNG) == 0)
    {
        SDL_Log("Unable to initialize SDL_image: %s", SDL_GetError());
        return false;
    }

    if (!LoadShaders())
        return false;

    if (!InitSpriteVerts())
        return false;
        
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
    SDL_GL_DeleteContext(mContext);
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

Texture* Game::GetTexture(const std::string& fileName)
{
    Texture* texture = nullptr;
    
    auto iter = mTextures.find(fileName);
    if (iter != mTextures.end())
    {
        texture = iter->second;
    }
    else
    {
        texture = new Texture();
        if (!texture->Load(fileName))
            return nullptr;

        mTextures.emplace(fileName.c_str(), texture);
    }
        
    return texture;
}

void Game::AddAsteroid(Asteroid* asteroid)
{
    mAsteroids.push_back(asteroid);
}

void Game::RemoveAsteroid(Asteroid* asteroid)
{
    auto iter = std::find(mAsteroids.begin(), mAsteroids.end(), asteroid);
    if (iter != mAsteroids.end())
    {
        mAsteroids.erase(iter);
    }
}

std::vector<Asteroid*>& Game::GetAsteroids()
{
    return mAsteroids;
}

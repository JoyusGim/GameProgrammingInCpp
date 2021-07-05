#include "Game.h"
#include <algorithm>
#include "Actor.h"
#include "Renderer.h"
#include "AudioSystem.h"
#include "InputSystem.h"
#include "SpriteComponent.h"
#include "MeshComponent.h"
#include "FPSActor.h"
#include "PlaneActor.h"
#include "PhysWorld.h"
#include "BallActor.h"
#include "TargetActor.h"
#include "Texture.h"

void Game::ProcessInput()
{
    mInputSystem->PrepareForUpdate();

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            mIsRunning = false;
            break;
        case SDL_MOUSEWHEEL:
            mInputSystem->ProcessEvent(event);
            break;
        case SDL_CONTROLLERDEVICEADDED:
        case SDL_CONTROLLERDEVICEREMOVED:
            mInputSystem->ProcessEvent(event);
            break;
        /*
        case SDL_MOUSEBUTTONDOWN:
            mInputSystem->SetRelativeMouseMode(true);
            break;
        case SDL_MOUSEBUTTONUP:
            mInputSystem->SetRelativeMouseMode(false);
            break;
        */   
        default:
            break;
        }
    }
    
    mInputSystem->Update();
    const InputState& state = mInputSystem->GetState();
    if (state.Keyboard.GetKeyState(SDL_SCANCODE_ESCAPE) == Released)
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

    mAudioSystem->Update(deltaTime);

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
    mRenderer->Draw();
}

void Game::LoadData()
{
    // Create actors
    Actor* a = nullptr;
    Quaternion q;
    //MeshComponent* mc = nullptr;

    // Setup floor
    const float start = -1250.0f;
    const float size = 250.0f;
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            a = new PlaneActor(this);
            a->SetPosition(Vector3(start + i * size, start + j * size, -100.0f));
        }
    }
    
    // Left/right walls
    q = Quaternion(Vector3::UnitX, Math::PiOver2);
    for (int i = 0; i < 10; i++)
    {
        a = new PlaneActor(this);
        a->SetPosition(Vector3(start + i * size, start - size, 0.0f));
        a->SetRotate(q);

        a = new PlaneActor(this);
        a->SetPosition(Vector3(start + i * size, -start + size, 0.0f));
        a->SetRotate(q);
    }

    q = Quaternion::Concatenate(q, Quaternion(Vector3::UnitZ, Math::PiOver2));
    // Forward/back walls
    for (int i = 0; i < 10; i++)
    {
        a = new PlaneActor(this);
        a->SetPosition(Vector3(start - size, start + i * size, 0.0f));
        a->SetRotate(q);

        a = new PlaneActor(this);
        a->SetPosition(Vector3(-start + size, start + i * size, 0.0f));
        a->SetRotate(q);
    }
    
    // Setup lights
    mRenderer->SetAmbientLight(Vector3(0.2f, 0.2f, 0.2f));
    DirectionalLight& dir = mRenderer->GetDirectionalLight();
    dir.mDirection = Vector3(0.0f, -0.707f, -0.707f);
    dir.mDiffuseColor = Vector3(0.78f, 0.88f, 1.0f);
    dir.mSpecColor = Vector3(0.8f, 0.8f, 0.8f);

    // UI elements
    a = new Actor(this);
    a->SetPosition(Vector3(-350.0f, -350.0f, 0.0f));
    SpriteComponent* sc = new SpriteComponent(a);
    sc->SetTexture(mRenderer->GetTexture("Assets/HealthBar.png"));

    a = new Actor(this);
    a->SetPosition(Vector3(-390.0f, 275.0f, 0.0f));
    a->SetScale(0.75f);
    sc = new SpriteComponent(a);
    sc->SetTexture(mRenderer->GetTexture("Assets/Radar.png"));


    // Enable relative mouse mode for camera look
    SDL_SetRelativeMouseMode(SDL_TRUE);
    // Make an initial call to get relative to clear out
    SDL_GetRelativeMouseState(nullptr, nullptr);

    // Different camera actors
    mFPSActor = new FPSActor(this);

    // Create target actors
    a = new TargetActor(this);
    a->SetPosition(Vector3(1450.0f, 0.0f, 100.0f));
    a = new TargetActor(this);
    a->SetPosition(Vector3(1450.0f, 0.0f, 400.0f));
    a = new TargetActor(this);
    a->SetPosition(Vector3(1450.0f, -500.0f, 200.0f));
    a = new TargetActor(this);
    a->SetPosition(Vector3(1450.0f, 500.0f, 200.0f));
}

void Game::UnloadData()
{
    while (!mActors.empty())
    {
        delete mActors.back();
    }

    mRenderer->UnloadData();
}

Game::Game()    :
    mRenderer{ nullptr },
    mAudioSystem{ nullptr },
    mIsRunning{ true },
    mTickCount{ 0 },
    mUpdatingActor{ false },
    mFPSActor{ nullptr },
    mInputSystem{ nullptr },
    mPhysWorld{ nullptr }
{
}

bool Game::Initialize()
{
    int sdlResult = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER);

    if (sdlResult != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    mRenderer = new Renderer(this);
    if (!mRenderer->Initialize(1024.f, 768.f))
        return false;

    mAudioSystem = new AudioSystem(this);
    if (!mAudioSystem->Initialize())
        return false;

    mInputSystem = new InputSystem();
    if (!mInputSystem->Initialize())
        return false;
    mInputSystem->SetRelativeMouseMode(true);

    mPhysWorld = new PhysWorld(this);
    
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
    if (mPhysWorld)
    {
        delete mPhysWorld;
        mRenderer->Shutdown();
    }
    if (mRenderer)
    {
        delete mRenderer;
        mAudioSystem->Shutdown();
    }
    if (mAudioSystem)
    {
        mAudioSystem->Shutdown();
        delete mAudioSystem;
    }
    if (mInputSystem)
    {
        mInputSystem->Shutdown();
        delete mInputSystem;
    }


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

void Game::AddPlane(PlaneActor* plane)
{
    mPlanes.emplace_back(plane);
}

Renderer* Game::GetRenderer() const
{
    return mRenderer;
}

AudioSystem* Game::GetAudioSystem() const
{
    return mAudioSystem;
}

PhysWorld* Game::GetPhysWorld() const
{
    return mPhysWorld;
}

std::vector<class PlaneActor*>& Game::GetPlanes()
{
    return mPlanes;
}

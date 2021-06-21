#include "Game.h"
#include "Actor.h"
#include "Renderer.h"
#include "SpriteComponent.h"
#include "MeshComponent.h"

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
    mRenderer->Draw();
}

void Game::LoadData()
{
    Actor* cube = new Actor(this);
    cube->SetPosition(Vector3(200.f, 75.f, 0.f));
    cube->SetScale(100.f);

    Quaternion q(Vector3::UnitY, -Math::PiOver2);
    q = Quaternion::Concatenate(q, Quaternion(Vector3::UnitZ, Math::Pi + Math::Pi / 4.f));
    cube->SetRotate(q);

    MeshComponent* mc = new MeshComponent(cube);
    mc->SetMesh(mRenderer->GetMesh("Assets/Cube.gpmesh"));

    Actor* sphere = new Actor(this);
    sphere->SetPosition(Vector3(200.f, -75.f, 0.f));
    sphere->SetScale(3.f);
    mc = new MeshComponent(sphere);
    mc->SetMesh(mRenderer->GetMesh("Assets/Sphere.gpmesh"));

    mRenderer->SetAmbientLight(Vector3(0.2f, 0.2f, 0.2f));
    DirectionalLight& dir = mRenderer->GetDirectionalLight();
    dir.mDirection = Vector3(0.f, -0.707f, -0.707f);
    dir.mDiffuseColor = Vector3(0.0f, 1.0f, 0.f);
    dir.mSpecColor = Vector3(0.5f, 1.f, 0.5f);

    Actor* UI = new Actor(this);
    UI->SetPosition(Vector3(-350.f, -350.f, 0.f));
    SpriteComponent* sc = new SpriteComponent(UI);
    sc->SetTexture(mRenderer->GetTexture("Assets/HealthBar.png"));
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
    mIsRunning{ true },
    mTickCount{ 0 },
    mUpdatingActor{ false }
{
}

bool Game::Initialize()
{
    mRenderer = new Renderer(this);
    if (!mRenderer->Initialize(1024.f, 768.f))
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
    mRenderer->Shutdown();
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

Renderer* Game::GetRenderer() const
{
    return mRenderer;
}

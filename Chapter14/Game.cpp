#include "Game.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <rapidjson\document.h>
#include <rapidjson\stream.h>
#include "Actor.h"
#include "Renderer.h"
#include "AudioSystem.h"
#include "InputSystem.h"
#include "SpriteComponent.h"
#include "MeshComponent.h"
#include "FollowActor.h"
#include "PlaneActor.h"
#include "PhysWorld.h"
#include "BallActor.h"
#include "TargetActor.h"
#include "Texture.h"
#include "Font.h"
#include "UIScreen.h"
#include "HUD.h"
#include "PauseMenu.h"
#include "Skeleton.h"
#include "Animation.h"
#include "PointLightComponent.h"
#include "LevelLoader.h"


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

    if (mGameState == GamePlay)
    {
        mUpdatingActor = true;
        for (auto actor : mActors)
        {
            actor->ProcessInput(state);
        }
        mUpdatingActor = false;

        if (state.Keyboard.GetKeyState(SDL_SCANCODE_ESCAPE) == Pressed)
        {
            new PauseMenu(this);
        }
    }
    else if (mGameState == GamePause)
    {
        mUIStack.back()->ProcessInput(state);
    }

    if (state.Keyboard.GetKeyState(SDL_SCANCODE_R) == Pressed)
    {
        LevelLoader::SaveLevel(this, "Assets/saved.gplevel");
    }
}

void Game::UpdateGame()
{
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTickCount + 16));

    float deltaTime = (SDL_GetTicks() - mTickCount) / 1000.f;
    mTickCount = SDL_GetTicks();

    if (deltaTime > 0.05f)
        deltaTime = 0.05f;

    if (mGameState == GamePlay)
    {
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

    mAudioSystem->Update(deltaTime);

    for(auto ui : mUIStack)
    {
        if (ui->GetState() == UIScreen::Active)
        {
            ui->Update(deltaTime);
        }
    }

    auto iter = mUIStack.begin();
    while (iter != mUIStack.end())
    {
        if ((*iter)->GetState() == UIScreen::Closing)
        {
            delete *iter;
            iter = mUIStack.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

void Game::GenerateOutput()
{
    mRenderer->Draw();
}

void Game::LoadData()
{    
    LoadText("Assets/English.gptext");

    mHUD = new HUD(this);

    LevelLoader::LoadLevel(this, "Assets/saved.gplevel");
}

void Game::UnloadData()
{
    while (!mActors.empty())
    {
        delete mActors.back();
    }

    while (!mUIStack.empty())
    {
        delete mUIStack.back();
        mUIStack.pop_back();
    }

    for (auto f : mFonts)
    {
        f.second->Unload();
        delete f.second;
    }
    
    for (auto s : mSkeletons)
    {
        delete s.second;
    }

    for (auto a : mAnimations)
    {
        delete a.second;
    }

    if (mRenderer)
    {
        mRenderer->UnloadData();
    }
}

Game::Game()    :
    mRenderer{ nullptr },
    mAudioSystem{ nullptr },
    mIsRunning{ true },
    mTickCount{ 0 },
    mUpdatingActor{ false },
    mFollowActor{ nullptr },
    mInputSystem{ nullptr },
    mPhysWorld{ nullptr },
    mGameState{ GamePause }
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

    int ttfResult = TTF_Init();
    if (ttfResult != 0)
    {
        SDL_Log("Unable to initialize SDL_TTF: %s", TTF_GetError());
        return false;
    }

    mRenderer = new Renderer(this);
    if (!mRenderer->Initialize(1024.f, 768.f))
        return false;
    if (!mRenderer->CreateMirrorTarget())
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

    mGameState = GameState::GamePlay;
    return true;
}

void Game::RunLoop()
{
    while (mGameState != GameExit)
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
    }
    if (mRenderer)
    {
        mRenderer->Shutdown();
        delete mRenderer;
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

    TTF_Quit();
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

const std::vector<class Actor*>& Game::GetActors() const
{
    return mActors;
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

void Game::LoadText(const std::string& fileName)
{
    mTextMap.clear();
    std::ifstream ifs(fileName);
    if (!ifs.is_open())
    {
        SDL_Log("Text file %s not found", fileName.c_str());
        return;
    }

    std::stringstream fileStream;
    fileStream << ifs.rdbuf();
    std::string contents = fileStream.str();

    rapidjson::StringStream jsonStr(contents.c_str());
    rapidjson::Document doc;
    doc.ParseStream(jsonStr);
    if (!doc.IsObject())
    {
        SDL_Log("Text file %s is not valid JSON", fileName.c_str());
        return;
    }

    const rapidjson::Value& actions = doc["TextMap"];
    for (rapidjson::Value::ConstMemberIterator iter = actions.MemberBegin();
        iter != actions.MemberEnd(); ++iter)
    {
        if (iter->name.IsString() && iter->value.IsString())
        {
            mTextMap.emplace(iter->name.GetString(), iter->value.GetString());
        }
    }
}

FollowActor* Game::GetPlayer() const
{
    return mFollowActor;
}

void Game::SetPlayer(FollowActor* actor)
{
    mFollowActor = actor;
}

Renderer* Game::GetRenderer() const
{
    return mRenderer;
}

InputSystem* Game::GetInputSystem() const
{
    return mInputSystem;
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

void Game::PushUI(UIScreen* screen)
{
    mUIStack.push_back(screen);
}

std::vector<class UIScreen*>& Game::GetUIStack()
{
    return mUIStack;
}

Font* Game::GetFont(const std::string& fileName)
{
    Font* font = nullptr;
    auto iter = mFonts.find(fileName);
    if (iter != mFonts.end())
    {
        font = iter->second;
    }
    else
    {
        font = new Font(this);
        if (!font->Load(fileName))
        {
            delete font;
            return nullptr;
        }
        mFonts.emplace(fileName, font);
    }

    return font;
}

Skeleton* Game::GetSkeleton(const std::string& fileName)
{
    auto iter = mSkeletons.find(fileName);
    if (iter != mSkeletons.end())
    {
        return iter->second;
    }
    else
    {
        Skeleton* sk = new Skeleton();
        if (sk->Load(fileName))
        {
            mSkeletons.emplace(fileName, sk);
        }
        else
        {
            delete sk;
            sk = nullptr;
        }
        return sk;
    }
}

Animation* Game::GetAnimation(const std::string& fileName)
{
    auto iter = mAnimations.find(fileName);
    if (iter != mAnimations.end())
    {
        return iter->second;
    }
    else
    {
        Animation* anim = new Animation();
        if (anim->Load(fileName))
        {
            mAnimations.emplace(fileName, anim);
        }
        else
        {
            delete anim;
            anim = nullptr;
        }
        return anim;
    }
    
}

void Game::SetGameState(const GameState& state)
{
    mGameState = state;
}

Game::GameState Game::GetGameState() const
{
    return mGameState;
}

HUD* Game::GetHUD() const
{
    return mHUD;
}

const std::string& Game::GetText(const std::string& key)
{
    static std::string errorMsg("**KEY NOT FOUND**");

    auto iter = mTextMap.find(key);
    if (iter != mTextMap.end())
    {
        return iter->second;
    }
    else
    {
        return errorMsg;
    }
}

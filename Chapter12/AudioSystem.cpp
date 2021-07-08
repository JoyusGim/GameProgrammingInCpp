#include "AudioSystem.h"
#include <vector>
#include <fmod.hpp>
#include <fmod_studio.hpp>
#include <fmod_errors.h>
#include <SDL.h>

unsigned int AudioSystem::sNextID = 0;

FMOD::Studio::EventInstance* AudioSystem::GetEventInstance(unsigned int id) const
{
    auto iter = mEventInstances.find(id);
    if (iter == mEventInstances.end())
        return nullptr;

    return iter->second;
}

AudioSystem::AudioSystem(Game* game) :
    mGame{ game },
    mSystem{ nullptr },
    mLowLevelSystem{ nullptr }
{
}

AudioSystem::~AudioSystem()
{
}

bool AudioSystem::Initialize()
{
    FMOD::Debug_Initialize(
        FMOD_DEBUG_LEVEL_ERROR,
        FMOD_DEBUG_MODE_TTY
    );

    FMOD_RESULT result;
    result = FMOD::Studio::System::create(&mSystem);
    if (result != FMOD_OK)
    {
        SDL_Log("Failed to create FMOD system: %s", FMOD_ErrorString(result));
        return false;
    }

    result = mSystem->initialize(
        512,                        // ä�� ��(���ÿ� ����� �� �ִ� ���� ��)
        FMOD_STUDIO_INIT_NORMAL,    // �⺻ ���� ���
        FMOD_INIT_NORMAL,           // �⺻ ���� ���
        nullptr                     // ��κ� nullptr
    );

    if (result != FMOD_OK)
    {
        SDL_Log("Failed to initialize FMOD system: %s", FMOD_ErrorString(result));
        return false;
    }
    
    mSystem->getLowLevelSystem(&mLowLevelSystem);

    // ������ ��ũ �ε�
    LoadBank("Assets/Master Bank.strings.bank"); // ���ڿ��� ��ũ�� �̺�Ʈ�� �����ϱ� ���� �ε�
    LoadBank("Assets/Master Bank.bank");

    return true;
}

void AudioSystem::Shutdown()
{
    UnloadAllBanks();
    if(mSystem)
        mSystem->release();
}

void AudioSystem::Update(float deltaTime)
{
    // ������ �̺�Ʈ�� ã�´�.
    std::vector<unsigned int> done;
    for (auto& instance : mEventInstances)
    {
        FMOD::Studio::EventInstance* e = instance.second;
        // ���� �̺�Ʈ�� ���°��� ��´�.
        FMOD_STUDIO_PLAYBACK_STATE state;
        e->getPlaybackState(&state);
        if (state == FMOD_STUDIO_PLAYBACK_STOPPED)
        {
            // �̺�Ʈ�� �����ϰ� id�� ������Ͽ� �߰�
            e->release();
            done.push_back(instance.first);
        }
    }

    // ������ �̺�Ʈ �ν��Ͻ� ����
    for (auto id : done)
    {
        mEventInstances.erase(id);
    }

    // FMOD ������Ʈ
    mSystem->update();
}

namespace
{
FMOD_VECTOR VecToFMOD(const Vector3& in)
{
    FMOD_VECTOR v;
    v.x = in.y;
    v.y = in.z;
    v.z = in.x;
    return v;
}
}

SoundEvent AudioSystem::PlayEvent(const std::string& name)
{
    unsigned int retID = 0;
    auto iter = mEvents.find(name);
    if (iter != mEvents.end())
    {
        FMOD::Studio::EventInstance* event = nullptr;
        iter->second->createInstance(&event);
        if (event)
        {
            event->start();
            sNextID++;
            retID = sNextID;
            mEventInstances.emplace(retID, event);
        }
    }
    return SoundEvent(this, retID);
}

void AudioSystem::SetListener(const Matrix4& viewMatrix)
{
    Matrix4 invView = viewMatrix;
    invView.Invert();
    FMOD_3D_ATTRIBUTES listener;

    listener.position = VecToFMOD(invView.GetTranslation());
    listener.forward = VecToFMOD(invView.GetZAxis());
    listener.up = VecToFMOD(invView.GetYAxis());
    listener.velocity = { 0.f, 0.f, 0.f };
    mSystem->setListenerAttributes(0, &listener);
}

void AudioSystem::LoadBank(const std::string& name)
{
    if (mBanks.find(name) != mBanks.end())
        return;

    FMOD::Studio::Bank* bank = nullptr;
    FMOD_RESULT result = mSystem->loadBankFile(
        name.c_str(),
        FMOD_STUDIO_LOAD_BANK_NORMAL,
        &bank
    );

    const int maxPathLength = 512;
    if (result == FMOD_OK)
    {
        // �ʿ� ��ũ �߰�
        mBanks.emplace(name, bank);
        // ��Ʈ���� ������ �ƴ� ��� ���� ������ �ε�
        bank->loadSampleData();
        // ��ũ�� �̺�Ʈ ��
        int numEvents = 0;
        bank->getEventCount(&numEvents);
        if (numEvents > 0)
        {
            // ��ũ���� �̺�Ʈ ��ũ���� ����Ʈ�� ��´�.
            std::vector<FMOD::Studio::EventDescription*> events(numEvents);
            bank->getEventList(events.data(), numEvents, &numEvents);
            char eventName[maxPathLength];

            for (int i = 0; i < numEvents; i++)
            {
                FMOD::Studio::EventDescription* e = events[i];
                // �̺�Ʈ ��θ� ��´�.
                e->getPath(eventName, maxPathLength, nullptr);
                // �̺�Ʈ�� �ʿ� �߰��Ѵ�.
                mEvents.emplace(eventName, e);
            }
        }

        int numBuses = 0;
        bank->getBusCount(&numBuses);
        if (numBuses > 0)
        {
            std::vector<FMOD::Studio::Bus*> buses(numBuses);
            bank->getBusList(buses.data(), numBuses, &numBuses);
            char busName[maxPathLength];

            for (int i = 0; i < numBuses; i++)
            {
                FMOD::Studio::Bus* bus = buses[i];
                bus->getPath(busName, maxPathLength, nullptr);
                mBuses.emplace(busName, bus);
            }
        }
    }
}

void AudioSystem::UnloadBank(const std::string& name)
{
    auto iter = mBanks.find(name);
    if (iter == mBanks.end())
        return;
    
    FMOD::Studio::Bank* bank = iter->second;

    int numEvents = 0;
    bank->getEventCount(&numEvents);
    if (numEvents > 0)
    {
        std::vector<FMOD::Studio::EventDescription*> events(numEvents);
        mBanks[name]->getEventList(events.data(), numEvents, &numEvents);

        char eventName[512];
        for (int i = 0; i < numEvents; i++)
        {
            FMOD::Studio::EventDescription* e = events[i];
            e->getPath(eventName, 512, nullptr);

            auto eiter = mEvents.find(eventName);
            if (eiter != mEvents.end())
            {
                mEvents.erase(eiter);
            }
        }
    }

    bank->unloadSampleData();
    bank->unload();
    
    mBanks.erase(iter);
}

void AudioSystem::UnloadAllBanks()
{
    for (auto& b : mBanks)
    {
        b.second->unloadSampleData();
        b.second->unload();
    }
    mBanks.clear();
    mEvents.clear();
}

float AudioSystem::GetBusVolume(const std::string& name) const
{
    float volume = 0.f;
    auto iter = mBuses.find(name);
    if (iter != mBuses.end())
    {
        iter->second->getVolume(&volume);
    }

    return volume;
}

void AudioSystem::SetBusVolume(const std::string& name, float volume)
{
    auto iter = mBuses.find(name);
    if (iter != mBuses.end())
    {
        iter->second->setVolume(volume);
    }
}

bool AudioSystem::GetBusPaused(const std::string& name) const
{
    bool paused = false;
    auto iter = mBuses.find(name);
    if (iter != mBuses.end())
    {
        iter->second->getPaused(&paused);
    }

    return paused;
}

void AudioSystem::SetBusPaused(const std::string& name, bool pause)
{
    auto iter = mBuses.find(name);
    if (iter != mBuses.end())
    {
        iter->second->setPaused(pause);
    }
}

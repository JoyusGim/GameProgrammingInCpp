#include "AudioComponent.h"
#include "Actor.h"
#include "Game.h"
#include "AudioSystem.h"

AudioComponent::AudioComponent(Actor* owner, int updateOrder)   :
    Component(owner, updateOrder)
{
}

AudioComponent::~AudioComponent()
{
    StopAllEvents();
}

void AudioComponent::Update(float deltaTime)
{
    Component::Update(deltaTime);

    auto iter = mEvents2D.begin();
    while (iter != mEvents2D.end())
    {
        if (!iter->IsValid())
        {
            iter = mEvents2D.erase(iter);
        }
        else
        {
            ++iter;
        }
    }

    iter = mEvents3D.begin();
    while (iter != mEvents3D.end())
    {
        if (!iter->IsValid())
        {
            iter = mEvents3D.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

void AudioComponent::OnUpdateWorldTransform()
{
    Matrix4 world = mOwner->GetWorldTransform();
    for (auto& e : mEvents3D)
    {
        if (e.IsValid())
        {
            e.Set3DAttributes(world);            
        }
    }
}

SoundEvent AudioComponent::PlayEvent(const std::string& name)
{
    SoundEvent e = mOwner->GetGame()->GetAudioSystem()->PlayEvent(name);

    if (e.Is3D())
    {
        mEvents3D.emplace_back(e);
        e.Set3DAttributes(mOwner->GetWorldTransform());
    }
    else
    {
        mEvents2D.emplace_back(e);
    }

    return e;
}

void AudioComponent::StopAllEvents()
{
    for (auto& e : mEvents2D)
    {
        e.Stop();
    }
    mEvents2D.clear();

    for (auto& e : mEvents3D)
    {
        e.Stop();
    }
    mEvents3D.clear();
}

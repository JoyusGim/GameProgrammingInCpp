#include "SoundEvent.h"
#include "AudioSystem.h"
#include <fmod.hpp>
#include <fmod_studio.hpp>
#include <fmod_errors.h>
#include <SDL.h>

SoundEvent::SoundEvent(AudioSystem* system, unsigned int id)   :
    mSystem{ system },
    mID{ id }
{
}

SoundEvent::SoundEvent()    :
    mSystem{ nullptr },
    mID{ 0 }
{
}

bool SoundEvent::IsValid() const
{
    auto event = mSystem ?
        mSystem->GetEventInstance(mID) : nullptr;
    
    return (event != nullptr);
}

bool SoundEvent::Is3D() const
{
    bool retVal = false;
    auto event = mSystem ?
        mSystem->GetEventInstance(mID) : nullptr;
    if (event)
    {
        FMOD::Studio::EventDescription* ed = nullptr;
        event->getDescription(&ed);
        if (ed)
        {
            ed->is3D(&retVal);
        }
    }

    return retVal;
}

void SoundEvent::Restart()
{
    auto event = mSystem ?
        mSystem->GetEventInstance(mID) : nullptr;
    if (event)
    {
        FMOD_RESULT result;
        result = event->start();
        if (result != FMOD_OK)
        {
            SDL_Log(
                "Failed to Restart event %d: %s\n",
                mID, FMOD_ErrorString(result)
            );
        }
    }
}

void SoundEvent::Stop(bool allowFadeOut)
{
    auto event = mSystem ?
        mSystem->GetEventInstance(mID) : nullptr;
    if (event)
    {
        FMOD_RESULT result;
        result = event->stop(allowFadeOut ?
            FMOD_STUDIO_STOP_ALLOWFADEOUT : FMOD_STUDIO_STOP_IMMEDIATE);
        if (result != FMOD_OK)
        {
            SDL_Log(
                "Failed to Stop event %d: %s\n",
                mID, FMOD_ErrorString(result)
            );
        }
    }
}

void SoundEvent::SetPaused(bool pause)
{
    auto event = mSystem ?
        mSystem->GetEventInstance(mID) : nullptr;
    if (event)
    {
        FMOD_RESULT result;
        result = event->setPaused(pause);
        if (result != FMOD_OK)
        {
            SDL_Log(
                "Failed to SetPaused event %d: %s\n",
                mID, FMOD_ErrorString(result)
            );
        }
    }
}

void SoundEvent::SetVolume(float value)
{
    auto event = mSystem ?
        mSystem->GetEventInstance(mID) : nullptr;
    if (event)
    {
        FMOD_RESULT result;
        result = event->setVolume(value);
        if (result != FMOD_OK)
        {
            SDL_Log(
                "Failed to SetVolume event %d: %s\n",
                mID, FMOD_ErrorString(result)
            );
        }
    }
}

void SoundEvent::SetPitch(float value)
{
    auto event = mSystem ?
        mSystem->GetEventInstance(mID) : nullptr;
    if (event)
    {
        FMOD_RESULT result;
        result = event->setPitch(value);
        if (result != FMOD_OK)
        {
            SDL_Log(
                "Failed to SetPitch event %d: %s\n",
                mID, FMOD_ErrorString(result)
            );
        }
    }
}

void SoundEvent::SetParameter(const std::string& name, float value)
{
    auto event = mSystem ?
        mSystem->GetEventInstance(mID) : nullptr;
    if (event)
    {
        FMOD_RESULT result;
        result = event->setParameterValue(name.c_str(), value);
        if (result != FMOD_OK)
        {
            SDL_Log(
                "Failed to SetParameter(%s) event %d: %s\n",
                name.c_str(), mID, FMOD_ErrorString(result)
            );
        }
    }
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

void SoundEvent::Set3DAttributes(const Matrix4& worldTrans)
{
    auto event = mSystem ?
        mSystem->GetEventInstance(mID) : nullptr;
    if (event)
    {
        FMOD_3D_ATTRIBUTES attr;
        attr.position = VecToFMOD(worldTrans.GetTranslation());
        attr.forward = VecToFMOD(worldTrans.GetXAxis());
        attr.up = VecToFMOD(worldTrans.GetZAxis());
        attr.velocity = { 0.f, 0.f, 0.f };
        event->set3DAttributes(&attr);
    }
}

bool SoundEvent::GetPaused() const
{
    bool paused = false;
    auto event = mSystem ?
        mSystem->GetEventInstance(mID) : nullptr;
    if (event)
    {
        FMOD_RESULT result;
        result = event->getPaused(&paused);
        if (result != FMOD_OK)
        {
            SDL_Log(
                "Failed to GetPaused event %d: %s\n",
                mID, FMOD_ErrorString(result)
            );
        }
    }
    return paused;
}

float SoundEvent::GetVolume() const
{
    float volume = 0.f;
    auto event = mSystem ?
        mSystem->GetEventInstance(mID) : nullptr;
    if (event)
    {
        FMOD_RESULT result;
        result = event->getVolume(&volume);
        if (result != FMOD_OK)
        {
            SDL_Log(
                "Failed to GetVolume event %d: %s\n",
                mID, FMOD_ErrorString(result)
            );
        }
    }
    return volume;
}

float SoundEvent::GetPitch() const
{
    float pitch = 0.f;
    auto event = mSystem ?
        mSystem->GetEventInstance(mID) : nullptr;
    if (event)
    {
        FMOD_RESULT result;
        result = event->getVolume(&pitch);
        if (result != FMOD_OK)
        {
            SDL_Log(
                "Failed to GetPitch event %d: %s\n",
                mID, FMOD_ErrorString(result)
            );
        }
    }
    return pitch;
}

float SoundEvent::GetParameter(const std::string& name) const
{
    float value = 0.f;
    auto event = mSystem ?
        mSystem->GetEventInstance(mID) : nullptr;
    if (event)
    {
        FMOD_RESULT result;
        result = event->getParameterValue(name.c_str(), &value);
        if (result != FMOD_OK)
        {
            SDL_Log(
                "Failed to GetParameter(%s) event %d: %s\n",
                name.c_str(), mID, FMOD_ErrorString(result)
            );
        }
    }
    return value;
}

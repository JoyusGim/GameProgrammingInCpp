#pragma once
#include "SoundEvent.h"
#include "Math.h"
#include <unordered_map>
#include <string>

namespace FMOD {
	class System;
	namespace Studio {
	class System;
	class Bank;
	class EventDescription;
	class EventInstance;
	class Bus;
	};
};

class AudioSystem
{
	static unsigned int sNextID;

	class Game* mGame;
	class FMOD::Studio::System* mSystem;
	class FMOD::System* mLowLevelSystem;

	std::unordered_map<std::string, FMOD::Studio::Bank*> mBanks;
	std::unordered_map<std::string, FMOD::Studio::EventDescription*> mEvents;
	std::unordered_map<std::string, FMOD::Studio::Bus*> mBuses;
	std::unordered_map<unsigned int, FMOD::Studio::EventInstance*> mEventInstances;

protected:
	friend class SoundEvent;
	FMOD::Studio::EventInstance* GetEventInstance(unsigned int id) const;

public:
	AudioSystem(class Game* game);
	~AudioSystem();

	bool Initialize();
	void Shutdown();
	void Update(float deltaTime);

	SoundEvent PlayEvent(const std::string& name);
	void SetListener(const Matrix4& viewMatrix);

	void LoadBank(const std::string& name);
	void UnloadBank(const std::string& name);
	void UnloadAllBanks();

	float GetBusVolume(const std::string& name) const;
	void SetBusVolume(const std::string& name, float volume);
	bool GetBusPaused(const std::string& name) const;
	void SetBusPaused(const std::string& name, bool pause);
};


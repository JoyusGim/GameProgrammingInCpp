#pragma once
#include <string>
#include "Math.h"

class SoundEvent
{
	class AudioSystem* mSystem;
	unsigned int mID;

protected:
	friend class AudioSystem;
	SoundEvent(class AudioSystem* system, unsigned int id);

public:
	SoundEvent();
	
	// ���� FMOD �̺�Ʈ �ν��Ͻ��� �����ϸ� true��ȯ
	bool IsValid() const;
	bool Is3D() const;

	void Restart();
	void Stop(bool allowFadeOut = false);

	void SetPaused(bool pause);
	void SetVolume(float value);
	void SetPitch(float value);
	void SetParameter(const std::string& name, float value);
	void Set3DAttributes(const Matrix4& worldTrans);

	bool GetPaused() const;
	float GetVolume() const;
	float GetPitch() const;
	float GetParameter(const std::string& name) const;
};


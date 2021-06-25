#pragma once
#include <SDL.h>
#include <vector>
#include <unordered_map>
#include <string>

class Game
{
	class Renderer* mRenderer;
	class AudioSystem* mAudioSystem;
	bool mIsRunning;

	Uint32 mTickCount;

	std::vector<class Actor*> mActors;
	std::vector<class Actor*> mPendingActors;
	bool mUpdatingActor;

	class CameraActor* mCameraActor;
	
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();

	void LoadData();
	void UnloadData();

public:
	Game();

	bool Initialize();
	void RunLoop();
	void Shutdown();

	void AddActor(class Actor* actor);
	void RemoveActor(class Actor* actor);

	class Renderer* GetRenderer() const;
	class AudioSystem* GetAudioSystem() const;
};


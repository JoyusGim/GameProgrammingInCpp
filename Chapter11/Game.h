#pragma once
#include <SDL.h>
#include <vector>
#include <unordered_map>
#include <string>

class Game
{
	class Renderer* mRenderer;
	class AudioSystem* mAudioSystem;
	class InputSystem* mInputSystem;
	class PhysWorld* mPhysWorld;
	bool mIsRunning;

	Uint32 mTickCount;

	std::vector<class Actor*> mActors;
	std::vector<class Actor*> mPendingActors;
	bool mUpdatingActor;

	class FPSActor* mFPSActor;
	
	std::vector<class PlaneActor*> mPlanes;

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

	void AddPlane(class PlaneActor* plane);

	class Renderer* GetRenderer() const;
	class AudioSystem* GetAudioSystem() const;
	class PhysWorld* GetPhysWorld() const;
	std::vector<class PlaneActor*>& GetPlanes();
};


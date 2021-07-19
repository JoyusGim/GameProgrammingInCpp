#pragma once
#include <SDL.h>
#include <vector>
#include <unordered_map>
#include <string>

class Game
{
public:
	enum GameState { GamePlay, GamePause, GameExit };

private:
	class Renderer* mRenderer;
	class AudioSystem* mAudioSystem;
	class InputSystem* mInputSystem;
	class PhysWorld* mPhysWorld;
	bool mIsRunning;

	Uint32 mTickCount;

	std::vector<class Actor*> mActors;
	std::vector<class Actor*> mPendingActors;
	bool mUpdatingActor;

	class FollowActor* mFollowActor;
	
	std::vector<class PlaneActor*> mPlanes;

	std::unordered_map<std::string, class Font*> mFonts;

	class HUD* mHUD;
	std::vector<class UIScreen*> mUIStack;

	std::unordered_map<std::string, std::string> mTextMap;

	std::unordered_map<std::string, class Skeleton*> mSkeletons;
	std::unordered_map<std::string, class Animation*> mAnimations;

	GameState mGameState;

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
	const std::vector<class Actor*>& GetActors() const;
	void RemoveActor(class Actor* actor);

	void AddPlane(class PlaneActor* plane);

	void LoadText(const std::string& fileName);

	class FollowActor* GetPlayer() const;
	void SetPlayer(class FollowActor* actor);
	class Renderer* GetRenderer() const;
	class InputSystem* GetInputSystem() const;
	class AudioSystem* GetAudioSystem() const;
	class PhysWorld* GetPhysWorld() const;
	std::vector<class PlaneActor*>& GetPlanes();

	void PushUI(class UIScreen* screen);
	std::vector<class UIScreen*>& GetUIStack();

	class Font* GetFont(const std::string& name);

	class Skeleton* GetSkeleton(const std::string& fileName);
	class Animation* GetAnimation(const std::string& fileName);

	void SetGameState(const GameState& state);
	GameState GetGameState() const;

	class HUD* GetHUD() const;

	const std::string& GetText(const std::string& key);
};


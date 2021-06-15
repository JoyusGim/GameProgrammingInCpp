#pragma once
#include <SDL.h>
#include <vector>
#include <unordered_map>
#include <string>

class Game
{
	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;
	bool mIsRunning;

	Uint32 mTickCount;

	std::vector<class Actor*> mActors;
	std::vector<class Actor*> mPendingActors;
	bool mUpdatingActor;

	std::unordered_map<std::string, SDL_Texture*> mTextures;
	std::vector<class SpriteComponent*> mSprites;
	
	std::vector<class Enemy*> mEnemies;
	class Grid* mGrid;

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

	void AddSpriteComponent(class SpriteComponent* sprite);
	void RemoveSpriteComponent(class SpriteComponent* sprite);

	void AddEnemy(class Enemy* enemy);
	void RemoveEnemy(class Enemy* enemy);
	std::vector<class Enemy*>& GetEnemies();

	class Grid* GetGrid() const { return mGrid; }

	SDL_Texture* GetTexture(const std::string& fileName);
};


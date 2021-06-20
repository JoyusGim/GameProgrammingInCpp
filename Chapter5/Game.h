#pragma once
#include <SDL.h>
#include <vector>
#include <unordered_map>
#include <string>

class Game
{
	SDL_Window* mWindow;
	SDL_GLContext mContext;
	bool mIsRunning;

	Uint32 mTickCount;

	std::vector<class Actor*> mActors;
	std::vector<class Actor*> mPendingActors;
	std::vector<class Asteroid*> mAsteroids;
	bool mUpdatingActor;

	std::unordered_map<std::string, class Texture*> mTextures;
	std::vector<class SpriteComponent*> mSprites;
	class Shader* mSpriteShader;
	class VertexArray* mSpriteVerts;
	
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();

	void LoadData();
	void UnloadData();

	bool LoadShaders();
	bool InitSpriteVerts();
public:
	Game();

	bool Initialize();
	void RunLoop();
	void Shutdown();

	void AddActor(class Actor* actor);
	void RemoveActor(class Actor* actor);

	void AddSpriteComponent(class SpriteComponent* sprite);
	void RemoveSpriteComponent(class SpriteComponent* sprite);

	class Texture* GetTexture(const std::string& fileName);

	void AddAsteroid(class Asteroid* asteroid);
	void RemoveAsteroid(class Asteroid* asteroid);

	std::vector<Asteroid*>& GetAsteroids();
};


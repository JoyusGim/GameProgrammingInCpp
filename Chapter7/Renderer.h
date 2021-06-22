#pragma once
#include <SDL.h>
#include <vector>
#include <unordered_map>
#include "Math.h"

struct DirectionalLight
{
	Vector3 mDirection;
	Vector3 mDiffuseColor;
	Vector3 mSpecColor;
};

class Renderer
{
	class Game* mGame;
	SDL_GLContext mContext;
	SDL_Window* mWindow;

	std::unordered_map<std::string, class Texture*> mTextures;
	std::unordered_map<std::string, class Mesh*> mMeshes;

	std::vector<class SpriteComponent*> mSprites;
	class Shader* mSpriteShader;
	class VertexArray* mSpriteVerts;

	std::vector<class MeshComponent*> mMeshComponents;
	class Shader* mMeshShader;

	Matrix4 mView;
	Matrix4 mProjection;

	float mScreenWidth;
	float mScreenHeight;

	Vector3 mAmbientLight;
	DirectionalLight mDirLight;

	bool LoadShaders();
	bool InitSpriteVerts();
	void SetLightUniforms(class Shader* shader);

public:
	Renderer(class Game* game);
	~Renderer();

	bool Initialize(float screenWidth, float screenHeight);
	void Shutdown();
	void UnloadData();

	void Draw();

	void AddSpriteComponent(class SpriteComponent* sprite);
	void RemoveSpriteComponent(class SpriteComponent* sprite);

	void AddMeshComponent(class MeshComponent* mesh);
	void RemoveMeshComponent(class MeshComponent* mesh);

	class Texture* GetTexture(const std::string& fileName);
	class Mesh* GetMesh(const std::string& fileName);

	float GetScreenWidth() const;
	float GetScreenHeight() const;

	void SetAmbientLight(const Vector3& ambient);
	DirectionalLight& GetDirectionalLight();
};


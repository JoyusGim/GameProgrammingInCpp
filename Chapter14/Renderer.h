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

	std::vector<class SkeletalMeshComponent*> mSkeletalMeshes;
	class Shader* mSkinnedShader;

	unsigned int mMirrorBuffer;
	class Texture* mMirrorTexture;

	class Shader* mGGlobalShader;
	class GBuffer* mGBuffer;

	std::vector<class PointLightComponent*> mPointLights;
	class Shader* mGPointLightShader;
	class Mesh* mPointLightMesh;

	Matrix4 mView;
	Matrix4 mMirrorView;
	Matrix4 mProjection;

	float mScreenWidth;
	float mScreenHeight;

	Vector3 mAmbientLight;
	DirectionalLight mDirLight;

	bool LoadShaders();
	bool InitSpriteVerts();
	void SetLightUniforms(class Shader* shader, const Matrix4& view);

	void Draw3DScene(unsigned int framebuffer, const Matrix4& view, const Matrix4& proj, float viewportScale = 1.f, bool lit = false);
	void DrawFromGBuffer();

public:
	Renderer(class Game* game);
	~Renderer();

	bool Initialize(float screenWidth, float screenHeight);
	void Shutdown();
	void UnloadData();

	void Draw();

	void AddSpriteComponent(class SpriteComponent* sprite);
	void RemoveSpriteComponent(class SpriteComponent* sprite);

	void AddMesh(class MeshComponent* mesh);
	void RemoveMesh(class MeshComponent* mesh);

	bool CreateMirrorTarget();

	class Texture* GetTexture(const std::string& fileName);
	class Mesh* GetMesh(const std::string& fileName);

	float GetScreenWidth() const;
	float GetScreenHeight() const;

	void SetViewMatrix(const Matrix4& view);
	
	void SetMirrorViewMatrix(const Matrix4& mirrorView);
	class Texture* GetMirrorTexture() const;

	void AddPointLight(class PointLightComponent* light);
	void RemovePointLight(class PointLightComponent* light);

	void SetAmbientLight(const Vector3& ambient);
	DirectionalLight& GetDirectionalLight();

	Vector3 Unproject(const Vector3& screenPoint) const;
	void GetScreenDirection(Vector3& outStart, Vector3& outDir) const;
};


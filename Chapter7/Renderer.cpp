#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"
#include "VertexArray.h"
#include "SpriteComponent.h"
#include "MeshComponent.h"
#include <glew.h>

bool Renderer::LoadShaders()
{
    mSpriteShader = new Shader();
    if (!mSpriteShader->Load("Shaders/Sprite.vert", "Shaders/Sprite.frag"))
    {
        return false;
    }
    mSpriteShader->SetActive();
    Matrix4 viewProj = Matrix4::CreateSimpleViewProj(mScreenWidth, mScreenHeight);
    mSpriteShader->SetMatrixUniform("uViewProj", viewProj);

    mMeshShader = new Shader();
    if (!mMeshShader->Load("Shaders/Phong.vert", "Shaders/Phong.frag"))
    {
        return false;
    }
    mMeshShader->SetActive();
    
    mView = Matrix4::CreateLookAt(
        Vector3::Zero,
        Vector3::UnitX,
        Vector3::UnitZ
    );
    mProjection = Matrix4::CreatePerspectiveFOV(
        Math::ToRadians(70.f),
        mScreenWidth,
        mScreenHeight,
        25.f,
        10000.f
    );
    mMeshShader->SetMatrixUniform("uViewProj", mView * mProjection);
    
    return true;
}

bool Renderer::InitSpriteVerts()
{
    float vertexBuffer[] = {
        -0.5f, 0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 0.f, // top left
        0.5f, 0.5f, 0.f, 0.f, 0.f, 0.0f, 1.f, 0.f, // top right
        0.5f,-0.5f, 0.f, 0.f, 0.f, 0.0f, 1.f, 1.f, // bottom right
        -0.5f,-0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 1.f  // bottom left
    };

    unsigned int indexBuffer[] = {
        0, 1, 2,
        2, 3, 0
    };

    mSpriteVerts = new VertexArray(vertexBuffer, 4, indexBuffer, 6);

    return true;
}

void Renderer::SetLightUniforms(Shader* shader)
{
    Matrix4 invView = mView;
    invView.Invert();

    shader->SetVectorUniform("uCameraPos", invView.GetTranslation());

    shader->SetVectorUniform("uAmbientLight", mAmbientLight);
    shader->SetVectorUniform("uDirLight.mDirection", mDirLight.mDirection);
    shader->SetVectorUniform("uDirLight.mDiffuseColor", mDirLight.mDiffuseColor);
    shader->SetVectorUniform("uDirLight.mSpecColor", mDirLight.mSpecColor);
}

Renderer::Renderer(Game* game)  :
    mGame{ game },
    mContext{},
    mWindow{ nullptr },
    mSpriteShader{ nullptr },
    mSpriteVerts{ nullptr },
    mMeshShader{ nullptr },
    mScreenWidth{ 0.f },
    mScreenHeight{ 0.f }
{
}

Renderer::~Renderer()
{
}

bool Renderer::Initialize(float screenWidth, float screenHeight)
{
    mScreenWidth = screenWidth;
    mScreenHeight = screenHeight;

    int sdlResult = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    if (sdlResult != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    mWindow = SDL_CreateWindow(
        "Game Programming in C++ Chapter 7",
        100, 100,
        static_cast<int>(mScreenWidth), static_cast<int>(mScreenHeight),
        SDL_WINDOW_OPENGL
    );

    if (!mWindow)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mContext = SDL_GL_CreateContext(mWindow);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        SDL_Log("Failed to initialize GLEW.");
        return false;
    }
    glGetError();

    if (!LoadShaders())
        return false;

    if (!InitSpriteVerts())
        return false;

    SetLightUniforms(mMeshShader);

    return true;
}

void Renderer::Shutdown()
{
    delete mSpriteVerts;
    mSpriteShader->Unload();
    delete mSpriteShader;
    mMeshShader->Unload();
    delete mMeshShader;

    SDL_GL_DeleteContext(mContext);
    SDL_DestroyWindow(mWindow);
}

void Renderer::UnloadData()
{
    for (auto tex : mTextures)
    {
        tex.second->Unload();
        delete tex.second;
    }
    mTextures.clear();

    for (auto m : mMeshes)
    {
        m.second->Unload();
        delete m.second;
    }
    mMeshes.clear();
}

void Renderer::Draw()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    mMeshShader->SetActive();
    mMeshShader->SetMatrixUniform("uViewProj", mView * mProjection);
    SetLightUniforms(mMeshShader);
    for (auto mc : mMeshComponents)
    {
        mc->Draw(mMeshShader);
    }

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

    mSpriteShader->SetActive();
    mSpriteVerts->SetActive();

    for (auto sprite : mSprites)
    {
        sprite->Draw(mSpriteShader);
    }

    SDL_GL_SwapWindow(mWindow);
}

void Renderer::AddSpriteComponent(SpriteComponent* sprite)
{
    int drawOrder = sprite->GetDrawOrder();
    auto iter = mSprites.begin();
    for (; iter != mSprites.end(); ++iter)
    {
        if (drawOrder < (*iter)->GetDrawOrder())
            break;
    }

    mSprites.insert(iter, sprite);
}

void Renderer::RemoveSpriteComponent(SpriteComponent* sprite)
{
    // drawOrder를 망치지 않기위해 swap을 하지 않는다.
    auto iter = std::find(mSprites.begin(), mSprites.end(), sprite);
    mSprites.erase(iter);
}

void Renderer::AddMeshComponent(MeshComponent* mesh)
{
    mMeshComponents.emplace_back(mesh);
}

void Renderer::RemoveMeshComponent(MeshComponent* mesh)
{
    auto iter = std::find(mMeshComponents.begin(), mMeshComponents.end(), mesh);
    std::iter_swap(iter, mMeshComponents.end() - 1);
    mMeshComponents.pop_back();
}

Texture* Renderer::GetTexture(const std::string& fileName)
{
    Texture* texture = nullptr;

    auto iter = mTextures.find(fileName);
    if (iter != mTextures.end())
    {
        texture = iter->second;
    }
    else
    {
        texture = new Texture();
        if (!texture->Load(fileName))
            return nullptr;

        mTextures.emplace(fileName.c_str(), texture);
    }

    return texture;
}

Mesh* Renderer::GetMesh(const std::string& fileName)
{
    Mesh* mesh = nullptr;

    auto iter = mMeshes.find(fileName);
    if (iter != mMeshes.end())
    {
        mesh = iter->second;
    }
    else
    {
        mesh = new Mesh();
        if (!mesh->Load(fileName, mGame))
            return nullptr;

        mMeshes.emplace(fileName.c_str(), mesh);
    }

    return mesh;
}

float Renderer::GetScreenWidth() const
{
    return mScreenWidth;
}

float Renderer::GetScreenHeight() const
{
    return mScreenHeight;
}

void Renderer::SetViewMatrix(const Matrix4& view)
{
    mView = view;
}

void Renderer::SetAmbientLight(const Vector3& ambient)
{
    mAmbientLight = ambient;
}

DirectionalLight& Renderer::GetDirectionalLight()
{
    return mDirLight;
}

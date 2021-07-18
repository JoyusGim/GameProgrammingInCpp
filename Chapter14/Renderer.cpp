#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"
#include "VertexArray.h"
#include "SpriteComponent.h"
#include "MeshComponent.h"
#include "SkeletalMeshComponent.h"
#include <glew.h>
#include "Game.h"
#include "UIScreen.h"
#include "GBuffer.h"
#include "PointLightComponent.h"

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
    if (!mMeshShader->Load("Shaders/Phong.vert", "Shaders/GBufferWrite.frag"))
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

    mSkinnedShader = new Shader();
    if (!mSkinnedShader->Load("Shaders/Skinned.vert", "Shaders/GBufferWrite.frag"))
    {
        return false;
    }
    mSkinnedShader->SetActive();

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
    mSkinnedShader->SetMatrixUniform("uViewProj", mView * mProjection);
    
    mGGlobalShader = new Shader();
    if (!mGGlobalShader->Load("Shaders/BasicMesh.vert", "Shaders/GBufferGlobal.frag"))
    {
        return false;
    }
    mGGlobalShader->SetActive();
    mGGlobalShader->SetIntUniform("uGDiffuse", 0);
    mGGlobalShader->SetIntUniform("uGNormal", 1);
    mGGlobalShader->SetIntUniform("uGWorldPos", 2);
    mGGlobalShader->SetIntUniform("uGSpecPower", 3);

    mGGlobalShader->SetMatrixUniform("uViewProj", viewProj);
    Matrix4 gbufferWorld = Matrix4::CreateScale(mScreenWidth, -mScreenHeight, 1.f);
    mGGlobalShader->SetMatrixUniform("uWorldTransform", gbufferWorld);


    mGPointLightShader = new Shader();
    if (!mGPointLightShader->Load("Shaders/BasicMesh.vert", "Shaders/GBufferPointLight.frag"))
    {
        return false;
    }
    mGPointLightShader->SetActive();
    mGPointLightShader->SetIntUniform("uGDiffuse", 0);
    mGPointLightShader->SetIntUniform("uGNormal", 1);
    mGPointLightShader->SetIntUniform("uGWorldPos", 2);
    mGPointLightShader->SetIntUniform("uGSpecPower", 3);
    mGPointLightShader->SetVector2Uniform("uScreenDimensions", Vector2(mScreenWidth, mScreenHeight));

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

    mSpriteVerts = new VertexArray(vertexBuffer, 4, VertexArray::PosNormTex, indexBuffer, 6);

    return true;
}

void Renderer::SetLightUniforms(Shader* shader, const Matrix4& view)
{
    Matrix4 invView = view;
    invView.Invert();

    shader->SetVectorUniform("uCameraPos", invView.GetTranslation());

    shader->SetVectorUniform("uAmbientLight", mAmbientLight);
    shader->SetVectorUniform("uDirLight.mDirection", mDirLight.mDirection);
    shader->SetVectorUniform("uDirLight.mDiffuseColor", mDirLight.mDiffuseColor);
    shader->SetVectorUniform("uDirLight.mSpecColor", mDirLight.mSpecColor);
}

void Renderer::Draw3DScene(unsigned int framebuffer, const Matrix4& view,
    const Matrix4& proj, float viewportScale /* = 1.f */, bool lit /* = false*/)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    
    glViewport(0, 0,
        static_cast<int>(mScreenWidth * viewportScale),
        static_cast<int>(mScreenHeight * viewportScale)
    );

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glDepthMask(GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    mMeshShader->SetActive();
    mMeshShader->SetMatrixUniform("uViewProj", view * proj);
    if (lit)
    {
        SetLightUniforms(mMeshShader, mView);
    }
    for (auto mc : mMeshComponents)
    {
        mc->Draw(mMeshShader);
    }

    mSkinnedShader->SetActive();
    mSkinnedShader->SetMatrixUniform("uViewProj", view * proj);
    if (lit)
    {
        SetLightUniforms(mSkinnedShader, view);
    }
    for (auto sk : mSkeletalMeshes)
    {
        sk->Draw(mSkinnedShader);
    }
}

void Renderer::DrawFromGBuffer()
{
    glDisable(GL_DEPTH_TEST);
    mGGlobalShader->SetActive();
    mSpriteVerts->SetActive();
    mGBuffer->SetTexturesActive();
    SetLightUniforms(mGGlobalShader, mView);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, mGBuffer->GetBufferID());
    int width = static_cast<int>(mScreenWidth);
    int height = static_cast<int>(mScreenHeight);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    mGPointLightShader->SetActive();
    mPointLightMesh->GetVertexArray()->SetActive();
    mGPointLightShader->SetMatrixUniform("uViewProj", mView * mProjection);
    Matrix4 invView = mView; invView.Invert();
    mGPointLightShader->SetVectorUniform("uCameraPos", invView.GetTranslation());
    mGBuffer->SetTexturesActive();

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    for (PointLightComponent* p : mPointLights)
    {
        p->Draw(mGPointLightShader, mPointLightMesh);
    }
}

Renderer::Renderer(Game* game)  :
    mGame{ game },
    mContext{},
    mWindow{ nullptr },
    mSpriteShader{ nullptr },
    mSpriteVerts{ nullptr },
    mMeshShader{ nullptr },
    mSkinnedShader{ nullptr },
    mMirrorTexture{ nullptr },
    mMirrorBuffer{ 0 },
    mScreenWidth{ 0.f },
    mScreenHeight{ 0.f }
{
}

Renderer::~Renderer()
{
}

bool Renderer::Initialize(float screenWidth, float screenHeight /* = 1.f */)
{
    mScreenWidth = screenWidth;
    mScreenHeight = screenHeight;

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
        "Game Programming in C++ Chapter 14",
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

    mGBuffer = new GBuffer();
    int width = static_cast<int>(mScreenWidth);
    int height = static_cast<int>(mScreenHeight);
    if (!mGBuffer->Create(width, height))
    {
        SDL_Log("Failed to create G-buffer.");
        return false;
    }

    mPointLightMesh = GetMesh("Assets/PointLight.gpmesh");

    return true;
}

void Renderer::Shutdown()
{
    delete mSpriteVerts;
    mSpriteShader->Unload();
    delete mSpriteShader;
    mMeshShader->Unload();
    delete mMeshShader;
    mSkinnedShader->Unload();
    delete mSkinnedShader;
    mGGlobalShader->Unload();
    delete mGGlobalShader;
    mGBuffer->Destroy();
    delete mGBuffer;
    mGPointLightShader->Unload();
    delete mGPointLightShader;

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

    mMirrorTexture->Unload();
    delete mMirrorTexture;

    for (auto m : mMeshComponents)
    {
        delete m;
    }
    mMeshComponents.clear();

    for (auto lit : mPointLights)
    {
        delete lit;
    }
    mPointLightMesh = nullptr;

    glDeleteBuffers(1, &mMirrorBuffer);
}

void Renderer::Draw()
{
    Draw3DScene(mMirrorBuffer, mMirrorView, mProjection, 0.25f);
    Draw3DScene(mGBuffer->GetBufferID(), mView, mProjection, 1.f, false);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    DrawFromGBuffer();

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

    for (auto ui : mGame->GetUIStack())
    {
        ui->Draw(mSpriteShader);
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

void Renderer::AddMesh(MeshComponent* mesh)
{
    if (mesh->GetIsSkeletal())
    {
        SkeletalMeshComponent* sk = reinterpret_cast<SkeletalMeshComponent*>(mesh);
        mSkeletalMeshes.emplace_back(sk);
    }
    else
    {
        mMeshComponents.emplace_back(mesh);
    }
}

void Renderer::RemoveMesh(MeshComponent* mesh)
{
    if (mesh->GetIsSkeletal())
    {
        SkeletalMeshComponent* sk = reinterpret_cast<SkeletalMeshComponent*>(mesh);
        auto iter = std::find(mSkeletalMeshes.begin(), mSkeletalMeshes.end(), sk);
        mSkeletalMeshes.erase(iter);
    }
    else
    {
        auto iter = std::find(mMeshComponents.begin(), mMeshComponents.end(), mesh);
        mMeshComponents.erase(iter);
    }
}

bool Renderer::CreateMirrorTarget()
{
    int width = static_cast<int>(mScreenWidth) / 4;
    int height = static_cast<int>(mScreenHeight) / 4;

    glGenFramebuffers(1, &mMirrorBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, mMirrorBuffer);

    mMirrorTexture = new Texture();
    mMirrorTexture->CreateForRendering(width, height, GL_RGB);

    GLuint depthBuffer;
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mMirrorTexture->GetTextureID(), 0);

    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        glDeleteFramebuffers(1, &mMirrorBuffer);
        mMirrorTexture->Unload();
        delete mMirrorTexture;
        mMirrorTexture = nullptr;
        return false;
    }

    return true;
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
        if (mesh->Load(fileName, this))
        {
            mMeshes.emplace(fileName, mesh);
        }
        else
        {
            delete mesh;
            return nullptr;
        }

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

void Renderer::SetMirrorViewMatrix(const Matrix4& mirrorView)
{
    mMirrorView = mirrorView;
}

Texture* Renderer::GetMirrorTexture() const
{
    return mMirrorTexture;
}

void Renderer::AddPointLight(PointLightComponent* light)
{
    mPointLights.emplace_back(light);
}

void Renderer::RemovePointLight(PointLightComponent* light)
{
    auto iter = std::find(mPointLights.begin(), mPointLights.end(), light);
    mPointLights.erase(iter);
}

void Renderer::SetAmbientLight(const Vector3& ambient)
{
    mAmbientLight = ambient;
}

DirectionalLight& Renderer::GetDirectionalLight()
{
    return mDirLight;
}

Vector3 Renderer::Unproject(const Vector3& screenPoint) const
{
    // 화면 좌표를 장치 좌표로 변환 [-1, 1]범위
    Vector3 deviceCoord = screenPoint;
    deviceCoord.x /= mScreenWidth * 0.5f;
    deviceCoord.y /= mScreenHeight * 0.5f;

    // 벡터를 언프로젝션 행렬로 변환
    Matrix4 unprojection = mView * mProjection;
    unprojection.Invert();
    return Vector3::TransformWithPerspDiv(deviceCoord, unprojection);
}

void Renderer::GetScreenDirection(Vector3& outStart, Vector3& outDir) const
{
    Vector3 screenPoint(0.f, 0.f, 0.f);
    outStart = Unproject(screenPoint);
    screenPoint.z = 0.9f;
    Vector3 end = Unproject(screenPoint);

    outDir = end - outStart;
    outDir.Normalize();
}

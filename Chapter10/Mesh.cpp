#include "Mesh.h"
#include "Game.h"
#include "Renderer.h"
#include "Math.h"
#include "VertexArray.h"
#include <SDL.h>
#include <fstream>
#include <sstream>
#include <rapidjson\document.h>

Mesh::Mesh()    :
    mVertexArray{ nullptr },
    mRadius{ 0.f },
    mSpecPower{ 0.f },
    mBox{Vector3::Infinity, Vector3::NegInfinity}
{
}

Mesh::~Mesh()
{
}

bool Mesh::Load(const std::string& fileName, Game* game)
{
    std::ifstream ifs(fileName);
    if (!ifs.is_open())
    {
        SDL_Log("File not found: Mesh %s", fileName.c_str());
        return false;
    }

    std::stringstream ss;
    ss << ifs.rdbuf();
    std::string contents = ss.str();
    rapidjson::StringStream jsonStr(contents.c_str());
    rapidjson::Document doc;
    doc.ParseStream(jsonStr);

    if (!doc.IsObject())
    {
        SDL_Log("Mesh %s is not valid json", fileName.c_str());
        return false;
    }

    int ver = doc["version"].GetInt();

    // gpmesh ������ ���� Ȯ��
    if (ver != 1)
    {
        SDL_Log("Mesh %s not version 1", fileName.c_str());
        return false;
    }

    mShaderName = doc["shader"].GetString();

    // ���ؽ� ���̾ƿ� ������ ���� é�Ϳ��� �ٷ��.
    // ����� �ϵ��ڵ�
    size_t vertSize = 8;

    // �Ž��� �ؽ�ó ��� �ε�
    const rapidjson::Value& textures = doc["textures"];
    if (!textures.IsArray() || textures.Size() < 1)
    {
        SDL_Log("Mesh %s has no textures, there should be at least one", fileName.c_str());
        return false;
    }

    // �ؽ�ó �ε�
    for (rapidjson::SizeType i = 0; i < textures.Size(); i++)
    {
        std::string texName = textures[i].GetString();

        Texture* t = game->GetRenderer()->GetTexture(texName);
        // �Ž��� ��ϵ� �ؽ�ó�� �ε��ϴµ� �����ߴٸ�
        if (t == nullptr)
        {
            // �⺻ �ؽ�ó�� �ε�
            t = game->GetRenderer()->GetTexture("Assets/Default.png");

            // �⺻ �ؽ�ó�� �ε��� �����ϸ�
            if (t == nullptr)
            {
                SDL_Log("Failed to load Texture %s of Mesh %s",
                    texName.c_str(), fileName.c_str());
                return false;
            }
        }
        mTextures.emplace_back(t);
    }

    // specPower �ε�
    mSpecPower = static_cast<float>(doc["specularPower"].GetDouble());

    // ���ؽ� �ε�
    const rapidjson::Value& vertsJson = doc["vertices"];
    if (!vertsJson.IsArray() || vertsJson.Size() < 1)
    {
        SDL_Log("Mesh %s has no vertices", fileName.c_str());
        return false;
    }

    std::vector<float> vertices;
    vertices.reserve(vertsJson.Size() * vertSize);
    mRadius = 0.f;
    for (rapidjson::SizeType i = 0; i < vertsJson.Size(); i++)
    {
        // ������ ���ؽ� ���̾ƿ��� 8���� float �ΰɷ� ����
        const rapidjson::Value& vert = vertsJson[i];
        if (!vert.IsArray() || vert.Size() != 8)
        {
            SDL_Log("Unexpected vertex format for %s", fileName.c_str());
            return false;
        }

        Vector3 pos(vert[0].GetDouble(), vert[1].GetDouble(), vert[2].GetDouble());
        mRadius = Math::Max(mRadius, pos.SquareLength());
        mBox.UpdateMinMax(pos);

        for (rapidjson::SizeType i = 0; i < vert.Size(); i++)
        {
            vertices.emplace_back(static_cast<float>(vert[i].GetDouble()));
        }
    }

    // ������ �������� ���Ͽ� mRadius�� ���������Ƿ�
    // ���� mRadius�� ������ ������ �ؾ���.
    mRadius = Math::Sqrt(mRadius);

    // �ε��� �ε�
    const rapidjson::Value& indJson = doc["indices"];
    if (!indJson.IsArray() || indJson.Size() < 1)
    {
        SDL_Log("Mesh %s has no indices", fileName.c_str());
        return false;
    }

    std::vector<unsigned int> indices;
    indices.reserve(indJson.Size() * 3);
    for (rapidjson::SizeType i = 0; i < indJson.Size(); i++)
    {
        const rapidjson::Value& ind = indJson[i];
        if (!ind.IsArray() || ind.Size() != 3)
        {
            SDL_Log("Invalid indices for %s", fileName.c_str());
            return false;
        }

        indices.emplace_back(ind[0].GetUint());
        indices.emplace_back(ind[1].GetUint());
        indices.emplace_back(ind[2].GetUint());
    }

    // ���ؽ� ��� ����
    mVertexArray = new VertexArray(
        vertices.data(), static_cast<unsigned int>(vertices.size()) / vertSize,
        indices.data(), static_cast<unsigned int>(indices.size())
    );

    return true;
}

void Mesh::Unload()
{
    delete mVertexArray;
    mVertexArray = nullptr;
}

VertexArray* Mesh::GetVertexArray() const
{
    return mVertexArray;
}

Texture* Mesh::GetTexture(size_t index) const
{
    if (index < mTextures.size())
        return mTextures[index];
    else
        return nullptr;
}

const std::string& Mesh::GetShaderName() const
{
    return mShaderName;
}

float Mesh::GetRadius() const
{
    return mRadius;
}

float Mesh::GetSpecPower() const
{
    return mSpecPower;
}

const AABB& Mesh::GetBox() const
{
    return mBox;
}

#include "Mesh.h"
#include "Game.h"
#include "Renderer.h"
#include "Math.h"
#include "VertexArray.h"
#include <SDL.h>
#include <fstream>
#include <sstream>
#include <rapidjson\document.h>

namespace
{
union Vertex
{
    float f;
    uint8_t b[4];
};
}

Mesh::Mesh()    :
    mVertexArray{ nullptr },
    mRadius{ 0.f },
    mSpecPower{ 100.f },
    mBox{Vector3::Infinity, Vector3::NegInfinity}
{
}

Mesh::~Mesh()
{
}

bool Mesh::Load(const std::string& fileName, Renderer* renderer)
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
    VertexArray::Layout layout = VertexArray::PosNormTex;
    size_t vertSize = 8;

    std::string vertexFormat = doc["vertexformat"].GetString();
    if (vertexFormat == "PosNormSkinTex")
    {
        layout = VertexArray::PosNormSkinTex;
        // This is the number of "Vertex" unions, which is 8 + 2 (for skinning)s
        vertSize = 10;
    }

    // �Ž��� �ؽ�ó ��� �ε�
    const rapidjson::Value& textures = doc["textures"];
    if (!textures.IsArray() || textures.Size() < 1)
    {
        SDL_Log("Mesh %s has no textures, there should be at least one", fileName.c_str());
        return false;
    }

    // specPower �ε�
    mSpecPower = static_cast<float>(doc["specularPower"].GetDouble());

    // �ؽ�ó �ε�
    for (rapidjson::SizeType i = 0; i < textures.Size(); i++)
    {
        std::string texName = textures[i].GetString();

        Texture* t = renderer->GetTexture(texName);
        // �Ž��� ��ϵ� �ؽ�ó�� �ε��ϴµ� �����ߴٸ�
        if (t == nullptr)
        {
            // �⺻ �ؽ�ó�� �ε�
            t = renderer->GetTexture(texName);

            // �⺻ �ؽ�ó�� �ε��� �����ϸ�
            if (t == nullptr)
            {
                t = renderer->GetTexture("Assets/Default.png");
            }
        }
        mTextures.emplace_back(t);
    }


    // ���ؽ� �ε�
    const rapidjson::Value& vertsJson = doc["vertices"];
    if (!vertsJson.IsArray() || vertsJson.Size() < 1)
    {
        SDL_Log("Mesh %s has no vertices", fileName.c_str());
        return false;
    }

    std::vector<Vertex> vertices;
    vertices.reserve(vertsJson.Size() * vertSize);
    mRadius = 0.f;
    for (rapidjson::SizeType i = 0; i < vertsJson.Size(); i++)
    {
        // ������ ���ؽ� ���̾ƿ��� 8���� float �ΰɷ� ����
        const rapidjson::Value& vert = vertsJson[i];
        if (!vert.IsArray())
        {
            SDL_Log("Unexpected vertex format for %s", fileName.c_str());
            return false;
        }

        Vector3 pos(vert[0].GetDouble(), vert[1].GetDouble(), vert[2].GetDouble());
        mRadius = Math::Max(mRadius, pos.SquareLength());
        mBox.UpdateMinMax(pos);

        if (layout == VertexArray::PosNormTex)
        {
            Vertex v;
            for (rapidjson::SizeType j = 0; j < vert.Size(); j++)
            {
                v.f = static_cast<float>(vert[j].GetDouble());
                vertices.emplace_back(v);
            }
        }
        else
        {
            Vertex v;
            for (rapidjson::SizeType j = 0; j < 6; j++)
            {
                v.f = static_cast<float>(vert[j].GetDouble());
                vertices.emplace_back(v);
            }

            for (rapidjson::SizeType j = 6; j < 14; j += 4)
            {
                v.b[0] = vert[j].GetUint();
                v.b[1] = vert[j + 1].GetUint();
                v.b[2] = vert[j + 2].GetUint();
                v.b[3] = vert[j + 3].GetUint();
                vertices.emplace_back(v);
            }

            for (rapidjson::SizeType j = 14; j < vert.Size(); j++)
            {
                v.f = vert[j].GetDouble();
                vertices.emplace_back(v);
            }
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
        vertices.data(), static_cast<unsigned>(vertices.size()) / vertSize,
        layout, indices.data(), static_cast<unsigned>(indices.size())
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

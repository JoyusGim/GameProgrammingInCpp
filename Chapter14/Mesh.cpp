#include "Mesh.h"
#include "Game.h"
#include "Renderer.h"
#include "Math.h"
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

const int BinaryVersion = 1;

struct MeshBinHeader
{
    char mSignature[4] = { 'G', 'M', 'S', 'H' };
    uint32_t mVersion = BinaryVersion;
    VertexArray::Layout mLayout = VertexArray::PosNormTex;

    uint32_t mNumTextures = 0;
    uint32_t mNumVerts = 0;
    uint32_t mNumIndices = 0;
    AABB mBox{ Vector3::Zero, Vector3::Zero };
    float mRadius = 0.f;
    float mSpecPower = 100.f;
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
    mFileName = fileName;
    if (LoadBinary(fileName + ".bin", renderer))
    {
        return true;
    }

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

    // gpmesh 파일의 버전 확인
    if (ver != 1)
    {
        SDL_Log("Mesh %s not version 1", fileName.c_str());
        return false;
    }

    mShaderName = doc["shader"].GetString();

    // 버텍스 레이아웃 정보는 이후 챕터에서 다룬다.
    // 현재는 하드코딩
    VertexArray::Layout layout = VertexArray::PosNormTex;
    size_t vertSize = 8;

    std::string vertexFormat = doc["vertexformat"].GetString();
    if (vertexFormat == "PosNormSkinTex")
    {
        layout = VertexArray::PosNormSkinTex;
        // This is the number of "Vertex" unions, which is 8 + 2 (for skinning)s
        vertSize = 10;
    }

    // 매쉬의 텍스처 목록 로딩
    const rapidjson::Value& textures = doc["textures"];
    if (!textures.IsArray() || textures.Size() < 1)
    {
        SDL_Log("Mesh %s has no textures, there should be at least one", fileName.c_str());
        return false;
    }

    // specPower 로딩
    mSpecPower = static_cast<float>(doc["specularPower"].GetDouble());

    // 텍스처 로딩
    std::vector<std::string> textureNames;
    for (rapidjson::SizeType i = 0; i < textures.Size(); i++)
    {
        std::string texName = textures[i].GetString();
        textureNames.emplace_back(texName);
        Texture* t = renderer->GetTexture(texName);
        // 매쉬에 등록된 텍스처를 로딩하는데 실패했다면
        if (t == nullptr)
        {
            // 기본 텍스처를 로딩
            t = renderer->GetTexture(texName);

            // 기본 텍스처도 로딩에 실패하면
            if (t == nullptr)
            {
                t = renderer->GetTexture("Assets/Default.png");
            }
        }
        mTextures.emplace_back(t);
    }


    // 버텍스 로딩
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
        // 지금은 버텍스 레이아웃은 8개의 float 인걸로 가정
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

    // 길이의 제곱값을 비교하여 mRadius에 저장했으므로
    // 최종 mRadius는 제곱근 연산을 해야함.
    mRadius = Math::Sqrt(mRadius);

    // 인덱스 로딩
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

    // 버텍스 어레이 생성
    unsigned int numVerts = static_cast<unsigned>(vertices.size()) / vertSize;
    mVertexArray = new VertexArray(vertices.data(), numVerts,
        layout, indices.data(), static_cast<unsigned>(indices.size())
    );

    SaveBinary(fileName + ".bin", vertices.data(),
        numVerts, layout, indices.data(),
        static_cast<unsigned>(indices.size()),
        textureNames, mBox, mRadius, mSpecPower
    );

    return true;
}

void Mesh::Unload()
{
    delete mVertexArray;
    mVertexArray = nullptr;
}

void Mesh::SaveBinary(const std::string& fileName, const void* verts, const uint32_t numVerts, VertexArray::Layout layout, const uint32_t* indices, uint32_t numIndices, const std::vector<std::string>& textureNames, const AABB& box, float radius, float specPower)
{
    MeshBinHeader header;
    header.mLayout = layout;
    header.mNumTextures = static_cast<unsigned>(textureNames.size());
    header.mNumVerts = numVerts;
    header.mNumIndices = numIndices;
    header.mBox = box;
    header.mRadius = radius;

    std::ofstream outFile(fileName, std::ios::out | std::ios::binary);
    if (outFile.is_open())
    {
        outFile.write(reinterpret_cast<char*>(&header), sizeof(header));

        for (const auto& tex : textureNames)
        {
            uint16_t nameSize = static_cast<uint16_t>(tex.length() + 1);
            outFile.write(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
            outFile.write(tex.c_str(), nameSize - 1);
            outFile.write("\0", 1);
        }

        unsigned vertexSize = VertexArray::GetVertexSize(layout);
        outFile.write(reinterpret_cast<const char*>(verts), numVerts * vertexSize);
        outFile.write(reinterpret_cast<const char*>(indices), numIndices * sizeof(uint32_t));

    }
}

bool Mesh::LoadBinary(const std::string& fileName, Renderer* renderer)
{
    std::ifstream inFile(fileName, std::ios::in | std::ios::binary);
    if (inFile.is_open())
    {
        MeshBinHeader header;
        inFile.read(reinterpret_cast<char*>(&header), sizeof(header));

        char* sig = header.mSignature;
        if (sig[0] != 'G' || sig[1] != 'M' || sig[2] != 'S' || sig[3] != 'H' ||
            header.mVersion != BinaryVersion)
        {
            return false;
        }

        for (uint32_t i = 0; i < header.mNumTextures; i++)
        {
            uint16_t nameSize = 0;
            inFile.read(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));

            char* texName = new char[nameSize];
            inFile.read(texName, nameSize);

            Texture* t = renderer->GetTexture(texName);
            if (t == nullptr)
            {
                t = renderer->GetTexture("Assets/Default.png");
            }
            mTextures.emplace_back(t);

            delete[] texName;
        }

        unsigned vertexSize = VertexArray::GetVertexSize(header.mLayout);
        char* verts = new char[header.mNumVerts * vertexSize];
        uint32_t* indices = new uint32_t[header.mNumIndices];
        inFile.read(verts, header.mNumVerts * vertexSize);
        inFile.read(reinterpret_cast<char*>(indices), header.mNumIndices * sizeof(uint32_t));

        mVertexArray = new VertexArray(verts, header.mNumVerts, header.mLayout, indices, header.mNumIndices);

        delete[] verts;
        delete[] indices;

        mBox = header.mBox;
        mRadius = header.mRadius;
        mSpecPower = header.mSpecPower;

        return true;
    }
    else
    {
        SDL_Log("Failed to load binary mesh file %s\n", fileName.c_str());
        return false;
    }
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

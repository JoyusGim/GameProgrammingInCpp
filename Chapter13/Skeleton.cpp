#include "Skeleton.h"
#include "MatrixPalette.h"
#include <fstream>
#include <sstream>
#include <rapidjson\document.h>
#include <SDL.h>

void Skeleton::ComputeGlobalInvBindPose()
{
    mGlobalInvBindPoses.resize(GetNumBones());

    // 루트 본의 글로벌 바인드 포즈 행렬은 로컬 바인드 포즈 행렬과 같다.
    mGlobalInvBindPoses[0] = mBones[0].mLocalBindPose.ToMatrix();

    // 각 본의 글로벌 바인드 포즈 행렬은 자신의 로컬 바인드 포즈 행렬과
    // 부모의 글로벌 바인드 포즈 행렬의 곱이다.
    for (size_t i = 1; i < mGlobalInvBindPoses.size(); i++)
    {
        Matrix4 localMat = mBones[i].mLocalBindPose.ToMatrix();
        mGlobalInvBindPoses[i] = localMat * mGlobalInvBindPoses[mBones[i].mParent];
    }

    // 마지막으로 모든 글로벌 바인드 포즈 행렬을 역행렬을 취한다. 
    for (size_t i = 0; i < mGlobalInvBindPoses.size(); i++)
    {
        mGlobalInvBindPoses[i].Invert();
    }
}

bool Skeleton::Load(const std::string& fileName)
{
    std::ifstream ifs(fileName);
    if (!ifs.is_open())
    {
        SDL_Log("File not found: Skeleton %s", fileName.c_str());
        return false;
    }

    std::stringstream fileStream;
    fileStream << ifs.rdbuf();
    std::string contents = fileStream.str();
    rapidjson::StringStream jsonStr(contents.c_str());
    rapidjson::Document doc;
    doc.ParseStream(jsonStr);

    if (!doc.IsObject())
    {
        SDL_Log("Skeleton %s is not valid json", fileName.c_str());
        return false;
    }

    int ver = doc["version"].GetInt();

    if (ver != 1)
    {
        SDL_Log("Skeleton %s unknown format", fileName.c_str());
        return false;
    }

    const rapidjson::Value& bonecount = doc["bonecount"];
    if (!bonecount.IsUint())
    {
        SDL_Log("Skeleton %s doesn't have a bone count.", fileName.c_str());
        return false;
    }

    size_t count = bonecount.GetUint();
    if (count > MAX_SKELETON_BONES)
    {
        SDL_Log("Skeleton %s exceeds maximum bone count.", fileName.c_str());
        return false;
    }

    mBones.reserve(count);
    const rapidjson::Value& bones = doc["bones"];
    if (!bones.IsArray())
    {
        SDL_Log("Skelton %s doesn't have a bone array?", fileName.c_str());
        return false;
    }

    if (bones.Size() != count)
    {
        SDL_Log("Skeleton %s has a mismatch between the bone count and number of bones", fileName.c_str());
        return false;
    }

    Bone temp;

    for (rapidjson::SizeType i = 0; i < count; i++)
    {
        if (!bones[i].IsObject())
        {
            SDL_Log("Skeleton %s: Bone %d is invalid.", fileName.c_str(), i);
            return false;
        }

        const rapidjson::Value& name = bones[i]["name"];
        temp.mName = name.GetString();

        const rapidjson::Value& parent = bones[i]["parent"];
        temp.mParent = parent.GetInt();

        const rapidjson::Value& bindpose = bones[i]["bindpose"];
        if (!bindpose.IsObject())
        {
            SDL_Log("Skeleton %s: Bone %d is invalid.", fileName.c_str(), i);
            return false;
        }

        const rapidjson::Value& rot = bindpose["rot"];
        const rapidjson::Value& trans = bindpose["trans"];

        if (!rot.IsArray() || !trans.IsArray())
        {
            SDL_Log("Skeleton %s: Bone %d is invalid.", fileName.c_str(), i);
            return false;
        }

        temp.mLocalBindPose.mRotation.x = rot[0].GetDouble();
        temp.mLocalBindPose.mRotation.y = rot[1].GetDouble();
        temp.mLocalBindPose.mRotation.z = rot[2].GetDouble();
        temp.mLocalBindPose.mRotation.w = rot[3].GetDouble();

        temp.mLocalBindPose.mTranslation.x = trans[0].GetDouble();
        temp.mLocalBindPose.mTranslation.y = trans[1].GetDouble();
        temp.mLocalBindPose.mTranslation.z = trans[2].GetDouble();

        mBones.emplace_back(temp);
    }

    ComputeGlobalInvBindPose();

    return true;
}

size_t Skeleton::GetNumBones() const
{
    return mBones.size();
}

const Skeleton::Bone& Skeleton::GetBone(size_t idx)
{
    return mBones[idx];
}

const std::vector<Skeleton::Bone>& Skeleton::GetBones() const
{
    return mBones;
}

const std::vector<Matrix4> Skeleton::GetGlobalInvBindPoses() const
{
    return mGlobalInvBindPoses;
}

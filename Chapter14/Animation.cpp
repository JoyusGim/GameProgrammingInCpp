#include "Animation.h"
#include "Skeleton.h"
#include <fstream>
#include <sstream>
#include <rapidjson\document.h>
#include <SDL_log.h>

bool Animation::Load(const std::string& fileName)
{
    std::ifstream ifs(fileName);
    if (!ifs.is_open())
    {
        SDL_Log("File not found: Animation %s", fileName.c_str());
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
        SDL_Log("Animation %s is not valid json", fileName.c_str());
        return false;
    }

    int ver = doc["version"].GetInt();
    if (ver != 1)
    {
        SDL_Log("Animation %s unknown format", fileName.c_str());
        return false;
    }

    const rapidjson::Value& sequence = doc["sequence"];
    if (!sequence.IsObject())
    {
        SDL_Log("Animation %s doesn't have a sequence", fileName.c_str());
        return false;
    }

    const rapidjson::Value& frames = sequence["frames"];
    const rapidjson::Value& length = sequence["length"];
    const rapidjson::Value& bonecount = sequence["bonecount"];
    if (!frames.IsUint() || !length.IsDouble() || !bonecount.IsUint())
    {
        SDL_Log("Sequence %s has invalid frames, length, or bone count", fileName.c_str());
        return false;
    }

    mNumFrames = frames.GetUint();
    mDuration = length.GetDouble();
    mNumBones = bonecount.GetUint();
    mFrameDuration = mDuration / (mNumFrames - 1);

    mTracks.resize(mNumBones);

    const rapidjson::Value& tracks = sequence["tracks"];
    if (!tracks.IsArray())
    {
        SDL_Log("Sequence %s missing a tracks array", fileName.c_str());
        return false;
    }

    for (rapidjson::SizeType i = 0; i < tracks.Size(); i++)
    {
        if (!tracks[i].IsObject())
        {
            SDL_Log("Animation %s: Track element %d is invalid", fileName.c_str(), i);
            return false;
        }

        size_t boneIndex = tracks[i]["bone"].GetUint();

        const rapidjson::Value& transforms = tracks[i]["transforms"];
        if (!transforms.IsArray())
        {
            SDL_Log("Animtation %s: Track element %d is missing transforms", fileName.c_str(), i);
            return false;
        }

        BoneTransform temp;
        if (transforms.Size() < mNumFrames)
        {
            SDL_Log("Animation %s: Track element %d has fewer frames than expected", fileName.c_str(), i);
            return false;
        }

        for (rapidjson::SizeType j = 0; j < transforms.Size(); j++)
        {
            const rapidjson::Value& rot = transforms[j]["rot"];
            const rapidjson::Value& trans = transforms[j]["trans"];
            if (!rot.IsArray() || !trans.IsArray())
            {
                SDL_Log("Skeleton %s: Bone %d is invalid", fileName.c_str(), i);
                return false;
            }

            temp.mRotation.x = rot[0].GetDouble();
            temp.mRotation.y = rot[1].GetDouble();
            temp.mRotation.z = rot[2].GetDouble();
            temp.mRotation.w = rot[3].GetDouble();

            temp.mTranslation.x = trans[0].GetDouble();
            temp.mTranslation.y = trans[1].GetDouble();
            temp.mTranslation.z = trans[2].GetDouble();

            mTracks[boneIndex].emplace_back(temp);
        }
    }
    mFileName = fileName;
    return true;
}

void Animation::GetGlobalPoseAtTime(std::vector<Matrix4>& outPoses, const Skeleton* inSkeleton, float inTime) const
{
    if (outPoses.size() != mNumBones)
    {
        outPoses.resize(mNumBones);
    }

    size_t frame = static_cast<size_t>(inTime / mFrameDuration);
    size_t nextFrame = frame + 1;
    float pct = inTime / mFrameDuration - frame;
    if (mTracks[0].size() > 0)
    {
        BoneTransform interp = BoneTransform::Interpolation(mTracks[0][frame],
            mTracks[0][nextFrame], pct);
        outPoses[0] = interp.ToMatrix();
    }
    else
    {
        outPoses[0] = Matrix4::Identity;
    }

    const std::vector<Skeleton::Bone>& bones = inSkeleton->GetBones();
    for (size_t bone = 1; bone < mNumBones; bone++)
    {
        Matrix4 localMat;
        if (mTracks[bone].size() > 0)
        {
            BoneTransform interp = BoneTransform::Interpolation(mTracks[bone][frame],
                mTracks[bone][nextFrame], pct);
            localMat = interp.ToMatrix();
        }

        outPoses[bone] = localMat * outPoses[bones[bone].mParent];
    }
}

void Animation::GetBlendedGlobalPoseAtTime(std::vector<Matrix4>& outPoses, const Skeleton* inSkeleton, const Animation* inA, float inTimeA, const Animation* inB, float inTimeB, float inBlendTime)
{
    size_t numBones = inSkeleton->GetNumBones();
    if (outPoses.size() != numBones)
    {
        outPoses.resize(numBones);
    }

    size_t frameA = static_cast<size_t>(inTimeA / inA->mFrameDuration);
    size_t nextFrameA = frameA + 1;
    size_t frameB = static_cast<size_t>(inTimeB / inB->mFrameDuration);
    size_t nextFrameB = frameB + 1;
    float pctA = inTimeA / inA->mFrameDuration - frameA;
    float pctB = inTimeB / inB->mFrameDuration - frameB;

    BoneTransform interpA, interpB, interp;
    if (inA->mTracks[0].size() > 0 && inB->mTracks[0].size() > 0)
    {
        interpA = BoneTransform::Interpolation(inA->mTracks[0][frameA],
            inA->mTracks[0][nextFrameA], pctA);
        interpB = BoneTransform::Interpolation(inB->mTracks[0][frameB],
            inB->mTracks[0][nextFrameB], pctB);
        interp = BoneTransform::Interpolation(interpA, interpB, inBlendTime);
        outPoses[0] = interp.ToMatrix();
    }
    else if(inA->mTracks[0].size() > 0)
    {
        interpA = BoneTransform::Interpolation(inA->mTracks[0][frameA],
            inA->mTracks[0][nextFrameA], pctA);
        outPoses[0] = interpA.ToMatrix();
    }
    else if (inB->mTracks[0].size() > 0)
    {
        interpB = BoneTransform::Interpolation(inB->mTracks[0][frameB],
            inB->mTracks[0][nextFrameB], pctB);
        outPoses[0] = interpB.ToMatrix();
    }
    else
    {
        outPoses[0] = Matrix4::Identity;
    }

    const std::vector<Skeleton::Bone>& bones = inSkeleton->GetBones();
    for (size_t bone = 1; bone < inSkeleton->GetNumBones(); bone++)
    {

        Matrix4 localMat;
        if (inA->mTracks[bone].size() > 0 && inB->mTracks[bone].size() > 0)
        {
            BoneTransform interpA = BoneTransform::Interpolation(inA->mTracks[bone][frameA],
                inA->mTracks[bone][nextFrameA], pctA);
            BoneTransform interpB = BoneTransform::Interpolation(inB->mTracks[bone][frameB],
                inB->mTracks[bone][nextFrameB], pctB);
            localMat = BoneTransform::Interpolation(interpA, interpB, inBlendTime).ToMatrix();
        }
        else if (inA->mTracks[bone].size() > 0)
        {
            localMat = BoneTransform::Interpolation(inA->mTracks[bone][frameA],
                inA->mTracks[bone][nextFrameA], pctA).ToMatrix();
        }
        else if (inB->mTracks[bone].size() > 0)
        {
            localMat = BoneTransform::Interpolation(inB->mTracks[bone][frameB],
                inB->mTracks[bone][nextFrameB], pctB).ToMatrix();
        }

        outPoses[bone] = localMat * outPoses[bones[bone].mParent];
    }
}


size_t Animation::GetNumBones() const
{
    return mNumBones;
}

size_t Animation::GetNumFrames() const
{
    return mNumFrames;
}

float Animation::GetDuration() const
{
    return mDuration;
}

float Animation::GetFrameDuration() const
{
    return mFrameDuration;
}

const std::string& Animation::GetFileName() const
{
    return mFileName;
}

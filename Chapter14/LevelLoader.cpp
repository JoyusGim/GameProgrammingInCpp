#include "LevelLoader.h"
#include <fstream>
#include <SDL_log.h>
#include <vector>
#include <rapidjson\stringbuffer.h>
#include <rapidjson\prettywriter.h>
#include "Game.h"
#include "Renderer.h"
#include "Actor.h"
#include "BallActor.h"
#include "FollowActor.h"
#include "PlaneActor.h"
#include "TargetActor.h"
#include "AudioComponent.h"
#include "BallMove.h"
#include "BoxComponent.h"
#include "CameraComponent.h"
#include "FollowCamera.h"
#include "MeshComponent.h"
#include "MoveComponent.h"
#include "SkeletalMeshComponent.h"
#include "SpriteComponent.h"
#include "MirrorCamera.h"
#include "PointLightComponent.h"
#include "TargetComponent.h"

const int LevelVersion = 1;

std::unordered_map<std::string, LevelLoader::ActorFunc> LevelLoader::sActorFactoryMap
{
    {"Actor", &Actor::Create<Actor>},
    {"BallActor", &Actor::Create<BallActor>},
    {"FollowActor", &Actor::Create<FollowActor>},
    {"PlaneActor", &Actor::Create<PlaneActor>},
    {"TargetActor", &Actor::Create<TargetActor>}
};

std::unordered_map<std::string, std::pair<int, LevelLoader::ComponentFunc>> LevelLoader::sComponentFactoryMap
{
        { "AudioComponent", { Component::AudioComponent, &Component::Create<AudioComponent>} },
    { "BallMove", { Component::BallMove, &Component::Create<BallMove> } },
    { "BoxComponent", { Component::BoxComponent, &Component::Create<BoxComponent> } },
    { "CameraComponent", { Component::CameraComponent, &Component::Create<CameraComponent> } },
    { "FollowCamera", { Component::FollowCamera, &Component::Create<FollowCamera> } },
    { "MeshComponent", { Component::MeshComponent, &Component::Create<MeshComponent> } },
    { "MoveComponent", { Component::MoveComponent, &Component::Create<MoveComponent> } },
    { "SkeletalMeshComponent", { Component::SkeletalMeshComponent, &Component::Create<SkeletalMeshComponent> } },
    { "SpriteComponent", { Component::SpriteComponent, &Component::Create<SpriteComponent> } },
    { "MirrorCamera", { Component::MirrorCamera, &Component::Create<MirrorCamera> } },
    { "PointLightComponent", { Component::PointLightComponent, &Component::Create<PointLightComponent> }},
    { "TargetComponent",{ Component::TargetComponent, &Component::Create<TargetComponent> } },
};

bool LevelLoader::LoadJSON(const std::string& fileName, rapidjson::Document& outDoc)
{
    std::ifstream inFile(fileName, std::ios::in | std::ios::binary | std::ios::ate);
    if (!inFile.is_open())
    {
        SDL_Log("File %s not found", fileName.c_str());
        return false;
    }

    std::ifstream::pos_type fileSize = inFile.tellg();
    inFile.seekg(0, std::ios::beg);

    std::vector<char> bytes(static_cast<size_t>(fileSize) + 1);
    inFile.read(bytes.data(), static_cast<size_t>(fileSize));

    outDoc.Parse(bytes.data());
    if (!outDoc.IsObject())
    {
        SDL_Log("File %s is not valid JSON", fileName.c_str());
        return false;
    }

    return true;
}

void LevelLoader::LoadGlobalProperties(Game* game, const rapidjson::Value& inObj)
{
    Vector3 ambient;
    if (JsonHelper::Get<Vector3>(inObj, "ambientLight", ambient))
    {
        game->GetRenderer()->SetAmbientLight(ambient);
    }

    const rapidjson::Value& dirObj = inObj["directionalLight"];
    if (dirObj.IsObject())
    {
        DirectionalLight& light = game->GetRenderer()->GetDirectionalLight();
        JsonHelper::Get<Vector3>(dirObj, "direction", light.mDirection);
        JsonHelper::Get<Vector3>(dirObj, "color", light.mDiffuseColor);        
    }
}

void LevelLoader::LoadActors(Game* game, const rapidjson::Value& inArray)
{
    for (rapidjson::SizeType i = 0; i < inArray.Size(); i++)
    {
        const rapidjson::Value& actorObj = inArray[i];
        if (actorObj.IsObject())
        {
            std::string type;
            if (JsonHelper::Get<std::string>(actorObj, "type", type))
            {
                auto iter = sActorFactoryMap.find(type);
                if (iter != sActorFactoryMap.end())
                {
                    Actor* actor = iter->second(game, actorObj["properties"]);
                    if (actorObj.HasMember("components"))
                    {
                        const rapidjson::Value& components = actorObj["components"];
                        if(components.IsArray())
                        {
                            LoadComponents(actor, components);
                        }
                    }
                }
                else
                {
                    SDL_Log("Unknown actor type %s", type.c_str());
                    return;
                }
            }
        }
    }
}

void LevelLoader::LoadComponents(Actor* actor, const rapidjson::Value& inArray)
{
    for (rapidjson::SizeType i = 0; i < inArray.Size(); i++)
    {
        const rapidjson::Value& compObj = inArray[i];
        if (compObj.IsObject())
        {
            std::string type;
            if (JsonHelper::Get<std::string>(compObj, "type", type))
            {
                auto iter = sComponentFactoryMap.find(type);
                if (iter != sComponentFactoryMap.end())
                {
                    Component::TypeID tid = static_cast<Component::TypeID>(iter->second.first);
                    Component* comp = actor->GetComponentOfType(tid);
                    if (comp == nullptr)
                    {
                        comp = iter->second.second(actor, compObj["properties"]);
                    }
                    else
                    {
                        comp->LoadProperties(compObj["properties"]);
                    }
                }
                else
                {
                    SDL_Log("Unknown component type %s", type.c_str());                    
                }
            }

        }
    }
}

void LevelLoader::SaveGlobalProperties(rapidjson::Document::AllocatorType& alloc, Game* game, rapidjson::Value& inObj)
{
    JsonHelper::Add<Vector3>(alloc, inObj, "ambientLight",
        game->GetRenderer()->GetAmbientLight());

    DirectionalLight& dirLight = game->GetRenderer()->GetDirectionalLight();
    rapidjson::Value dirObj(rapidjson::kObjectType);
    JsonHelper::Add<Vector3>(alloc, dirObj, "direction", dirLight.mDirection);
    JsonHelper::Add<Vector3>(alloc, dirObj, "color", dirLight.mDiffuseColor);
    inObj.AddMember("directionalLight", dirObj, alloc);
}

void LevelLoader::SaveActors(rapidjson::Document::AllocatorType& alloc, Game* game, rapidjson::Value& inArray)
{
    const auto& actors = game->GetActors();
    for (const Actor* actor : actors)
    {
        rapidjson::Value obj(rapidjson::kObjectType);
        JsonHelper::Add<std::string>(alloc, obj, "type", Actor::sTypeNames[actor->GetType()]);
        
        rapidjson::Value props(rapidjson::kObjectType);
        actor->SaveProperties(alloc, props);
        obj.AddMember("properties", props, alloc);

        rapidjson::Value components(rapidjson::kArrayType);
        SaveComponents(alloc, actor, components);
        obj.AddMember("components", components, alloc);

        inArray.PushBack(obj, alloc);
    }
}

void LevelLoader::SaveComponents(rapidjson::Document::AllocatorType& alloc, const Actor* actor, rapidjson::Value& inArray)
{
    const auto& components = actor->GetComponents();
    for (const Component* c : components)
    {
        rapidjson::Value obj(rapidjson::kObjectType);
        JsonHelper::Add<std::string>(alloc, obj, "type", Component::sTypeNames[c->GetType()]);

        rapidjson::Value props(rapidjson::kObjectType);
        c->SaveProperties(alloc, props);
        obj.AddMember("properties", props, alloc);

        inArray.PushBack(obj, alloc);
    }
}

bool LevelLoader::LoadLevel(Game* game, const std::string& fileName)
{
    rapidjson::Document doc;
    if (!LoadJSON(fileName, doc))
    {
        SDL_Log("Failed to load level %s", fileName.c_str());
        return false;
    }

    int version = 0;
    if (!JsonHelper::Get<int>(doc, "version", version) || version != LevelVersion)
    {
        SDL_Log("Incorrect level file version for %s", fileName.c_str());
        return false;
    }

    const rapidjson::Value& globals = doc["globalProperties"];
    if (globals.IsObject())
    {
        LoadGlobalProperties(game, globals);
    }

    const rapidjson::Value& actors = doc["actors"];
    if (actors.IsArray())
    {
        LoadActors(game, actors);
    }

    return true;
}

void LevelLoader::SaveLevel(Game* game, const std::string& fileName)
{
    rapidjson::Document doc;
    doc.SetObject();

    JsonHelper::Add<int>(doc.GetAllocator(), doc, "version", LevelVersion);

    rapidjson::Value globals(rapidjson::kObjectType);
    SaveGlobalProperties(doc.GetAllocator(), game, globals);
    doc.AddMember("globalProperties", globals, doc.GetAllocator());

    rapidjson::Value actors(rapidjson::kArrayType);
    SaveActors(doc.GetAllocator(), game, actors);
    doc.AddMember("actors", actors, doc.GetAllocator());

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);

    const char* output = buffer.GetString();

    std::ofstream ofs(fileName);
    if (ofs.is_open())
    {
        ofs << output;
    }
}

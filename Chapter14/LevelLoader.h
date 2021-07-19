#pragma once
#include <string>
#include <unordered_map>
#include <rapidjson\document.h>
#include <type_traits>
#include "Math.h"
#include "SDL_log.h"
#include <functional>

class LevelLoader
{	
	template <typename Result, typename... T>
	using Func = std::function<Result(T...)>;

	using ActorFunc = Func<class Actor*, class Game*, const rapidjson::Value&>;
	using ComponentFunc = Func<class Component*, class Actor*, const rapidjson::Value&>;
	
	static std::unordered_map<std::string, ActorFunc> sActorFactoryMap;
	static std::unordered_map<std::string, std::pair<int, ComponentFunc>> sComponentFactoryMap;

	static bool LoadJSON(const std::string& fileName, rapidjson::Document& outDoc);
	static void LoadGlobalProperties(class Game* game, const rapidjson::Value& inObj);
	static void LoadActors(class Game* game, const rapidjson::Value& inArray);
	static void LoadComponents(class Actor* actor, const rapidjson::Value& inArray);

	static void SaveGlobalProperties(rapidjson::Document::AllocatorType& alloc, class Game* game, rapidjson::Value& inObj);
	static void SaveActors(rapidjson::Document::AllocatorType& alloc, class Game* game, rapidjson::Value& inArray);
	static void SaveComponents(rapidjson::Document::AllocatorType& alloc, const class Actor* actor, rapidjson::Value& inArray);

public:
	static bool LoadLevel(class Game* game, const std::string& fileName);
	static void SaveLevel(class Game* game, const std::string& fileName);
};

class JsonHelper
{
public:
	template <typename T>
	static bool Get(const rapidjson::Value& inObj, const char* inProperty, T& outValue)
	{
		auto iter = inObj.FindMember(inProperty);
		if (iter == inObj.MemberEnd())
		{
			return false;
		}

		auto& property = iter->value;

		if constexpr (std::is_same_v<T, int>)
		{
			if (!property.IsInt()) return false;
			outValue = property.GetInt();
		}
		else if constexpr (std::is_same_v<T, float>)
		{
			if (!property.IsDouble()) return false;
			outValue = property.GetDouble();
		}
		else if constexpr (std::is_same_v<T, bool>)
		{
			if (!property.IsBool()) return false;
			outValue = property.GetBool();
		}
		else if constexpr (std::is_same_v<T, std::string>)
		{
			if (!property.IsString()) return false;
			outValue = property.GetString();
		}
		else if constexpr (std::is_same_v<T, Vector2>)
		{
			if (!property.IsArray() || property.Size() != 2) return false;
			for (rapidjson::SizeType i = 0; i < 2; i++)
			{
				if (!property[i].IsDouble()) return false;
			}
			outValue.x = property[0].GetDouble();
			outValue.y = property[1].GetDouble();
		}
		else if constexpr (std::is_same_v<T, Vector3>)
		{
			if (!property.IsArray() || property.Size() != 3) return false;
			for (rapidjson::SizeType i = 0; i < 3; i++)
			{
				if (!property[i].IsDouble()) return false;
			}
			outValue.x = property[0].GetDouble();
			outValue.y = property[1].GetDouble();
			outValue.z = property[2].GetDouble();
		}
		else if constexpr (std::is_same_v<T, Quaternion>)
		{
			if (!property.IsArray() || property.Size() != 4) return false;
			for (rapidjson::SizeType i = 0; i < 4; i++)
			{
				if (!property[i].IsDouble()) return false;
			}
			outValue.x = property[0].GetDouble();
			outValue.y = property[1].GetDouble();
			outValue.z = property[2].GetDouble();
			outValue.w = property[3].GetDouble();
		}
		else
		{
			SDL_Log("Unknown JSON property type %s", inProperty);
			return false;
		}

		return true;
	}

	template <typename T>
	static void Add(rapidjson::Document::AllocatorType& alloc, rapidjson::Value& inObj, const char* name, const T& value)
	{
		if constexpr (std::is_same_v<T, int> || std::is_same_v<T, float> || std::is_same_v<T, bool>)
		{
			rapidjson::Value v(value);
			inObj.AddMember(rapidjson::StringRef(name), v, alloc);
		}
		else if constexpr (std::is_same_v<T, std::string>)
		{
			rapidjson::Value v;
			v.SetString(value.c_str(), static_cast<rapidjson::SizeType>(value.length()), alloc);
			inObj.AddMember(rapidjson::StringRef(name), v, alloc);
		}
		else if constexpr (std::is_same_v<T, Vector2>)
		{
			rapidjson::Value v(rapidjson::kArrayType);
			v.PushBack(rapidjson::Value(value.x).Move(), alloc);
			v.PushBack(rapidjson::Value(value.y).Move(), alloc);
			inObj.AddMember(rapidjson::StringRef(name), v, alloc);
		}
		else if constexpr (std::is_same_v<T, Vector3>)
		{
			rapidjson::Value v(rapidjson::kArrayType);
			v.PushBack(rapidjson::Value(value.x).Move(), alloc);
			v.PushBack(rapidjson::Value(value.y).Move(), alloc);
			v.PushBack(rapidjson::Value(value.z).Move(), alloc);
			inObj.AddMember(rapidjson::StringRef(name), v, alloc);
		}
		else if constexpr (std::is_same_v<T, Quaternion>)
		{
			rapidjson::Value v(rapidjson::kArrayType);
			v.PushBack(rapidjson::Value(value.x).Move(), alloc);
			v.PushBack(rapidjson::Value(value.y).Move(), alloc);
			v.PushBack(rapidjson::Value(value.z).Move(), alloc);
			v.PushBack(rapidjson::Value(value.w).Move(), alloc);
			inObj.AddMember(rapidjson::StringRef(name), v, alloc);
		}
	}
};
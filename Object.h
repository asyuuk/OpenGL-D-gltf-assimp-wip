#pragma once
#include"ECS.h"
#include<list>
#include<memory>
#include<algorithm>
#include<iostream>
#include<unordered_map>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

class Component;

class Object
{
public:
	Object() {}
	~Object() = default;
	std::list<std::unique_ptr<Component>> components;
	Component* component = new Component;

	std::unordered_map<std::string, GLuint> textureMapID;
	void Update()
	{

		for (auto& component : components)
		{
			component->update();
		}
	}

	void Draw(const std::string& name)
	{

		for (auto& component : components)
		{
			auto it = textureMapID.find(name);
			if (it != textureMapID.end()) {
				GLuint textureID = it->second;
				component->Draw(textureID);
			}

			else {
				// Handle the case where the texture name is not found
				std::cerr << "Texture name not found: " << name << std::endl;

			}
		}

	}


	void Draw3D(float x, float y, float z,float size,float angle,glm::vec3 center,float cameraposx,float cameraposy,float cameraposz)
	{
		for (auto& component : components)
		{
			component->Draw3D(x, y, z,size,angle,center,cameraposx,cameraposy,cameraposz);
		}
	}
	void init()
	{
		for (auto& component : components)
		{
			component->init();
		}
	}
	bool collidercheck(Object* target)
	{
		for (auto& component : components)
		{
			if (component->collidercheck(target))
			{
				return true;
			}
		}
		return false;
	}

	template<typename T>
	T* GetComponent()
	{
		for (auto& component : components)
		{
			T* buff = dynamic_cast<T*>(component.get());
			if (buff != nullptr)
			{
				return buff;
			}
		}
		return nullptr;
	}
	template<typename T, typename... Args>
	T* AddComponent(Args&&...args)
	{
		auto component = std::make_unique<T>(std::forward<Args>(args)...);
		component->owner = this;
		T* rawPtr = component.get();
		component->init();
		components.push_back(std::move(component));
		return rawPtr;
	}

	template<typename T>
	T* DelComponent(Component* target)
	{
		auto it = std::find_if(components.begin(), components.end(),
			[&target](const std::unique_ptr<Component>& component) {
				return component.get() == target;
			});
		if (it != components.end())
		{
			T* buff = dynamic_cast<T*>((*it).get());
			components.erase(it);
			return buff;
		}
		else
		{
			return nullptr;
		}
	}

	template<typename T>
	T* RemoveComponent()
	{
		auto it = std::find_if(components.begin(), components.end(),
			[](const std::unique_ptr<Component>& component) {
				return dynamic_cast<T*>(component.get()) != nullptr;
			});
		if (it != components.end())
		{
			T* buff = dynamic_cast<T*>((*it).get());
			components.erase(it);
			return buff;
		}
		else
		{
			return nullptr;
		}

	}
};


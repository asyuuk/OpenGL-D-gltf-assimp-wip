#pragma once
#include<GL/glew.h>	
#include<GLFW/glfw3.h>
#include<string>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
class Object;

class Component
{
public:
	virtual ~Component() = default;
	virtual void update() {};
	virtual void AmmoUpdate() {};
	virtual void Delete() {};
	virtual void spawnupdate() {};
	virtual bool ColliderUpdate(int i) { return true; };
	virtual void Draw(GLuint textureID) {};
	virtual void UIDraw(std::string text, float x, float y, float scale) {};
	virtual void serifu() {};
	virtual bool init()
	{
		return true;
	};
	virtual void Draw3D(float x, float y, float z,float size,float angle,glm::vec3 center,float cameraposx, float cameraposy, float cameraposz) {};
	virtual void DrawComponents() {};
	virtual bool collidercheck(Object* targetObject) { return false; }
	Object* owner = nullptr;
	std::string name;
};


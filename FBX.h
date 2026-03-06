#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include"Renderer.h"
#include"Object.h"
#include<assimp/anim.h>
#include<assimp/camera.h>
#include<assimp/color4.h>
#include<assimp/mesh.h>
#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>
#include<glm/gtx/component_wise.hpp>

namespace Drawing
{
	class Renderer;
}


struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
};
class FBX:public Component
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int>indices;
	std::string Path = "E:/3D/girl OBJ.obj";
	std::string texPaths;
	GLuint vao, vbo, ebo,texture;
	int width, height, channel;	
	aiString texPath;
	std::string fullpath;
	Drawing::Renderer* shader = nullptr;
public:
	FBX(Object* Owner, Drawing::Renderer* renderer);
	~FBX()=default;
	bool LoadFBX();
	GLuint LoadImage(const std::string& filename);
	glm::vec3 center;

	void Draw3D(float x, float y, float z, float size, float angle,glm::vec3 center,float cameraposx, float cameraposy, float cameraposz)override;

};


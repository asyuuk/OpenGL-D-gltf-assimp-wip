#pragma once
#include"Renderer.h"
#include"Object.h"
#include<cgltf.h>

namespace Drawing
{
	class Renderer;
}

class Gltf3D : public Component
{
public:
	Gltf3D(Object* Owner, Drawing::Renderer* renderer);
	~Gltf3D() = default;
	bool initcGltf(const char* path);
	bool Load3Ddatas();
	bool LoadImages();
	void Draw3D(float x, float y, float z,float size,float angle, glm::vec3 center,float cameraposx, float cameraposy, float cameraposz) override;
	void update() override;
	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texcoord;
	};
	GLuint vao, vbo, ebo;
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	GLuint textureID;
private:
	cgltf_options options = {};
	cgltf_data* data = nullptr;

	//頂点抽出
	cgltf_accessor* positionAccessor = nullptr;
	cgltf_accessor* normalAccessor = nullptr;
	cgltf_accessor* texcoordAccessor = nullptr;
	Drawing::Renderer* shader = nullptr;
	std::string gltfPath = "E:/Blender/shrine_ao.glb";
	std::string baseDir;


};
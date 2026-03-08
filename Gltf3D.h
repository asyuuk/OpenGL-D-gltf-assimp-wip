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
	bool initcGltf();
	bool Load3Ddatas();
	bool LoadImages();
	bool LoadOneTexture(cgltf_texture* texture, const std::string& baseDir);
	void Draw3D(Gltf3D* gltf,float x, float y, float z,float size,float angle, glm::vec3 center,float cameraposx, float cameraposy, float cameraposz) override;
	void update() override;
	void BindMaterialTextures(cgltf_material* material,GLuint shader);
	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texcoord;
	};
	GLuint vao=0, vbo=0, ebo=0;
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<GLuint>texturesID;
	GLuint textureID=0;

	cgltf_options options = {};
	cgltf_data* data = nullptr;

	cgltf_texture* texture;
	

	std::unordered_map<cgltf_texture*, GLuint> textureIDMap;

	std::vector<GLuint>primitiveVAO;
	std::vector<GLuint>primitiveVBO;
	std::vector<GLuint>primitiveEBO;
	std::vector<GLsizei> primitiveIndexCount;
	std::vector<cgltf_material*>primitiveMaterial;

	cgltf_accessor* position = nullptr;
	cgltf_accessor* normal = nullptr;
	cgltf_accessor* UV = nullptr;

	//頂点抽出
	cgltf_accessor* positionAccessor = nullptr;
	cgltf_accessor* normalAccessor = nullptr;
	cgltf_accessor* texcoordAccessor = nullptr;
	Drawing::Renderer* shader = nullptr;
	std::string gltfPath = "E:/3D/Cyber_Surroundings_01.glb";
	std::string baseDir;


};
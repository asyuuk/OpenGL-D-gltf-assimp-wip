#define CGLTF_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS
#include"Gltf3D.h"
#include<stb_image.h>

Gltf3D::Gltf3D(Object* Owner, Drawing::Renderer* renderer)
{
	shader = renderer;

	if (!initcGltf(gltfPath.c_str()))
	{
		std::cout << "cgltf init error" << std::endl;
	}
	if (!Load3Ddatas())
	{
		std::cout << "Load3Ddatas error" << std::endl;
	}

	if (!LoadImages())
	{
		std::cout << "LoadImages error" << std::endl;
	}

}

void Gltf3D::update()
{

}

void Gltf3D::Draw3D(float x, float y, float z,float size,float angle, glm::vec3 center,float cameraposx, float cameraposy, float cameraposz)
{

	if (!shader->Draw3D(vao, indices.size(), textureID, x, y, z,size,angle,center,cameraposx,cameraposy,cameraposz))
	{
		std::cout << "Draw3D error" << std::endl;
	}
}

bool Gltf3D::initcGltf(const char* path)
{


	cgltf_result result = cgltf_parse_file(&options, path, &data);//ファイルから構造を読み取り methなど…

	if (result != cgltf_result_success)
	{
		std::cout << "error path or.." << std::endl;
		cgltf_free(data);
	}
	cgltf_load_buffers(&options, data, path);//バイナリーを読み込み

	return true;
}

bool Gltf3D::LoadImages()
{
	cgltf_primitive& primitive = data->meshes[0].primitives[0];
	if (!primitive.material)
		return false;

	cgltf_material& material = *primitive.material;
	if (!material.pbr_metallic_roughness.base_color_texture.texture)
		return false;

	cgltf_texture* texture = material.pbr_metallic_roughness.base_color_texture.texture;
	if(!texture)
	{ 
		std::cout << "No base color texture found" << std::endl;
		return false;
	}
	cgltf_texture& tex = *texture;
	if (!tex.image)
	{
		std::cout << "Texture has no image" << std::endl;
		return false;
	}

	cgltf_image& image = *tex.image;
	unsigned char* data = nullptr;
	int width, height, channels;
	//外部画像がある場合
	if (image.uri != NULL)
	{
		baseDir = gltfPath.substr(0, gltfPath.find_last_of("/\\") + 1);
		std::string imagePath = baseDir + image.uri;
		data = stbi_load(imagePath.c_str(), &width, &height, &channels, 0);
		if (!data)
		{
			std::cout << "Failed to load external texture:" << imagePath << std::endl;
			return false;
		}
	}
	//埋め込み画像
	else if (image.buffer_view != NULL)
	{
		cgltf_buffer_view* view = image.buffer_view;
		cgltf_buffer* buffer = view->buffer;
		const unsigned char* bufferStart = (const unsigned char*)buffer->data + view->offset;
		size_t bufferSize = view->size;
		data = stbi_load_from_memory(bufferStart, bufferSize, &width, &height, &channels, 0);

		if (!data)
		{
			std::cout << "Failed to load embedded texture from bufferView" << std::endl;
			return false;
		}
	}
	else
	{
		std::cout << "No image data found!" << std::endl;
		return false;
	}
	// OpenGL テクスチャ生成
	GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0,
		format, GL_UNSIGNED_BYTE, data);

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(data);
	return true;

}

bool Gltf3D::Load3Ddatas()
{
	cgltf_mesh& mesh = data->meshes[0];
	cgltf_primitive& primitive = mesh.primitives[0];
	for (cgltf_size i = 0; i < mesh.primitives[0].attributes_count; i++)
	{
		cgltf_attribute& attribute = primitive.attributes[i];
		if (attribute.type == cgltf_attribute_type_position)
		{
			positionAccessor = attribute.data;
		}
		else if (attribute.type == cgltf_attribute_type_normal)
		{
			normalAccessor = attribute.data;
		}
		else if (attribute.type == cgltf_attribute_type_texcoord)
		{
			texcoordAccessor = attribute.data;
		}
	}
	for (cgltf_size i = 0; i < primitive.attributes_count; i++)
	{
		std::cout << "attributes:" << primitive.attributes[i].name << std::endl;
	}

	for (cgltf_size i = 0; i < positionAccessor->count; i++)
	{
		Vertex vertex;
		cgltf_accessor_read_float(positionAccessor, i, &vertex.position.x, 3);
		cgltf_accessor_read_float(normalAccessor, i, &vertex.normal.x, 3);
		cgltf_accessor_read_float(texcoordAccessor, i, &vertex.texcoord.x, 2);
		vertices.push_back(vertex);
	}
	cgltf_accessor* indexAccessor = primitive.indices;
	for (cgltf_size i = 0; i < indexAccessor->count; ++i)
	{
		uint32_t index = 0;
		cgltf_accessor_read_uint(indexAccessor, i, &index, 1);
		indices.push_back(index);
	}

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));
	glEnableVertexAttribArray(2);

	return true;
}


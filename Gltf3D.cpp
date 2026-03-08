#define CGLTF_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS
#include"Gltf3D.h"
#include<stb_image.h>

Gltf3D::Gltf3D(Object* Owner, Drawing::Renderer* renderer)
{
	shader = renderer;



}

void Gltf3D::update()
{

}

void Gltf3D::Draw3D(Gltf3D* gltf,float x, float y, float z,float size,float angle, glm::vec3 center,float cameraposx, float cameraposy, float cameraposz)
{
	if (!shader->Draw3D( gltf,x, y, z,size,angle,center,cameraposx,cameraposy,cameraposz))
	{
		std::cout << "Draw3D error" << std::endl;
	}
}

void Gltf3D::BindMaterialTextures(cgltf_material* material,GLuint shader)
{
	int unit = 0;

	auto bind = [&](cgltf_texture_view& view, const char* uniformName) {
		if (view.texture)
		{
			glActiveTexture(GL_TEXTURE0 + unit);
			glBindTexture(GL_TEXTURE_2D, textureIDMap[view.texture]);
			glUniform1i(glGetUniformLocation(shader, uniformName), unit);
			unit++;
		}
		};

	bind(material->pbr_metallic_roughness.base_color_texture, "baseColorTex");
	bind(material->pbr_metallic_roughness.metallic_roughness_texture, "metallicRoughnessTex");
	bind(material->normal_texture, "normalTex");
	bind(material->occlusion_texture, "occlusionTex");
	bind(material->emissive_texture, "emissiveTex");
}


bool Gltf3D::initcGltf()
{


	cgltf_result result = cgltf_parse_file(&options, gltfPath.c_str(), &data);//ファイルから構造を読み取り methなど…

	if (result != cgltf_result_success)
	{
		std::cout << "error path or.." << std::endl;
		cgltf_free(data);
	}
	result=  cgltf_load_buffers(&options, data, gltfPath.c_str());//バイナリーを読み込み

	if (result != cgltf_result_success)
	{
		std::cout << "buffer load error" << std::endl;
		cgltf_free(data);
		return false;
	}


	result = cgltf_validate(data);
	if (result != cgltf_result_success)
	{
		std::cout << "gltf validate error" << std::endl;
	}
	return true;
}

bool Gltf3D::LoadImages()
{

	baseDir = gltfPath.substr(0, gltfPath.find_last_of("/\\") + 1);
	for (size_t m = 0; m < data->meshes_count; m++)
	{
		cgltf_mesh& mesh = data->meshes[m];
		for (size_t p = 0; p < data->meshes[m].primitives_count; p++)
		{
			cgltf_primitive& primitive = mesh.primitives[p];
			if (!primitive.material)
				continue;

			cgltf_material& material = *primitive.material;
			if (material.pbr_metallic_roughness.base_color_texture.texture)
			{
				LoadOneTexture(material.pbr_metallic_roughness.base_color_texture.texture,baseDir);
			}

			// metallicRoughnessTexture
			if (material.pbr_metallic_roughness.metallic_roughness_texture.texture)
			{
				LoadOneTexture(material.pbr_metallic_roughness.metallic_roughness_texture.texture,baseDir);
			}

			// normalTexture
			if (material.normal_texture.texture)
			{
				LoadOneTexture(material.normal_texture.texture,baseDir);
			}

			// occlusionTexture
			if (material.occlusion_texture.texture)
			{
				LoadOneTexture(material.occlusion_texture.texture,baseDir);
			}

			// emissiveTexture
			if (material.emissive_texture.texture)
			{
				LoadOneTexture(material.emissive_texture.texture,baseDir);
			}

			
		}
	}

	return true;

}

bool Gltf3D::LoadOneTexture(cgltf_texture* texture,const std::string& baseDir)
{
	if (!texture || !texture->image)
		return false;

	cgltf_image& image = *texture->image;
	unsigned char* data = nullptr;
	int width, height, channels;
	//外部画像がある場合
	if (image.uri != NULL)
	{
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

	textureIDMap[texture] = textureID;

	return true;
}

bool Gltf3D::Load3Ddatas()
{
	if (!data)
	{
		std::cout << "Error: gltf data null!" << std::endl;
		return false;
	}

	for (cgltf_size i = 0; i <data->meshes_count; i++)
	{
		cgltf_mesh& mesh = data->meshes[i];
		
		
		for (cgltf_size ja = 0; ja < mesh.primitives_count; ja++)
		{
			vertices.clear();
			indices.clear();

		
			cgltf_primitive& primitive = mesh.primitives[ja];

			position = nullptr;
			normal = nullptr;
			UV = nullptr;
		

			for (cgltf_size k = 0; k < primitive.attributes_count; k++)
			{
				cgltf_attribute& attribute = primitive.attributes[k];
				if (attribute.type == cgltf_attribute_type_position)
				{
					position = attribute.data;
				}

				if (attribute.type == cgltf_attribute_type_normal)
				{
					normal = attribute.data;
				}

				if (attribute.type == cgltf_attribute_type_texcoord)
				{
					UV = attribute.data;
				}
				std::cout << "attribute type: "
					<< attribute.type
					<< " index: "
					<< attribute.index
					<< std::endl;
			}
			if (!position)
			{
				std::cout << "Error missing position primitive" << std::endl;
				continue;
			}
			cgltf_size vertCount = position->count;

			bool hasnormal = (normal != nullptr);
			bool hasUV = (UV != nullptr);

			if (hasnormal)
				vertCount = std::min(vertCount, normal->count);
			if (hasUV)
				vertCount = std::min(vertCount, UV->count);
			//position
			for (cgltf_size i = 0; i < position->count; i++)
			{
				Vertex v;
				cgltf_accessor_read_float(position, i, &v.position.x, 3);
				if (hasnormal)
					cgltf_accessor_read_float(normal, i, &v.normal.x, 3);
				else
					v.normal = glm::vec3(0, 1, 0);
				if (hasUV)
					cgltf_accessor_read_float(UV, i, &v.texcoord.x, 2);
				else
					v.texcoord = glm::vec2(0, 0);

				vertices.push_back(v);
			}

			//index
			if (primitive.indices)
			{
				cgltf_accessor* indexAccessor = primitive.indices;
				for (cgltf_size i = 0; i < indexAccessor->count; i++)
				{
					uint32_t idx;
					cgltf_accessor_read_uint(indexAccessor, i, &idx, 1);
					indices.push_back(idx);
				}
			}
			

			GLuint vao, vbo, ebo;
			//vao vbo ebo
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
			primitiveVAO.push_back(vao);
			primitiveVBO.push_back(vbo);
			primitiveEBO.push_back(ebo);
			primitiveIndexCount.push_back(indices.size());
			primitiveMaterial.push_back(primitive.material);
			std::cout << "meshes: " << data->meshes_count << std::endl;
			std::cout << "buffers: " << data->buffers_count << std::endl;
		}
		
	
	}

	return true;
}


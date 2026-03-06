#include<stb_image.h>
#include "FBX.h"

FBX::FBX(Object* Owner, Drawing::Renderer* renderer)
{
	shader = renderer;
	if (!LoadFBX())
	{
		std::cout << "LoadFBX()Error" << std::endl;
	}
}

void FBX::Draw3D(float x, float y, float z, float size, float angle,glm::vec3 center, float cameraposx, float cameraposy, float cameraposz)
{
	if (!shader->Draw3D(vao, indices.size(), texture, x, y, z, size, angle,center,cameraposx,cameraposy,cameraposz))
	{
		std::cout << "Draw3D error" << std::endl;
		
	}
}

bool FBX::LoadFBX()
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(Path, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);
	if (!scene || !scene->mRootNode)
	{
		std::cerr << importer.GetErrorString() << std::endl;
		return false;
	}
	if (scene->mMeshes[0] == nullptr)
	{
		std::cout << "mMeshes[0]is nullptr" << std::endl;
	}
	aiMesh* mesh = scene->mMeshes[0];
	if (mesh == nullptr)
	{
		return false;
	}
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	if (material == nullptr)
	{
		return false;
	}
	std::string directory = Path.substr(0, Path.find_last_of("/\\") + 1);

	if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS)
	{
		texPaths = texPath.C_Str();
		fullpath = directory + texPaths;
		texture = LoadImage(fullpath);

		std::cout << "Diffuse texture:" << texPaths << std::endl;
			
	}

	vertices.clear();
	indices.clear();

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex v;
		v.position = {
			mesh->mVertices[i].x,mesh->mVertices[i].y,mesh->mVertices[i].z
		};
		v.normal =
		{
			mesh->mNormals[i].x,mesh->mNormals[i].y,mesh->mNormals[i].z
		};
		if (mesh->mTextureCoords[0])
		{
			v.uv =
			{
				mesh->mTextureCoords[0][i].x,mesh->mTextureCoords[0][i].y
			};
			
		}
		else
		{
			v.uv =
			{
				0.0f,0.0f
			};

		}
		vertices.push_back(v);

	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	glm::vec3 minP(FLT_MAX), maxP(-FLT_MAX);
	for (auto& v : vertices) {
		minP = glm::min(minP, v.position);
		maxP = glm::max(maxP, v.position);
	}


	center = (minP + maxP) * 0.5f;
	glm::vec3 size3 = maxP - minP;
	float scale = 1.0f / glm::compMax(size3);
	for (auto& v : vertices)
	{
		v.position = (v.position - center) * scale;
	}

	glGenVertexArrays(1, &vao);
	std::cout << "vao=" << vao << std::endl;
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,uv));

	glBindVertexArray(0);


	std::cout << "min: " << minP.x << "," << minP.y << "," << minP.z << std::endl;
	std::cout << "max: " << maxP.x << "," << maxP.y << "," << maxP.z << std::endl;


	return true;
}

GLuint FBX::LoadImage(const std::string& filename)
{
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channel, 4);
	if (!data)
	{
		std::cerr << "Failed to load texture:" << filename << std::endl;
		return 0;
	}
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(data);
	return tex;

}
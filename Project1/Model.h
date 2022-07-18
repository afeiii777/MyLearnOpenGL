#pragma once
#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>
#include <iostream>

#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"

using namespace std;
using namespace Assimp;

unsigned int TextureFromFile(const char* path, const string& directory)
{
	string filename = string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		cout << "Texture failed to load at path: " << path << endl;
		stbi_image_free(data);
	}

	return textureID;
}

class Model {

public:

	Model(string path) {
		LoadModel(path);
	}
	void Draw(Shader &shader) {
		for (int i = 0; i < meshes.size(); +i) meshes[i].Draw(shader);
	}

private:
	vector<texture> textures_loaded;
	vector<Mesh> meshes;
	string directory;
	void LoadModel(string path) {
		Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			cout << "ERROR:ASSIMP:" << importer.GetErrorString() << endl;
			return ;
		}
		directory = path.substr(0, path.find_last_of('/'));
		ProcessNode(scene->mRootNode, scene);
	}
	void ProcessNode(aiNode* node, const aiScene* scene) {
		for (int i = 0; i < node->mNumMeshes; ++i) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(ProcessMesh(mesh, scene));
		}
		for (int i = 0; i < node->mNumChildren; ++i) {
			ProcessNode(node->mChildren[i], scene);
		}
	}
	Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene) {
		vector<vertex> vertices;
		vector<unsigned int> indices;
		vector<texture> textures;

		for (int i = 0; i < mesh->mNumVertices; ++i) {
			vertex tempVertex;
			vec3 tempVec;
			tempVec.x = mesh->mVertices[i].x;
			tempVec.y = mesh->mVertices[i].y;
			tempVec.z = mesh->mVertices[i].z;
			tempVertex.pos = tempVec;
			tempVec.x = mesh->mNormals[i].x;
			tempVec.y = mesh->mNormals[i].y;
			tempVec.z = mesh->mNormals[i].z;
			tempVertex.normal = tempVec;
			if (mesh->mTextureCoords[0]) {
				vec2 vec;
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				tempVertex.texCoords = vec;
			}
			else tempVertex.texCoords = vec2(0.0f, 0.0f);
			vertices.push_back(tempVertex);
		}

		for (int i = 0; i < mesh->mNumFaces; ++i) {
			aiFace face = mesh->mFaces[i];
			for (int i = 0; i < face.mNumIndices; ++i) indices.push_back(face.mIndices[i]);
		}

		if (mesh->mMaterialIndex >= 0) {
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			vector<texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			vector<texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}

		return Mesh(vertices, indices, textures);
	}
	vector<texture> LoadMaterialTextures(aiMaterial *material, aiTextureType type, string typeName) {
		vector<texture> textures;

		for (int i = 0; i < material->GetTextureCount(type); ++i) {
			aiString str;
			
			material->GetTexture(type, i, &str);
			bool skip = false;
			for (int j = 0; j < textures_loaded.size(); j++)
			{
				if (textures_loaded[j].path == str)
				{
					textures.push_back(textures_loaded[j]);
					skip = true;
					break;
				}
			}
			if (!skip) {
				texture tempTexture;
				tempTexture.id = TextureFromFile(str.C_Str(), directory);
				tempTexture.type = typeName;
				tempTexture.path = str;
				textures.push_back(tempTexture);
				textures_loaded.push_back(tempTexture); 
			}
		}
		return textures;
	}
};

#endif // !MODEL_H


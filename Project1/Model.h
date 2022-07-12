#pragma once
#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"

using namespace std;
using namespace Assimp;

class Model {

public:

	Model(string path) {
		LoadModel(path);
	}
	void Draw(Shader &shader) {
		for (int i = 0; i < meshes.size(); +i) meshes[i].Draw(shader);
	}

private:
	
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
		}

		return Mesh(vertices, indices, textures);
	}
	vector<texture> LoadMaterialTextures(aiMaterial *material, aiTextureType type, string typeName) {

	}
};

#endif // !MODEL_H


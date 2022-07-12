#pragma once
#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>

#include <GL/glew.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "MyShader.h"

using namespace std;
using namespace glm;

#define MAX_BONE_INFLUENCE 4

struct vertex {
    vec3 pos;
    vec3 normal;
    vec2 texCoords;
    vec3 tangent;
    vec3 biTangent;
    int myBoneIds[MAX_BONE_INFLUENCE];
    float myWeights[MAX_BONE_INFLUENCE];
};

struct texture {
    unsigned int id;
    string type;
    string path;
};

class Mesh {
public:
    vector<vertex> vertices;
    vector<texture> textures;
    vector<unsigned int> indices;
    unsigned int VAO;
    Mesh(vector<vertex> vertices, vector<unsigned int> indices, vector<texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        setupMesh();
    }
    void Draw(Shader &shader) {
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;
        for (unsigned int i = 0; i < textures.size(); ++i) {

            glActiveTexture(GL_TEXTURE0 + i);

            string number;
            string name = textures[i].type;

            if (name == "texture_diffuse") number = to_string(diffuseNr++);
            else if (name == "texture_specular") number = to_string(specularNr++);
            else if (name == "texture_normal") number = to_string(normalNr++);
            else if (name == "texture_height") number = to_string(heightNr++);
            shader.setInt("Material." + name + number, i);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
    }
private:

    unsigned int VBO, EBO;

    void setupMesh()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), &vertices[0], GL_STREAM_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STREAM_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, normal));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, texCoords));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, tangent));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, biTangent));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_INT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, myBoneIds));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, myWeights));
        glBindVertexArray(0);
    }
};
#endif
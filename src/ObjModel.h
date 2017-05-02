//
// Created by Patrick Werner on 27.04.17.
//

#ifndef FAST_RAYTRACER_OBJMODEL_H
#define FAST_RAYTRACER_OBJMODEL_H

#include <iostream>
#include "Model.h"
#include <vector>
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "Mesh.h"
#include "Triangle.h"

class ObjModel : public Model {
public:
    ObjModel(const glm::vec3 origin, const Material material, std::string path) :
            Model{std::move(origin), std::move(material)},
            meshes{} {
        loadModel(path);
    }

    ObjModel(const glm::vec3 origin, const Material material, const std::vector<Mesh> meshes) :
            Model{std::move(origin), std::move(material)},
            meshes{std::move(meshes)} {
    }

    std::vector<Mesh> meshes;

private:
    void loadModel(std::string path) {
        Assimp::Importer import;
        const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate |
                                                     aiProcess_JoinIdenticalVertices |
                                                     aiProcess_OptimizeMeshes |
                                                     aiProcess_OptimizeGraph);

        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << "\n";
            return;
        }

        processNode(scene->mRootNode, scene);
    }

    void processNode(aiNode* node, const aiScene* scene) {
        for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(this->processMesh(mesh, scene));
        }

        for (unsigned int i = 0; i < node->mNumChildren; ++i) {
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
        std::vector<std::shared_ptr<Primitive>> primitives;
        for(unsigned int i = 0; i < mesh->mNumFaces; ++i) {
            std::array<glm::vec3, 3> vertices;
            auto face = mesh->mFaces[i];
            for(unsigned int j = 0; j < face.mNumIndices; ++j) {
                auto aiVertex = mesh->mVertices[face.mIndices[j]];
                auto vertex = glm::vec3(aiVertex.x, aiVertex.y, aiVertex.z);
                vertices[j] = vertex;
            }
            primitives.emplace_back(std::make_shared<Triangle>(vertices));
        }

        return Mesh(primitives);
    }
};

#endif //FAST_RAYTRACER_OBJMODEL_H

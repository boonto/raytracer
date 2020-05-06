//
// Created by Patrick Werner on 21.04.17.
//

#ifndef RAYTRACER_MODEL_H
#define RAYTRACER_MODEL_H

#include <utility>
#include <glm/vec3.hpp>
#include <tuple>
#include <memory>
#include "Material.h"
#include "Ray.h"
#include "Mesh.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "Triangle.h"

class Model {
public:
    Model(const glm::vec3 origin, const std::string& path) :
            Model{origin, std::vector<std::shared_ptr<Mesh>>{}} {
        loadModel(path, nullptr);
    }

    Model(const glm::vec3 origin, const std::string& path, const std::shared_ptr<Material>& material) :
            Model{origin, std::vector<std::shared_ptr<Mesh>>{}} {
        loadModel(path, material);
    }

    Model(const glm::vec3 origin, std::vector<std::shared_ptr<Mesh>>  meshes) :
            origin{origin},
            meshes{std::move(meshes)} {
    }

    //TODO translation with origin
    std::vector<std::shared_ptr<Primitive>> getPrimitives() {
        auto primitives = std::vector<std::shared_ptr<Primitive>>{};

        for (auto const &mesh : meshes) {
            auto meshPrimitives = mesh->getPrimitives();
            primitives.insert(primitives.end(),
                              std::make_move_iterator(meshPrimitives.begin()),
                              std::make_move_iterator(meshPrimitives.end()));
        }

        return primitives;
    }

private:
    glm::vec3 origin;
    std::vector<std::shared_ptr<Mesh>> meshes;

    void loadModel(const std::string& path, const std::shared_ptr<Material>& material) {
        Assimp::Importer import;
        const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate |
                                                     aiProcess_JoinIdenticalVertices |
                                                     aiProcess_OptimizeMeshes |
                                                     aiProcess_OptimizeGraph);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << "\n";
            return;
        }

        processAiNode(scene->mRootNode, scene, material);
    }

    void processAiNode(aiNode *node, const aiScene *scene, const std::shared_ptr<Material>& material) {
        for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(this->processAiMesh(mesh, scene, material));
        }

        for (unsigned int i = 0; i < node->mNumChildren; ++i) {
            processAiNode(node->mChildren[i], scene, material);
        }
    }

    static std::shared_ptr<Mesh> processAiMesh(aiMesh *mesh, const aiScene *scene, const std::shared_ptr<Material>& material) {
        std::vector<std::shared_ptr<Primitive>> primitives;
        for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
            std::array<glm::vec3, 3> vertices{};
            auto face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; ++j) {
                auto aiVertex = mesh->mVertices[face.mIndices[j]];
                auto vertex = glm::vec3(aiVertex.x, aiVertex.y, aiVertex.z);
                vertices[j] = vertex;
            }
            if (material) {
                primitives.push_back(std::make_shared<Triangle>(vertices, material));
            } else {
                primitives.push_back(std::make_shared<Triangle>(vertices));
            }
        }

        return std::make_shared<Mesh>(Mesh{primitives});
    }
};

#endif //RAYTRACER_MODEL_H

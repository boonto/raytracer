//
// Created by Patrick Werner on 21.04.17.
//

#ifndef FAST_RAYTRACER_SCENE_H
#define FAST_RAYTRACER_SCENE_H

#include <vector>
#include "Model.h"
#include "Camera.h"
#include "Plane.h"
#include "Sphere.h"
#include "PointLight.h"

class Scene {
public:
    Scene(const glm::ivec2 resolution) :
            camera{glm::vec3{-55.0f, 15.0f, 10.0f}, glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec3{0.0f, 0.0f, 0.0f}, std::move(resolution)},
            primitives{},
            lights{} {
        auto greenMaterial = Material{glm::vec3{0.5f, 1.0f, 0.5f},
                                      0.0f,
                                      1.0f,
                                      0.0f,
                                      0.0f,
                                      0.0f,
                                      100,
                                      1.2f
        };

//        auto reflectiveMaterial = Material{glm::vec3{1.0f, 0.0f, 0.0f},
//                                       0.0f,
//                                       0.0f,
//                                       0.0f,
//                                       1.0f,
//                                       0.0f,
//                                       90,
//                                       1.1f
//        };

//        auto redMaterial = Material{glm::vec3{1.0f, 0.5f, 0.5f},
//                                     0.0f,
//                                     1.0f,
//                                     0.0f,
//                                     0.0f,
//                                     0.0f,
//                                     100,
//                                     1.0f
//        };

        auto plane = Plane{glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f, 1.0f, 0.0f}, 100.0f};
        primitives.push_back(std::make_shared<Plane>(plane));

        auto sphere0 = Sphere{glm::vec3{6.0f, 6.0f, 8.0f}, 5.0f};
        primitives.push_back(std::make_shared<Sphere>(sphere0));

        auto sphere1 = Sphere{glm::vec3{0.0f, 6.0f, -8.0f}, 5.0f};
        primitives.push_back(std::make_shared<Sphere>(sphere1));

        auto sphere2 = Sphere{glm::vec3{-6.0f, 6.0f, 8.0f}, 5.0f};
        primitives.push_back(std::make_shared<Sphere>(sphere2));

        auto sphere3 = Sphere{glm::vec3{-12.0f, 6.0f, -8.0f}, 5.0f};
        primitives.push_back(std::make_shared<Sphere>(sphere3));

        auto sphere0Pointer = std::make_shared<Sphere>(sphere0);
        auto sphere0Vector = std::vector<std::shared_ptr<Primitive>>{sphere0Pointer};
        auto sphere0Mesh = Mesh{sphere0Vector};
        auto sphere0MeshPointer = std::make_shared<Mesh>(sphere0Mesh);
        auto sphere0MeshVector = std::vector<std::shared_ptr<Mesh>>{sphere0MeshPointer};
        auto SphereModel = Model{glm::vec3{0.0f}, sphere0MeshVector};

//        auto airplane = ObjModel{glm::vec3{0.0f, 2.0f, 0.0f}, redMaterial, std::string{"../cessna.obj"}};
//        for (auto triangle : airplane.meshes.front().primitives) {
//            primitives.push_back(triangle);
//        }

//        auto triangle0 = Triangle{glm::vec3{5.0f, 0.0f, 0.0f}, glm::vec3{5.0f, 5.0f, 5.0f}, glm::vec3{5.0f, 5.0f, -5.0f}};
//        primitives.push_back(std::make_shared<Triangle>(triangle0));

//        auto triangle1 = Triangle{glm::vec3{5.0f, 5.0f, 5.0f}, glm::vec3{10.0f, 7.0f, 0.0f}, glm::vec3{5.0f, 5.0f, -5.0f}};
//        primitives.push_back(std::make_shared<Triangle>(triangle1));

//        for (unsigned int i = 0; i < 5; ++i) {
//            for (unsigned int j = 0; j < 20; ++j) {
//                for (unsigned int k = 0; k < 5; ++k) {
//                    primitives.push_back(std::make_shared<Sphere>(Sphere{glm::vec3{-8.0f + i, 1.0f + j, -2.0f + k}, 0.4f}));
//                }
//            }
//        }

        lights.push_back(std::make_shared<PointLight>(PointLight{glm::vec3{-5.0f, 30.0f, 0.0f}, glm::vec3{1.0f}, 0.5f}));
        lights.push_back(std::make_shared<PointLight>(PointLight{glm::vec3{-10.0f, 20.0f, 0.0f}, glm::vec3{1.0f}, 0.5f}));
    }

    Camera camera;
    std::vector<std::shared_ptr<Primitive>> primitives;
    std::vector<std::shared_ptr<PointLight>> lights;
};

#endif //FAST_RAYTRACER_SCENE_H

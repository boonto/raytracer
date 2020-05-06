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
    explicit Scene(const glm::uvec2 resolution) :
            camera{glm::vec3{0.0f, 7.0f, 22.0f}, glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec3{0.0f, 6.0f, 0.0f},
//            camera{glm::vec3{230.0f, 60.0f, 450.0f}, glm::vec3{0.0f, 1.0f, 0.0f}, glm::vec3{50.0f, -50.0f, 0.0f},
                   resolution},
            primitives{},
            lights{} {
        auto whiteMaterial = std::make_shared<Material>(Material{glm::vec3{1.0f, 1.0f, 1.0f},
                                                                 0.0f,
                                                                 1.0f,
                                                                 0.0f,
                                                                 0.0f,
                                                                 0.0f,
                                                                 100,
                                                                 1.0f
        });

        auto greenMaterial = std::make_shared<Material>(Material{glm::vec3{0.5f, 1.0f, 0.5f},
                                                                 0.0f,
                                                                 1.0f,
                                                                 0.0f,
                                                                 0.0f,
                                                                 0.0f,
                                                                 100,
                                                                 1.0f
        });

        auto reflectiveMaterial = std::make_shared<Material>(Material{glm::vec3{1.0f, 1.0f, 1.0f},
                                                                      0.1f,
                                                                      0.0f,
                                                                      0.0f,
                                                                      1.0f,
                                                                      0.0f,
                                                                      100,
                                                                      1.0f
        });

        auto glassMaterial = std::make_shared<Material>(Material{glm::vec3{0.1f, 0.1f, 0.1f},
                                                                 0.0f,
                                                                 1.0f,
                                                                 0.0f,
                                                                 0.0f,
                                                                 1.0f,
                                                                 100,
                                                                 1.5f
        });

        auto redMaterial = std::make_shared<Material>(Material{glm::vec3{1.0f, 0.5f, 0.5f},
                                                               0.0f,
                                                               1.0f,
                                                               0.0f,
                                                               0.0f,
                                                               0.0f,
                                                               100,
                                                               1.0f
        });

        auto floor = Plane{glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f, 1.0f, 0.0f}, 100.0f, whiteMaterial};
        primitives.push_back(std::make_shared<Plane>(floor));
        auto leftWall = Plane{glm::vec3{-10.0f, 0.0f, 0.0f}, glm::vec3{1.0f, 0.0f, 0.0f}, 100.0f, redMaterial};
        primitives.push_back(std::make_shared<Plane>(leftWall));
        auto rightWall = Plane{glm::vec3{10.0f, 0.0f, 0.0f}, glm::vec3{-1.0f, 0.0f, 0.0f}, 100.0f, greenMaterial};
        primitives.push_back(std::make_shared<Plane>(rightWall));
        auto backWall = Plane{glm::vec3{0.0f, 0.0f, -10.0f}, glm::vec3{0.0f, 0.0f, 1.0f}, 100.0f, whiteMaterial};
        primitives.push_back(std::make_shared<Plane>(backWall));
        auto frontWall = Plane{glm::vec3{0.0f, 0.0f, 23.0f}, glm::vec3{0.0f, 0.0f, -1.0f}, 100.0f, whiteMaterial};
        primitives.push_back(std::make_shared<Plane>(backWall));
        auto ceiling = Plane{glm::vec3{0.0f, 14.0f, 0.0f}, glm::vec3{0.0f, -1.0f, 0.0f}, 100.0f, whiteMaterial};
        primitives.push_back(std::make_shared<Plane>(ceiling));

        auto sphere0 = Sphere{glm::vec3{-7.0f, 4.0f, -4.0f}, 3.0f, reflectiveMaterial};
        primitives.push_back(std::make_shared<Sphere>(sphere0));
        auto sphere1 = Sphere{glm::vec3{0.0f, 3.0f, -2.0f}, 2.0f, whiteMaterial};
        primitives.push_back(std::make_shared<Sphere>(sphere1));
        auto sphere2 = Sphere{glm::vec3{7.0f, 2.0f, 0.0f}, 1.0f, glassMaterial};
        primitives.push_back(std::make_shared<Sphere>(sphere2));

        for (int i = -3; i <= 3; ++i) {
            for (int j = -3; j <= 3; ++j) {
                lights.push_back(std::make_shared<PointLight>(
                        PointLight{glm::vec3{0.2f * (float) i, 13.99f, 0.2f * (float) j}, glm::vec3{1.0f}, 0.8f / 49}));
            }
        }

//        auto house = Model{glm::vec3{0.0f, 4.0f, 0.0f}, std::string{"../baltrams_warenhaus.glb"}, whiteMaterial};
//        for (auto triangle : house.getPrimitives()) {
//            primitives.push_back(triangle);
//        }
//
//        lights.push_back(std::make_shared<PointLight>(PointLight{glm::vec3{200.0f, 100.0f, 400.0f}, glm::vec3{1.0f}, 0.20f}));
//        lights.push_back(std::make_shared<PointLight>(PointLight{glm::vec3{210.0f, 110.0f, 410.0f}, glm::vec3{1.0f}, 0.20f}));
//        lights.push_back(std::make_shared<PointLight>(PointLight{glm::vec3{220.0f, 120.0f, 420.0f}, glm::vec3{1.0f}, 0.20f}));
//        lights.push_back(std::make_shared<PointLight>(PointLight{glm::vec3{230.0f, 130.0f, 430.0f}, glm::vec3{1.0f}, 0.20f}));
    }

    Camera camera;
    std::vector<std::shared_ptr<Primitive>> primitives;
    std::vector<std::shared_ptr<PointLight>> lights;
};

#endif //FAST_RAYTRACER_SCENE_H

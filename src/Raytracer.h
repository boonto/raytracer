//
// Created by Patrick Werner on 08.05.17.
//

#ifndef FAST_RAYTRACER_RAYTRACER_H
#define FAST_RAYTRACER_RAYTRACER_H

#include <glm/vec3.hpp>
#include <vector>
#include <stack>
#include "Ray.h"
#include "Primitive.h"
#include "KdTree.h"
#include "PointLight.h"
#include "Window.h"
#include "Camera.h"
#include "Scene.h"

const glm::vec3 BACKGROUND_COLOR(0.0f, 0.0f, 0.0f);
const int MAX_DEPTH = 100;
const bool USE_KDTREE = true;

class Raytracer {
public:
    explicit Raytracer(const std::shared_ptr<Scene>& scene) :
            scene{scene},
            kdTree{scene->primitives},
            counter{0} {
    }

    unsigned long long render(Window &window);

private:
    std::shared_ptr<Scene> scene;
    KdTree kdTree;
    unsigned long long counter;

    glm::vec3 raytrace(const Ray &ray, int depth);

    glm::vec3 shade(const Ray &ray, int depth, float t, const std::weak_ptr<Primitive> &prim);

    static glm::vec3 refract(const glm::vec3 &intersectionDirection, const glm::vec3 &intersectionNormal, const float &refrIndex);
    static float fresnel(const glm::vec3 &intersectionDirection, const glm::vec3 &intersectionNormal, const float &refrIndex);
};

#endif //FAST_RAYTRACER_RAYTRACER_H

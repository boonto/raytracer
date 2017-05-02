//
// Created by Patrick Werner on 21.04.17.
//

#ifndef RAYTRACER_POINTLIGHT_H
#define RAYTRACER_POINTLIGHT_H

#include <glm/vec3.hpp>
#include <memory>
#include "Primitive.h"

class PointLight {
public:
    glm::vec3 origin;
    glm::vec3 color;
    float intensity;
    std::weak_ptr<Primitive> blocker;

    PointLight(const glm::vec3 origin, const glm::vec3 color, const float intensity) :
            origin{std::move(origin)},
            color{std::move(color)},
            intensity{intensity},
            blocker{} {
    }
};

#endif //RAYTRACER_POINTLIGHT_H

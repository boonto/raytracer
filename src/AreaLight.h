//
// Created by Patrick Werner on 06.05.20.
//

#ifndef RAYTRACER_AREALIGHT_H
#define RAYTRACER_AREALIGHT_H

#include <glm/vec3.hpp>
#include <memory>
#include "Primitive.h"

class AreaLight {
public:
    glm::vec3 origin;
    glm::vec3 normal;
    float distance;
    glm::vec3 color;
    float intensity;
    std::weak_ptr<Primitive> blocker;

    AreaLight(const glm::vec3 origin, const glm::vec3 normal, const float distance, const glm::vec3 color,
              const float intensity) :
            origin{origin},
            normal{normal},
            distance{distance},
            color{color},
            intensity{intensity},
            blocker{} {
    }
};

#endif //RAYTRACER_AREALIGHT_H

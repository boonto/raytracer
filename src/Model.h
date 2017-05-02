//
// Created by Patrick Werner on 21.04.17.
//

#ifndef RAYTRACER_MODEL_H
#define RAYTRACER_MODEL_H

#include <glm/vec3.hpp>
#include <tuple>
#include "Material.h"
#include "Ray.h"

class Model {
public:
    Model(const glm::vec3 origin, const Material material) :
            origin{std::move(origin)},
            material{std::move(material)} {
    }
private:
    glm::vec3 origin;
    Material material;
};

#endif //RAYTRACER_MODEL_H

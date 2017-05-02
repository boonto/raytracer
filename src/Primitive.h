//
// Created by Patrick Werner on 01.05.17.
//

#ifndef FAST_RAYTRACER_PRIMITIVE_H
#define FAST_RAYTRACER_PRIMITIVE_H

#include <tuple>
#include <glm/vec3.hpp>
#include "Ray.h"

class Primitive {
public:
    enum class Axis {
        x,
        y,
        z
    };

    //TODO
    virtual ~Primitive() {
    }

    //TODO einzelne funktion?
    virtual std::tuple<bool, float> intersect(const Ray &ray, const float dist) const = 0; //TODO: schönerer retval?

    virtual std::tuple<glm::vec3, glm::vec3> getIntersectionVectors(const Ray &ray, const float dist) const = 0;

    virtual std::tuple<glm::vec3, glm::vec3> getExtremes() const = 0;
};


#endif //FAST_RAYTRACER_PRIMITIVE_H

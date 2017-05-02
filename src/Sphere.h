//
// Created by patrick on 21.04.17.
//

#ifndef RAYTRACER_SPHERE_H
#define RAYTRACER_SPHERE_H

#include "Primitive.h"

class Sphere : public Primitive {
public:
    Sphere(const glm::vec3 origin, const float radius) :
            origin{std::move(origin)},
            radius{radius} {
    }

    std::tuple<bool, float> intersect(const Ray &ray, const float dist) const override;

    std::tuple<glm::vec3, glm::vec3> getIntersectionVectors(const Ray &ray, const float dist) const override;

    std::tuple<glm::vec3, glm::vec3> getExtremes() const override;

private:
    glm::vec3 origin;
    float radius;
};

#endif //RAYTRACER_SPHERE_H

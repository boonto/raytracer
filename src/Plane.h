//
// Created by Patrick Werner on 21.04.17.
//

#ifndef RAYTRACER_PLANE_H
#define RAYTRACER_PLANE_H

#include "Primitive.h"

class Plane : public Primitive {
public:
    Plane(const glm::vec3 origin, const glm::vec3 normal, const float distance) :
            Primitive{},
            origin{std::move(origin)},
            normal{std::move(normal)},
            distance(distance) {
    }

    std::tuple<bool, float> intersect(const Ray &ray, const float dist) const override;

    std::tuple<glm::vec3, glm::vec3> getIntersectionVectors(const Ray &ray, const float dist) const override;

    std::tuple<glm::vec3, glm::vec3> getExtremes() const override;

private:
    glm::vec3 origin;
    glm::vec3 normal;
    float distance;
};

#endif //RAYTRACER_PLANE_H

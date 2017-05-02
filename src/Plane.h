//
// Created by patrick on 21.04.17.
//

#ifndef RAYTRACER_PLANE_H
#define RAYTRACER_PLANE_H

#include "Primitive.h"

class Plane : public Primitive {
public:
    Plane(const glm::vec3 origin, const glm::vec3 normal) :
            origin{std::move(origin)},
            normal{std::move(normal)} {
    }

    std::tuple<bool, float> intersect(const Ray &ray, const float dist) const override;

    std::tuple<glm::vec3, glm::vec3> getIntersectionVectors(const Ray &ray, const float dist) const override;

    std::tuple<glm::vec3, glm::vec3> getExtremes() const override;

private:
    glm::vec3 origin;
    glm::vec3 normal;
};

#endif //RAYTRACER_PLANE_H

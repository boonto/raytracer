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
            origin{origin},
            normal{normal},
            distance(distance) {
    }

    Plane(const glm::vec3 origin, const glm::vec3 normal, const float distance,
          const std::shared_ptr<Material>& material) :
            Primitive{material},
            origin{origin},
            normal{normal},
            distance(distance) {
    }

    Intersection intersect(const Ray &ray, float dist) const override;

    IntersectionVectors getIntersectionVectors(const Ray &ray, float dist) const override;

    Extremes getExtremes(Axis axis) const override;

private:
    glm::vec3 origin;
    glm::vec3 normal;
    float distance;
};

#endif //RAYTRACER_PLANE_H

//
// Created by Patrick Werner on 21.04.17.
//

#ifndef RAYTRACER_SPHERE_H
#define RAYTRACER_SPHERE_H

#include "Primitive.h"

class Sphere : public Primitive {
public:
    Sphere(const glm::vec3 origin, const float radius,
           const std::shared_ptr<Material>& material = std::make_shared<Material>(Material{})) :
            Primitive{material},
            origin{origin},
            radius{radius} {
    }

    Intersection intersect(const Ray &ray, float dist) const override;

    IntersectionVectors getIntersectionVectors(const Ray &ray, float dist) const override;

    Extremes getExtremes(Axis axis) const override;

private:
    glm::vec3 origin;
    float radius;
};

#endif //RAYTRACER_SPHERE_H

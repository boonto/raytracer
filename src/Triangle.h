//
// Created by Patrick Werner on 27.04.17.
//

#ifndef FAST_RAYTRACER_TRIANGLE_H
#define FAST_RAYTRACER_TRIANGLE_H

#include <glm/glm.hpp>
#include <array>
#include "Primitive.h"

class Triangle : public Primitive {
public:
    Triangle(const glm::vec3 p0, const glm::vec3 p1, const glm::vec3 p2, const std::shared_ptr<Material> material = std::make_shared<Material>(Material{})) :
            Primitive{std::move(material)},
            vertices{std::move(p0), std::move(p1), std::move(p2)} {
    }

    Triangle(const std::array<glm::vec3, 3> vertices, const std::shared_ptr<Material> material = std::make_shared<Material>(Material{})) :
            Primitive{std::move(material)},
            vertices(std::move(vertices)) {
    }

    Intersection intersect(const Ray &ray, const float dist) const override;

    IntersectionVectors getIntersectionVectors(const Ray &ray, const float dist) const override;

    std::tuple<glm::vec3, glm::vec3> getExtremes() const override;

private:
    std::array<glm::vec3, 3> vertices;
};


#endif //FAST_RAYTRACER_TRIANGLE_H

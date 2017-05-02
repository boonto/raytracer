//
// Created by Patrick Werner on 21.04.17.
//

#include <glm/glm.hpp>
#include "Plane.h"

std::tuple<bool, float> Plane::intersect(const Ray &ray, const float dist) const {
    auto result = false;
    auto t = std::numeric_limits<float>::max();

    auto denom = glm::dot(normal, ray.getDirection());
    if(abs(denom) > std::numeric_limits<float>::epsilon()) {
        t  = glm::dot((origin - ray.getOrigin()), normal) / denom;
        if (t > 0.0f && t < dist) {
            result = true;
        }
    }

    return std::make_tuple(result, t);
}

std::tuple<glm::vec3, glm::vec3> Plane::getIntersectionVectors(const Ray &ray, const float dist) const {
    auto position = ray.getOrigin() + dist * ray.getDirection();

    return std::make_tuple(position, this->normal);
}

std::tuple<glm::vec3, glm::vec3> Plane::getExtremes() const {
    auto minimum = origin - (glm::vec3{1.0f} - normal) * distance;
    auto maximum = origin + (glm::vec3{1.0f} - normal) * distance;
    return std::make_tuple(minimum, maximum);
}

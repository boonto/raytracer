//
// Created by patrick on 21.04.17.
//

#include <limits>
#include <glm/geometric.hpp>
#include "Sphere.h"

std::tuple<bool, float> Sphere::intersect(const Ray &ray, const float dist) const {
    auto result = false;
    auto t = std::numeric_limits<float>::max();

    auto v = glm::vec3{ray.getOrigin() - origin};
    auto b = -glm::dot(v, ray.getDirection());
    auto det = (b * b) - glm::dot(v, v) + powf(radius, 2.0f);

    if(det > 0.0f) {
        det = sqrtf(det);
        auto t0 = b - det;
        auto t1 = b + det;

        if (t1 > 0.0f) {
            if (t0 < 0.0f) {
                if (t1 < dist) {
                    t = t1;
                    result = false; // inside
                }
            } else {
                if (t0 < dist) {
                    t = t0;
                    result = true;
                }
            }
        }
    }

    return std::make_tuple(result, t);
}

std::tuple<glm::vec3, glm::vec3> Sphere::getIntersectionVectors(const Ray &ray, const float dist) const {
    auto position = ray.getOrigin() + dist * ray.getDirection();
    auto normal = glm::normalize((position - origin) / radius);

    return std::make_tuple(position, normal);
}

std::tuple<glm::vec3, glm::vec3> Sphere::getExtremes() const {
    auto minimum = origin - radius;
    auto maximum = origin + radius;
    return std::make_tuple(minimum, maximum);
}
//
// Created by Patrick Werner on 21.04.17.
//

#include <glm/glm.hpp>
#include "Plane.h"

Primitive::Intersection Plane::intersect(const Ray &ray, const float dist) const {
    auto result = false;
    auto t = std::numeric_limits<float>::max();

    auto denom = glm::dot(normal, ray.getDirection());
    if (abs(denom) > std::numeric_limits<float>::epsilon()) {
        t = glm::dot((origin - ray.getOrigin()), normal) / denom;
        if (t > 0.0f && t < dist) {
            result = true;
        }
    }

    return Intersection{result, t};
}

Primitive::IntersectionVectors Plane::getIntersectionVectors(const Ray &ray, const float dist) const {
    auto position = ray.getOrigin() + dist * ray.getDirection();

    return IntersectionVectors{position, normal};
}

Primitive::Extremes Plane::getExtremes(Axis axis) const {
    switch (axis) {
        case Axis::x: {
            return Extremes{origin.x - (1 - normal.x) * distance, origin.x + (1 - normal.x) * distance};
        }
        case Axis::y: {
            return Extremes{origin.y - (1 - normal.y) * distance, origin.y + (1 - normal.y) * distance};
        }
        case Axis::z: {
            return Extremes{origin.z - (1 - normal.z) * distance, origin.z + (1 - normal.z) * distance};
        }
        default: {
            return Extremes{0.0f, 0.0f};
        }
    }
}

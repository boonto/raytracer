//
// Created by Patrick Werner on 08.05.17.
//

#include "Triangle.h"

Primitive::Intersection Triangle::intersect(const Ray &ray, const float dist) const {
    auto edge0 = vertices[1] - vertices[0];
    auto edge1 = vertices[2] - vertices[0];

    auto P = glm::cross(ray.getDirection(), edge1);

    auto det = glm::dot(edge0, P);
    if(abs(det) < std::numeric_limits<float>::epsilon())
        return Intersection{false, 0};

    auto invDet = 1.0f / det;

    auto T = ray.getOrigin() - vertices[0];
    auto u = glm::dot(T, P) * invDet;
    if (u < 0.0f || u > 1.0f)
        return Intersection{false, 0};

    auto Q = glm::cross(T, edge0);
    auto v = glm::dot(ray.getDirection(), Q) * invDet;
    if (v < 0.0f || u + v > 1.0f)
        return Intersection{false, 0};

    auto t = glm::dot(edge1, Q) * invDet;


    if (t < dist && t > std::numeric_limits<float>::epsilon()) {
        return Intersection{true, t};
    }

    return Intersection{false, 0};
}

Primitive::IntersectionVectors Triangle::getIntersectionVectors(const Ray &ray, const float dist) const {
    auto edge0 = vertices[1] - vertices[0];
    auto edge1 = vertices[2] - vertices[0];

    auto P = glm::cross(ray.getDirection(), edge1);

    auto det = glm::dot(edge0, P);
    if(abs(det) < std::numeric_limits<float>::epsilon())
        return IntersectionVectors{glm::vec3{0.0f}, glm::vec3{0.0f}};

    auto invDet = 1.0f / det;

    auto T = ray.getOrigin() - vertices[0];
    auto u = glm::dot(T, P) * invDet;
    if (u < 0.0f || u > 1.0f)
        return IntersectionVectors{glm::vec3{0.0f}, glm::vec3{0.0f}};

    auto Q = glm::cross(T, edge0);
    auto v = glm::dot(ray.getDirection(), Q) * invDet;
    if (v < 0.0f || u + v > 1.0f)
        return IntersectionVectors{glm::vec3{0.0f}, glm::vec3{0.0f}};

    auto t = glm::dot(edge1, Q) * invDet;

    //TODO hatte nur <= statt < eingefügt
    if (t <= dist && t > std::numeric_limits<float>::epsilon()) {
        auto N = glm::normalize(glm::cross(edge0, edge1));
        auto P1 = ray.getOrigin() + t * ray.getDirection();
        return IntersectionVectors{P1, N};
    }

    return IntersectionVectors{glm::vec3{0.0f}, glm::vec3{0.0f}};
}

std::tuple<glm::vec3, glm::vec3> Triangle::getExtremes() const {
    auto minimum = glm::vec3{std::numeric_limits<float>::max()};
    auto maximum = glm::vec3{std::numeric_limits<float>::min()};

    for (auto const& vertex : vertices) {
        minimum.x = (vertex.x < minimum.x) ? vertex.x : minimum.x;
        minimum.y = (vertex.y < minimum.y) ? vertex.y : minimum.y;
        minimum.z = (vertex.z < minimum.z) ? vertex.z : minimum.z;

        maximum.x = (vertex.x > maximum.x) ? vertex.x : maximum.x;
        maximum.y = (vertex.y > maximum.y) ? vertex.y : maximum.y;
        maximum.z = (vertex.z > maximum.z) ? vertex.z : maximum.z;
    }

    return std::make_tuple(minimum, maximum);
}

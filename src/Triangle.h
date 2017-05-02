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

    //TODO ctors aufräumen
    Triangle(const glm::vec3 p0, const glm::vec3 p1, const glm::vec3 p2) :
            Primitive{},
            vertices{p0, p1, p2} {
    }

    Triangle(const glm::vec3 p0, const glm::vec3 p1, const glm::vec3 p2, const std::shared_ptr<Material> material) :
            Primitive{material},
            vertices{p0, p1, p2} {
    }

    Triangle(const std::array<glm::vec3, 3> vertices) :
            Primitive{},
            vertices(std::move(vertices)) {
    }

    Triangle(const std::array<glm::vec3, 3> vertices, const std::shared_ptr<Material> material) :
            Primitive{material},
            vertices(std::move(vertices)) {
    }

    //TODO intersect & intersectionVectors kombinieren/komplett trennen
    std::tuple<bool, float> intersect(const Ray &ray, const float dist) const override {
        auto edge0 = vertices[1] - vertices[0];
        auto edge1 = vertices[2] - vertices[0];

        auto P = glm::cross(ray.getDirection(), edge1);

        auto det = glm::dot(edge0, P);
        if(abs(det) < std::numeric_limits<float>::epsilon())
            return std::make_tuple(false, 0);

        auto invDet = 1.0f / det;

        auto T = ray.getOrigin() - vertices[0];
        auto u = glm::dot(T, P) * invDet;
        if (u < 0.0f || u > 1.0f)
            return std::make_tuple(false, 0);

        auto Q = glm::cross(T, edge0);
        auto v = glm::dot(ray.getDirection(), Q) * invDet;
        if (v < 0.0f || u + v > 1.0f)
            return std::make_tuple(false, 0);

        auto t = glm::dot(edge1, Q) * invDet;


        if (t < dist && t > std::numeric_limits<float>::epsilon()) {
            return std::make_tuple(true, t);
        }

        return std::make_tuple(false, 0);
    }

    std::tuple<glm::vec3, glm::vec3> getIntersectionVectors(const Ray &ray, const float dist) const override {
        auto edge0 = vertices[1] - vertices[0];
        auto edge1 = vertices[2] - vertices[0];

        auto P = glm::cross(ray.getDirection(), edge1);

        auto det = glm::dot(edge0, P);
        if(abs(det) < std::numeric_limits<float>::epsilon())
            return std::make_tuple(glm::vec3{0.0f}, glm::vec3{0.0f});

        auto invDet = 1.0f / det;

        auto T = ray.getOrigin() - vertices[0];
        auto u = glm::dot(T, P) * invDet;
        if (u < 0.0f || u > 1.0f)
            return std::make_tuple(glm::vec3{0.0f}, glm::vec3{0.0f});

        auto Q = glm::cross(T, edge0);
        auto v = glm::dot(ray.getDirection(), Q) * invDet;
        if (v < 0.0f || u + v > 1.0f)
            return std::make_tuple(glm::vec3{0.0f}, glm::vec3{0.0f});

        auto t = glm::dot(edge1, Q) * invDet;

        //TODO hatte nur <= statt < eingefügt
        if (t <= dist && t > std::numeric_limits<float>::epsilon()) {
            auto N = glm::normalize(glm::cross(edge0, edge1));
            auto P1 = ray.getOrigin() + t * ray.getDirection();
            return std::make_tuple(P1, N);
        }

        return std::make_tuple(glm::vec3{0.0f}, glm::vec3{0.0f});
    }

    std::tuple<glm::vec3, glm::vec3> getExtremes() const override {
        auto minimum = glm::vec3{std::numeric_limits<float>::max()};
        auto maximum = glm::vec3{std::numeric_limits<float>::min()};

        for (auto const vertex : vertices) {
            minimum.x = (vertex.x < minimum.x) ? vertex.x : minimum.x;
            minimum.y = (vertex.y < minimum.y) ? vertex.y : minimum.y;
            minimum.z = (vertex.z < minimum.z) ? vertex.z : minimum.z;

            maximum.x = (vertex.x > maximum.x) ? vertex.x : maximum.x;
            maximum.y = (vertex.y > maximum.y) ? vertex.y : maximum.y;
            maximum.z = (vertex.z > maximum.z) ? vertex.z : maximum.z;
        }

        return std::make_tuple(minimum, maximum);
    }

private:
    std::array<glm::vec3, 3> vertices;
};


#endif //FAST_RAYTRACER_TRIANGLE_H

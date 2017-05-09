//
// Created by patrick on 01.05.17.
//

#ifndef FAST_RAYTRACER_BOUNDINGBOX_H
#define FAST_RAYTRACER_BOUNDINGBOX_H

#include "Model.h"

class BoundingBox {
public:
    BoundingBox(const std::vector<std::shared_ptr<Primitive>> primitives) :
            min{std::move(glm::vec3{std::numeric_limits<float>::max()})},
            max{std::move(glm::vec3{std::numeric_limits<float>::min()})} {
        
        for (auto const& primitive : primitives) {
            auto extremesX = primitive->getExtremes(Primitive::Axis::x);
            auto extremesY = primitive->getExtremes(Primitive::Axis::y);
            auto extremesZ = primitive->getExtremes(Primitive::Axis::z);

            min.x = (extremesX.min < min.x) ? extremesX.min : min.x;
            min.y = (extremesY.min < min.y) ? extremesY.min : min.y;
            min.z = (extremesZ.min < min.z) ? extremesZ.min : min.z;

            max.x = (extremesX.max > max.x) ? extremesX.max : max.x;
            max.y = (extremesY.max > max.y) ? extremesY.max : max.y;
            max.z = (extremesZ.max > max.z) ? extremesZ.max : max.z;
        }
    }
    
    BoundingBox(const glm::vec3 min, const glm::vec3 max) :
            min{std::move(min)},
            max{std::move(max)} {
    }

    bool intersect(const Ray &ray) const {
        auto tmin = 0.0f;
        auto tmax = 0.0f;

        if (ray.getInverseDirection().x >= 0) {
            tmin = (min.x - ray.getOrigin().x) * ray.getInverseDirection().x;
            tmax = (max.x - ray.getOrigin().x) * ray.getInverseDirection().x;
        }
        else {
            tmin = (max.x - ray.getOrigin().x) * ray.getInverseDirection().x;
            tmax = (min.x - ray.getOrigin().x) * ray.getInverseDirection().x;
        }

        auto tymin = 0.0f;
        auto tymax = 0.0f;

        if (ray.getInverseDirection().y >= 0) {
            tymin = (min.y - ray.getOrigin().y) * ray.getInverseDirection().y;
            tymax = (max.y - ray.getOrigin().y) * ray.getInverseDirection().y;
        }
        else {
            tymin = (max.y - ray.getOrigin().y) * ray.getInverseDirection().y;
            tymax = (min.y - ray.getOrigin().y) * ray.getInverseDirection().y;
        }

        if ((tmin > tymax) || (tymin > tmax))
            return false;

        if (tymin > tmin)
            tmin = tymin;

        if (tymax < tmax)
            tmax = tymax;

        auto tzmin = 0.0f;
        auto tzmax = 0.0f;

        if (ray.getInverseDirection().z >= 0) {
            tzmin = (min.z - ray.getOrigin().z) * ray.getInverseDirection().z;
            tzmax = (max.z - ray.getOrigin().z) * ray.getInverseDirection().z;
        }
        else {
            tzmin = (max.z - ray.getOrigin().z) * ray.getInverseDirection().z;
            tzmax = (min.z - ray.getOrigin().z) * ray.getInverseDirection().z;
        }

        if ((tmin > tzmax) || (tzmin > tmax))
            return false;

        if (tzmin > tmin)
            tmin = tzmin;

        if (tzmax < tmax)
            tmax = tzmax;

        // ignore intersections behind the ray
        return tmax >= 0.0f;
//         return true;
    }

private:
    glm::vec3 min;
    glm::vec3 max;
};


#endif //FAST_RAYTRACER_BOUNDINGBOX_H

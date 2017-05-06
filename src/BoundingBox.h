//
// Created by patrick on 01.05.17.
//

#ifndef FAST_RAYTRACER_BOUNDINGBOX_H
#define FAST_RAYTRACER_BOUNDINGBOX_H


#include "Model.h"

class BoundingBox {
public:
    BoundingBox(std::vector<std::shared_ptr<Primitive>> primitives) : 
            min{std::move(glm::vec3{std::numeric_limits<float>::max()})},
            max{std::move(glm::vec3{std::numeric_limits<float>::min()})} {
        
        for (auto primitive : primitives) {
            auto extremes = primitive->getExtremes();

            auto primMin = std::get<0>(extremes);
            min.x = (primMin.x < min.x) ? primMin.x : min.x;
            min.y = (primMin.y < min.y) ? primMin.y : min.y;
            min.z = (primMin.z < min.z) ? primMin.z : min.z;

            auto primMax = std::get<1>(extremes);
            max.x = (primMax.x > max.x) ? primMax.x : max.x;
            max.y = (primMax.y > max.y) ? primMax.y : max.y;
            max.z = (primMax.z > max.z) ? primMax.z : max.z;
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

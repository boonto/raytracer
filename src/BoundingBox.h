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

    //TODO drüberschauen/optimieren
    bool intersect(const Ray &ray) const {
        float tmin = (min.x - ray.getOrigin().x) / ray.getDirection().x;
        float tmax = (max.x - ray.getOrigin().x) / ray.getDirection().x;

        if (tmin > tmax) std::swap(tmin, tmax);

        float tymin = (min.y - ray.getOrigin().y) / ray.getDirection().y;
        float tymax = (max.y - ray.getOrigin().y) / ray.getDirection().y;

        if (tymin > tymax) std::swap(tymin, tymax);

        if ((tmin > tymax) || (tymin > tmax))
            return false;

        if (tymin > tmin)
            tmin = tymin;

        if (tymax < tmax)
            tmax = tymax;

        float tzmin = (min.z - ray.getOrigin().z) / ray.getDirection().z;
        float tzmax = (max.z - ray.getOrigin().z) / ray.getDirection().z;

        if (tzmin > tzmax) std::swap(tzmin, tzmax);

        if ((tmin > tzmax) || (tzmin > tmax))
            return false;

        if (tzmin > tmin)
            tmin = tzmin;

        if (tzmax < tmax)
            tmax = tzmax;

        return true;
    }

private:
    glm::vec3 min;
    glm::vec3 max;
};


#endif //FAST_RAYTRACER_BOUNDINGBOX_H

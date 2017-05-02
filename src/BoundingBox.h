//
// Created by patrick on 01.05.17.
//

#ifndef FAST_RAYTRACER_BOUNDINGBOX_H
#define FAST_RAYTRACER_BOUNDINGBOX_H


#include "Model.h"

class BoundingBox {
public:
    BoundingBox(const glm::vec3 &vmin, const glm::vec3 &vmax) :
            min(vmin),
            max(vmax) {
    }

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

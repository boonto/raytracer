//
// Created by patrick on 01.05.17.
//

#ifndef FAST_RAYTRACER_BOUNDINGBOX_H
#define FAST_RAYTRACER_BOUNDINGBOX_H

#include "Model.h"

class BoundingBox {
public:
    explicit BoundingBox(const std::vector<std::shared_ptr<Primitive>>& primitives) :
            min{glm::vec3{std::numeric_limits<float>::max()}},
            max{glm::vec3{std::numeric_limits<float>::min()}} {

        for (auto const &primitive : primitives) {
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
            min{min},
            max{max} {
    }

    struct Intersection {
        bool result;
        float tmin;
        float tsplit;
        float tmax;
    };

    //https://tavianator.com/fast-branchless-raybounding-box-intersections-part-2-nans/
    Intersection intersect(const Ray &ray, const float splitLocation, const Primitive::Axis splitAxis) const {
        auto tx1 = (min.x - ray.getOrigin().x) * ray.getInverseDirection().x;
        auto tx2 = (max.x - ray.getOrigin().x) * ray.getInverseDirection().x;

        auto tmin = std::min(tx1, tx2);
        auto tmax = std::max(tx1, tx2);

        auto ty1 = (min.y - ray.getOrigin().y) * ray.getInverseDirection().y;
        auto ty2 = (max.y - ray.getOrigin().y) * ray.getInverseDirection().y;

        tmin = std::max(tmin, std::min(std::min(ty1, ty2), tmax));
        tmax = std::min(tmax, std::max(std::max(ty1, ty2), tmin));

        auto tz1 = (min.z - ray.getOrigin().z) * ray.getInverseDirection().z;
        auto tz2 = (max.z - ray.getOrigin().z) * ray.getInverseDirection().z;

        tmin = std::max(tmin, std::min(std::min(tz1, tz2), tmax));
        tmax = std::min(tmax, std::max(std::max(tz1, tz2), tmin));

        auto tsplit = 0.0f;
        switch (splitAxis) {
            case Primitive::Axis::x: {
                tsplit = (splitLocation - ray.getOrigin().x) * ray.getInverseDirection().x;
                break;
            }
            case Primitive::Axis::y: {
                tsplit = (splitLocation - ray.getOrigin().y) * ray.getInverseDirection().y;
                break;
            }
            case Primitive::Axis::z: {
                tsplit = (splitLocation - ray.getOrigin().z) * ray.getInverseDirection().z;
                break;
            }
        }

        return Intersection{tmax >= tmin && tmax > 0, tmin, tsplit, tmax};
    }

    glm::vec3 getMin() const {
        return min;
    }

    glm::vec3 getMax() const {
        return max;
    }

private:
    glm::vec3 min;
    glm::vec3 max;
};


#endif //FAST_RAYTRACER_BOUNDINGBOX_H

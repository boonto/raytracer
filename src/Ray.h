//
// Created by Patrick Werner on 21.04.17.
//

#ifndef RAYTRACER_RAY_H
#define RAYTRACER_RAY_H

#include <glm/vec3.hpp>

class Ray {
public:
    Ray(const glm::vec3 origin, const glm::vec3 direction) :
            origin{origin},
            direction{direction},
            invDir{glm::vec3{1.0f} / direction} {
    }

    glm::vec3 getOrigin() const {
        return origin;
    }

    glm::vec3 getDirection() const {
        return direction;
    }

    glm::vec3 getInverseDirection() const {
        return invDir;
    }

private:
    glm::vec3 origin;
    glm::vec3 direction;
    glm::vec3 invDir;
};

#endif //RAYTRACER_RAY_H

//
// Created by patrick on 21.04.17.
//

#ifndef RAYTRACER_RAY_H
#define RAYTRACER_RAY_H

#include <glm/vec3.hpp>

class Ray {
public:
    Ray(const glm::vec3 origin, const glm::vec3 direction) :
            origin{std::move(origin)},
            direction{std::move(direction)} {
    }

    const glm::vec3 &getOrigin() const {
        return origin;
    }

    const glm::vec3 &getDirection() const {
        return direction;
    }

private:
    glm::vec3 origin;
    glm::vec3 direction;
};

#endif //RAYTRACER_RAY_H

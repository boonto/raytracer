//
// Created by patrick on 21.04.17.
//

#ifndef RAYTRACER_CAMERA_H
#define RAYTRACER_CAMERA_H

#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include "Ray.h"

class Camera {
public:
    Camera(const glm::vec3 origin, const glm::vec3 up, const glm::vec3 viewingPlaneCenter, const glm::ivec2 resolution) :
            origin{std::move(origin)},
            up{std::move(up)},
            gaze{viewingPlaneCenter - origin},
            dist{glm::length(gaze)},
            hFov{24.0f * float(M_PI) / 180.0f},
            vFov{24.0f * float(M_PI) / 180.0f},
            scrnX{glm::normalize(glm::cross(gaze, up)) * 2.0f * dist * (tan(hFov)/float(resolution.x))},
            scrnY{glm::normalize(glm::cross(scrnX, gaze)) * 2.0f * dist * (tan(vFov)/float(resolution.y))},
            first{gaze - scrnX * float(resolution.x) * 0.5f + scrnY * float(resolution.y) * 0.5f} {
    }

    Ray getRay(const unsigned int x, const unsigned int y) const {
        return Ray{origin, glm::normalize(first + scrnX * float(x) - scrnY * float(y))};
    }

private:
    glm::vec3 origin;
    glm::vec3 up;
    glm::vec3 gaze;

    float dist;
    float hFov;
    float vFov;

    glm::vec3 scrnX;
    glm::vec3 scrnY;

    glm::vec3 first;
};

#endif //RAYTRACER_CAMERA_H

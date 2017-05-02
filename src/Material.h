//
// Created by Patrick Werner on 21.04.17.
//

#ifndef RAYTRACER_MATERIAL_H
#define RAYTRACER_MATERIAL_H

#include <glm/vec3.hpp>

class Material {
public:
    Material(const glm::vec3 diffuseColor = glm::vec3{0.8f}, const float kAmbient = 0.0f, const float kDiffuse = 1.0f,
             const float kSpecular = 0.0f, const float kReflective = 0.0f, const float kTransparent = 0.0f,
             const unsigned int specPower = 0, const float refrIndex = 0.0f) :
            diffuseColor{std::move(diffuseColor)},
            kAmbient{kAmbient},
            kDiffuse{kDiffuse},
            kSpecular{kSpecular},
            kReflective{kReflective},
            kTransparent{kTransparent},
            specPower{specPower},
            refrIndex{refrIndex} {
    }

    glm::vec3 diffuseColor;
    float kAmbient;
    float kDiffuse;
    float kSpecular;
    float kReflective;
    float kTransparent;
    unsigned int specPower;
    float refrIndex;
};

#endif //RAYTRACER_MATERIAL_H

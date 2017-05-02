//
// Created by Patrick Werner on 21.04.17.
//

#ifndef RAYTRACER_MATERIAL_H
#define RAYTRACER_MATERIAL_H

#include <glm/vec3.hpp>

class Material {
public:
    Material(const glm::vec3 diffuseColor, const float kAmbient, const float kDiffuse,
             const float kSpecular, const float kReflective, const float kTransparent,
             const unsigned int specPower, const float refrIndex) :
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

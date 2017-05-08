//
// Created by Patrick Werner on 01.05.17.
//

#ifndef FAST_RAYTRACER_PRIMITIVE_H
#define FAST_RAYTRACER_PRIMITIVE_H

#include <tuple>
#include <memory>
#include "glm/vec3.hpp"
#include "Ray.h"
#include "Material.h"

class Primitive {
public:
    struct IntersectionA {
        bool result;
        float t;
    };

    struct IntersectionB {
        glm::vec3 position;
        glm::vec3 normal;
    };

    enum class Axis {
        x,
        y,
        z
    };

    Primitive(const std::shared_ptr<Material> material = std::make_shared<Material>(Material{})) :
            material{std::move(material)} {
    }

    //TODO
    virtual ~Primitive() {
    }

    virtual IntersectionA intersect(const Ray &ray, const float dist) const = 0; //TODO: schönerer retval?

    virtual IntersectionB getIntersectionVectors(const Ray &ray, const float dist) const = 0;

    virtual std::tuple<glm::vec3, glm::vec3> getExtremes() const = 0;

    std::shared_ptr<Material> getMaterial() const {
        return material;
    }

private:
    std::shared_ptr<Material> material;
};


#endif //FAST_RAYTRACER_PRIMITIVE_H

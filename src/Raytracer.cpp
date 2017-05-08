//
// Created by Patrick Werner on 08.05.17.
//

#include "Raytracer.h"

unsigned int Raytracer::render(Window &window) {
    auto resolution = scene->camera.getResolution();
    counter = 0;
    for (unsigned int y = 0; y < resolution.y; ++y) {
        for (unsigned int x = 0; x < resolution.x; ++x) {
            auto ray = scene->camera.getRay(x, y);
            auto color = raytrace(ray, 0);
            color.r = std::min(std::max(color.r, 0.0f), 1.0f);
            color.g = std::min(std::max(color.g, 0.0f), 1.0f);
            color.b = std::min(std::max(color.b, 0.0f), 1.0f);

            window.putPixel(x, y, color);
        }
        window.setTitle(std::to_string(int(roundf((y+1)/float(resolution.y)*100))) + "%");
    }
    return counter;
}

glm::tvec3<float, 0> Raytracer::raytrace(const Ray &ray, const int depth) {
    if (depth > MAX_DEPTH) {
        return BACKGROUND_COLOR;
    }

    // mit kdtree
    auto intersection = kdTree.intersect(ray, counter);

    // ohne kdtree
    //    for (auto const& primitive : primitives) {
    //        auto newIntersection = primitive->intersect(ray, minDistance);
    //        if(std::get<0>(newIntersection)) {
    //            m = primitive;
    //            intersection = newIntersection;
    //            minDistance = std::get<1>(newIntersection);
    //        }
    //        counter++;
    //    }

    auto color = BACKGROUND_COLOR;
    if(!std::get<1>(intersection).expired()) {
        color = shade(ray, depth, std::get<0>(intersection), std::get<1>(intersection));
    }

    return color;
}

glm::vec3 Raytracer::shade(const Ray &ray, const int depth, const float t, const std::weak_ptr<Primitive> &prim) {
    auto intersectionVectors = prim.lock()->getIntersectionVectors(ray, t);
    auto intersectionPosition = std::get<0>(intersectionVectors);
    auto intersectionNormal = std::get<1>(intersectionVectors);
    auto color = glm::vec3(0.0f);
    auto material = prim.lock()->getMaterial();

    if (material->kDiffuse > 0.0f) {
        auto lightAmt = glm::vec3{0.0f};
        auto specularColor = glm::vec3{0.0f};

        auto shadowPointOrigin = (dot(ray.getDirection(), intersectionNormal) < 0.0f) ?
                                 intersectionPosition + intersectionNormal * std::numeric_limits<float>::epsilon() * 10000.0f :
                                 intersectionPosition - intersectionNormal * std::numeric_limits<float>::epsilon() * 10000.0f;

        for (auto const &light : scene->lights) {
            auto lightDirection = light->origin - intersectionPosition;
            auto lightDistance2 = dot(lightDirection, lightDirection);
            lightDirection = normalize(lightDirection);
            auto LdotN = std::max(dot(lightDirection, intersectionNormal), 0.0f);
            auto tNearShadow = lightDistance2;

            auto inShadow = false;
            // Blocker Cache
            if (!light->blocker.expired()) {
                auto shadowIntersection = light->blocker.lock()->intersect(Ray{shadowPointOrigin, lightDirection}, tNearShadow);
                // needed with tNearShadow = lightDistance2?
                inShadow = std::get<0>(shadowIntersection)/* && tNearShadow * tNearShadow < lightDistance2*/;
            }

            // TODO Kdtree nutzen
            if (!inShadow) {
                for (auto const &model : scene->primitives) {
                    auto shadowIntersection = model->intersect(Ray{shadowPointOrigin, lightDirection}, tNearShadow);
                    tNearShadow = std::get<1>(shadowIntersection);
                    inShadow = std::get<0>(shadowIntersection)/* && tNearShadow * tNearShadow < lightDistance2*/;
                    if (inShadow) {
                        light->blocker = prim;
                        break;
                    }
                }
            }

            lightAmt += (1 - inShadow) * light->intensity * LdotN;
            auto reflectionDirection = normalize(reflect(-lightDirection, intersectionNormal));
            specularColor += powf(std::max(-glm::dot(reflectionDirection, ray.getDirection()), 0.0f), material->specPower) * light->intensity;
        }
        color = lightAmt * material->diffuseColor * material->kDiffuse + specularColor * material->kSpecular;
    }

    // Reflection
    if (material->kReflective > 0.0f) {
        auto reflectionDirection = reflect(normalize(intersectionPosition - ray.getOrigin()),
                                           intersectionNormal);
        auto reflectionOrigin = (dot(reflectionDirection, intersectionNormal) < 0) ?
                                intersectionPosition + intersectionNormal * std::numeric_limits<float>::epsilon() :
                                intersectionPosition - intersectionNormal * std::numeric_limits<float>::epsilon(); //TODO: nötig?
        auto reflectionColor = raytrace(Ray{reflectionOrigin, reflectionDirection}, depth + 1) * material->kReflective;
        color = reflectionColor;
    }

    return color;
}

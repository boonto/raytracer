//
// Created by Patrick Werner on 08.05.17.
//

#include "Raytracer.h"

unsigned long long Raytracer::render(Window &window) {
    std::cout << "Scene with " << scene->primitives.size() << " primitives and " << scene->lights.size() << " lights";

    auto resolution = scene->camera.getResolution();
    counter = 0;
    for (unsigned int y = 0; y < resolution.y; ++y) {
        for (unsigned int x = 0; x < resolution.x; ++x) {
            auto ray = scene->camera.getRay(x, y);
            auto color = raytrace(ray, 0);
            color.r = std::min(std::max(color.r, 0.0f), 1.0f);
            color.g = std::min(std::max(color.g, 0.0f), 1.0f);
            color.b = std::min(std::max(color.b, 0.0f), 1.0f);

            window.putPixel((int) x, (int) y, color);
        }
        window.setTitle(std::to_string(int(roundf(((float) y + 1.0f) / float(resolution.y) * 100))) + "%");
    }
    return counter;
}

glm::vec3 Raytracer::raytrace(const Ray &ray, const int depth) {
    if (depth > MAX_DEPTH) {
        return BACKGROUND_COLOR;
    }

    auto intersection = std::tuple<float, std::weak_ptr<Primitive>>{};
    if (USE_KDTREE) {
        intersection = kdTree.intersect(ray, counter);
    } else {
        std::get<0>(intersection) = std::numeric_limits<float>::max();
        for (auto const &primitive : scene->primitives) {
            auto newIntersection = primitive->intersect(ray, std::get<0>(intersection));
            if (newIntersection.result) {
                intersection = std::make_tuple(newIntersection.t, primitive);
            }
            counter++;
        }
    }

    auto color = BACKGROUND_COLOR;
    if (!std::get<1>(intersection).expired()) {
        color = shade(ray, depth, std::get<0>(intersection), std::get<1>(intersection));
    }

    return color;
}

glm::vec3 Raytracer::shade(const Ray &ray, const int depth, const float t, const std::weak_ptr<Primitive> &prim) {
    auto intersectionVectors = prim.lock()->getIntersectionVectors(ray, t);
    auto intersectionPosition = intersectionVectors.position;
    auto intersectionNormal = intersectionVectors.normal;
    auto color = glm::vec3(0.0f);
    auto material = prim.lock()->getMaterial();

    if (material->kDiffuse > 0.0f) {
        auto lightAmt = glm::vec3{0.0f};
        auto specularColor = glm::vec3{0.0f};

        auto shadowPointOrigin = (dot(ray.getDirection(), intersectionNormal) < 0.0f) ?
                                 intersectionPosition +
                                 intersectionNormal * std::numeric_limits<float>::epsilon() * 10000.0f :
                                 intersectionPosition -
                                 intersectionNormal * std::numeric_limits<float>::epsilon() * 10000.0f;

        for (auto const &light : scene->lights) {
            auto lightDirection = light->origin - intersectionPosition;
            auto lightDistance2 = dot(lightDirection, lightDirection);
            lightDirection = normalize(lightDirection);
            auto LdotN = std::max(dot(lightDirection, intersectionNormal), 0.0f);
            auto tNearShadow = lightDistance2;

            auto inShadow = false;
            // Blocker Cache
            if (!light->blocker.expired()) {
                auto shadowIntersection = light->blocker.lock()->intersect(Ray{shadowPointOrigin, lightDirection},
                                                                           tNearShadow);
                inShadow = shadowIntersection.result && tNearShadow * tNearShadow < lightDistance2;
            }

            if (!inShadow) {
                if (USE_KDTREE) {
                    auto shadowIntersection = kdTree.intersect(Ray{shadowPointOrigin, lightDirection}, counter);
                    tNearShadow = std::get<0>(shadowIntersection);
                    inShadow = !std::get<1>(shadowIntersection).expired() &&
                               tNearShadow * tNearShadow < lightDistance2;
                    if (inShadow) {
                        light->blocker = prim;
                        break;
                    }
                } else {
                    for (auto const &model : scene->primitives) {
                        auto shadowIntersection = model->intersect(Ray{shadowPointOrigin, lightDirection},
                                                                   tNearShadow);
                        counter++;
                        tNearShadow = shadowIntersection.t;
                        inShadow = shadowIntersection.result && tNearShadow * tNearShadow < lightDistance2;
                        if (inShadow) {
                            light->blocker = prim;
                            break;
                        }
                    }
                }
            }

            lightAmt += (1.0f - (float) inShadow) * light->intensity * LdotN;
            auto reflectionDirection = normalize(reflect(-lightDirection, intersectionNormal));
            specularColor +=
                    powf(std::max(-glm::dot(reflectionDirection, ray.getDirection()), 0.0f),
                         (float) material->specPower) *
                    light->intensity;
        }
        color += (lightAmt * material->diffuseColor * material->kDiffuse + specularColor * material->kSpecular);
    }

    // Reflection
    if (material->kReflective > 0.0f) {
        auto reflectionDirection = reflect(normalize(intersectionPosition - ray.getOrigin()),
                                           intersectionNormal);
        auto reflectionOrigin = (dot(reflectionDirection, intersectionNormal) < 0) ?
                                intersectionPosition + intersectionNormal * std::numeric_limits<float>::epsilon() :
                                intersectionPosition -
                                intersectionNormal * std::numeric_limits<float>::epsilon(); //TODO: nötig?
        auto reflectionColor = raytrace(Ray{reflectionOrigin, reflectionDirection}, depth + 1) * material->kReflective;
        color += reflectionColor;
    }

    // Refraction
    // based on https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-shading/reflection-refraction-fresnel
    if (material->kTransparent > 0.0f) {
        auto dir = normalize(intersectionPosition - ray.getOrigin());
        auto refractionColor = glm::vec3(0.0f);
        auto reflectedLight = fresnel(dir, intersectionNormal, material->refrIndex);
        auto isOutside = dot(dir, intersectionNormal) < 0;
        auto bias = 0.01f * intersectionNormal;
        if (reflectedLight < 1) {
            glm::vec3 refractionDirection = normalize(refract(dir, intersectionNormal, material->refrIndex));
            glm::vec3 refractionRayOrig = isOutside ? intersectionPosition - bias : intersectionPosition + bias;
            refractionColor = raytrace({refractionRayOrig, refractionDirection}, depth + 1);
        }
        auto reflectionDirection = normalize(reflect(dir, intersectionNormal));
        auto reflectionRayOrig = isOutside ? intersectionPosition + bias : intersectionPosition - bias;
        auto reflectionColor = raytrace({reflectionRayOrig, reflectionDirection}, depth + 1);

        color += reflectionColor * reflectedLight + refractionColor * (1.0f - reflectedLight) * material->kTransparent;
    }

    return color;
}

// based on https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-shading/reflection-refraction-fresnel
glm::vec3 Raytracer::refract(const glm::vec3 &intersectionDirection, const glm::vec3 &intersectionNormal,
                             const float &refrIndex) {
    auto intersectionAngle = glm::clamp(dot(intersectionDirection, intersectionNormal), -1.0f, 1.0f);
    auto refrIndexInc = 1.0f;
    auto refrIndexObj = refrIndex;
    auto normal = intersectionNormal;
    if (intersectionAngle < 0.0f) {
        intersectionAngle = -intersectionAngle;
    } else {
        std::swap(refrIndexInc, refrIndexObj);
        normal = -intersectionNormal;
    }
    auto eta = refrIndexInc / refrIndexObj;
    auto k = 1.0f - eta * eta * (1.0f - intersectionAngle * intersectionAngle);
    return k < 0.0f ? glm::vec3{0.0f} : eta * intersectionDirection + (eta * intersectionAngle - sqrtf(k)) * normal;
}

// based on https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-shading/reflection-refraction-fresnel
float Raytracer::fresnel(const glm::vec3 &intersectionDirection, const glm::vec3 &intersectionNormal,
                         const float &refrIndex) {
    auto intersectionAngle = glm::clamp(dot(intersectionDirection, intersectionNormal), -1.0f, 1.0f);
    auto refrIndexInc = 1.0f, refrIndexObj = refrIndex;
    if (intersectionAngle > 0.0f) {
        std::swap(refrIndexInc, refrIndexObj);
    }
    auto refractionAngle =
            refrIndexInc / refrIndexObj * sqrtf(std::max(0.0f, 1.0f - intersectionAngle * intersectionAngle));
    if (refractionAngle >= 1.0f) {
        return 1.0f;
    } else {
        auto cost = sqrtf(std::max(0.f, 1.0f - refractionAngle * refractionAngle));
        intersectionAngle = fabsf(intersectionAngle);
        auto Rs = ((refrIndexObj * intersectionAngle) - (refrIndexInc * cost)) /
                  ((refrIndexObj * intersectionAngle) + (refrIndexInc * cost));
        auto Rp = ((refrIndexInc * intersectionAngle) - (refrIndexObj * cost)) /
                  ((refrIndexInc * intersectionAngle) + (refrIndexObj * cost));
        return (Rs * Rs + Rp * Rp) / 2.0f;
    }
}

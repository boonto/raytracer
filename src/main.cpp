//
// Created by Patrick Werner on 30.04.17.
//

#include <iostream>
#include <vector>
#include <tuple>
#include <chrono>
#include <stack>
#include "SDL2/SDL.h"
#include "glm/glm.hpp"
#include "glm/gtx/norm.hpp"
#include "glm/gtx/string_cast.hpp"
#include "Model.h"
#include "PointLight.h"
#include "Camera.h"
#include "Window.h"
#include "Scene.h"
#include "KdTree.h"

//TODO: code verbessern, t/dist drübergehen

const int MAX_DEPTH = 5;
const int WIDTH = 600;
const int HEIGHT = 600;
const auto MAX_VALUE = std::numeric_limits<float>::max();
const auto EPSILON = std::numeric_limits<float>::epsilon();
const glm::vec3 BACKGROUND_COLOR(0.0f, 0.0f, 0.0f);

int counter = 0;

KdTree gtree{std::vector<std::shared_ptr<Primitive>>()};

glm::vec3 shade(const Ray &ray, int depth, const std::vector<std::shared_ptr<Primitive>> &primitives,
                const std::vector<std::shared_ptr<PointLight>> &lights, glm::vec3 &color,
                std::tuple<bool, float> &intersection, const std::weak_ptr<Primitive> &m);

//TODO: eigene Klasse
glm::vec3 raytrace(const Ray &ray, int depth, const std::vector<std::shared_ptr<Primitive>> &primitives, const std::vector<std::shared_ptr<PointLight>> &lights) {
    if (depth > MAX_DEPTH) {
        return BACKGROUND_COLOR;
    }

    auto color = BACKGROUND_COLOR;
    auto minDistance = MAX_VALUE;

    auto intersection = std::tuple<bool, float>{}; //TODO init?
    auto m = std::weak_ptr<Primitive>{};

    // mit kdgrid
    auto node = std::make_shared<KdTreeNode>(gtree.root);
    auto stack = std::stack<std::shared_ptr<KdTreeNode>>{};
    stack.push(node);
    //TODO eigene funktion
    while(!stack.empty()) {
        node = stack.top();
        stack.pop();
        if (node->bbox) {
            if (node->bbox->intersect(ray)) {
                if (node->rightChild) {
                    stack.push(node->rightChild);
                }
                if (node->leftChild) {
                    stack.push(node->leftChild);
                }
            }
        } else if (node->data) {
            auto newIntersection = node->data->intersect(ray, minDistance);
            if (std::get<0>(newIntersection)) {
                m = node->data;
                intersection = newIntersection;
                minDistance = std::get<1>(newIntersection);
            }
            counter++;
        }
    }

    // ohne kdgrid
//    for (auto const& primitive : primitives) {
//        auto newIntersection = primitive->intersect(ray, minDistance);
//        if(std::get<0>(newIntersection)) {
//            m = primitive;
//            intersection = newIntersection;
//            minDistance = std::get<1>(newIntersection);
//        }
//        counter++;
//    }

    // TODO eigene funktion
    if(!m.expired()) {
        color = shade(ray, depth, primitives, lights, color, intersection, m);

    }

    return color;
}

glm::vec3 shade(const Ray &ray, int depth, const std::vector<std::shared_ptr<Primitive>> &primitives,
const std::vector<std::shared_ptr<PointLight>> &lights, glm::vec3 &color,
std::tuple<bool, float> &intersection, const std::weak_ptr<Primitive> &m) {
    auto intersectionVectors = m.lock()->getIntersectionVectors(ray, std::get<1>(intersection));
    auto intersectionPosition = std::get<0>(intersectionVectors);
    auto intersectionNormal = std::get<1>(intersectionVectors);

    if (true) {
        //if (m.lock()->material.kDiffuse > 0.0f) {
        auto lightAmt = glm::vec3{0.0f};
        auto specularColor = glm::vec3{0.0f};

        auto shadowPointOrigin = (dot(ray.getDirection(), intersectionNormal) < 0.0f) ?
                             intersectionPosition + intersectionNormal * EPSILON * 10000.0f :
                             intersectionPosition - intersectionNormal * EPSILON * 10000.0f;

        for (auto const &light : lights) {
            auto lightDirection = light->origin - intersectionPosition;
            auto lightDistance2 = dot(lightDirection, lightDirection);
            lightDirection = normalize(lightDirection);
            auto LdotN = std::max(dot(lightDirection, intersectionNormal), 0.0f);
            auto tNearShadow = MAX_VALUE;

            //TODO schöner
            bool inShadow = false;
            // Blocker Cache
            if (!light->blocker.expired()) {
                auto shadowIntersection = light->blocker.lock()->intersect(Ray{shadowPointOrigin, lightDirection}, tNearShadow);
                tNearShadow = std::get<1>(shadowIntersection);
                inShadow = std::get<0>(shadowIntersection) && tNearShadow * tNearShadow <
                                                              lightDistance2; //TODO: nötig? vll tNearShadow = sqrt(lightDistance)
            }

            if (!inShadow) {
                for (auto const &model : primitives) {
                    auto shadowIntersection = model->intersect(Ray{shadowPointOrigin, lightDirection},
                                                               tNearShadow);
                    tNearShadow = std::get<1>(shadowIntersection);
                    inShadow = std::get<0>(shadowIntersection) && tNearShadow * tNearShadow <
                                                                  lightDistance2; //TODO: nötig? vll tNearShadow = sqrt(lightDistance)
                    if (inShadow) {
                        light->blocker = m;
                        break;
                    }
                }
            }

            lightAmt += (1 - inShadow) * light->intensity * LdotN;
            auto reflectionDirection = normalize(reflect(-lightDirection, intersectionNormal));
            specularColor += (1 - inShadow) * powf(std::max(-dot(reflectionDirection, ray.getDirection()), 0.0f), 100) * light->intensity;
            //specularColor += powf(std::max(-glm::dot(reflectionDirection, ray.getDirection()), 0.0f), m->material.specPower) * light->intensity;
        }
        color = lightAmt * glm::vec3{1.0f, 0.0f, 0.0f} * 1.0f + specularColor * 1.0f;
        //color = lightAmt * m.lock()->material.diffuseColor * m.lock()->material.kDiffuse + specularColor * m.lock()->material.kSpecular;
    }

    // Reflection
    if(false) {
        //if (m.lock()->material.kReflective > 0.0f) {
        auto reflectionDirection = reflect(normalize(intersectionPosition - ray.getOrigin()),
                                           intersectionNormal);
        auto reflectionOrigin = (dot(reflectionDirection, intersectionNormal) < 0) ?
                            intersectionPosition + intersectionNormal * EPSILON :
                            intersectionPosition - intersectionNormal * EPSILON; //TODO: nötig?
        auto reflectionColor = raytrace(Ray{reflectionOrigin, reflectionDirection}, depth + 1, primitives, lights);

        //reflectionColor *= m->material.kReflective;

        color = reflectionColor;
    }
    return color;
}

void render(Window &window, const Camera &camera, const std::vector<std::shared_ptr<Primitive>> &primitives,
            const std::vector<std::shared_ptr<PointLight>> &lights) {
    for (unsigned int y = 0; y < HEIGHT; ++y) {
        for (unsigned int x = 0; x < WIDTH; ++x) {
            auto ray = camera.getRay(x, y);
            auto color = raytrace(ray, 0, primitives, lights);
            color.r = std::min(std::max(color.r, 0.0f), 1.0f);
            color.g = std::min(std::max(color.g, 0.0f), 1.0f);
            color.b = std::min(std::max(color.b, 0.0f), 1.0f);

            window.putPixel(x, y, color);
        }
    }
}

int main() {
    auto t0 = std::chrono::high_resolution_clock::now();

    Window window{glm::ivec2{WIDTH, HEIGHT}};
    auto scene = Scene{glm::ivec2{WIDTH, HEIGHT}};

    gtree = KdTree{scene.primitives};

    auto t1 = std::chrono::high_resolution_clock::now();
    std::cout << "Init: " << std::chrono::duration_cast<std::chrono::milliseconds>(t1-t0).count()
              << " ms\n";

    render(window,
           scene.camera,
           scene.primitives, scene.lights);

    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout << "Render: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
              << " ms\n";

    std::cout << "Intersections checked: " << counter << "\n";
    window.render();

    return EXIT_SUCCESS;
}

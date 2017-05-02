//
// Created by patrick on 30.04.17.
//

#ifndef FAST_RAYTRACER_KDTREE_H
#define FAST_RAYTRACER_KDTREE_H

#include <memory>
#include <vector>
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <map>
#include <algorithm>
#include <chrono>
#include "Model.h"
#include "BoundingBox.h"

class KdTreeNode {
public:
    std::shared_ptr<BoundingBox> bbox;
    std::shared_ptr<KdTreeNode> leftChild;
    std::shared_ptr<KdTreeNode> rightChild;
    std::shared_ptr<Primitive> data;
};

class KdTree {
public:
    KdTree(std::vector<std::shared_ptr<Primitive>> primitives) {
        root = build(primitives, 0);

        std::cout << "lol\n";
    }

    KdTreeNode root;

private:
    KdTreeNode build(std::vector<std::shared_ptr<Primitive>> primitives, int depth) {
        auto node = KdTreeNode{};

        if (primitives.size() == 1) {
            node.data = primitives[0];
            return node;
        } else if (primitives.size() == 0) {
            return node;
        }

        // create bounding box TODO eigene funktion/klasse, umdenken
        auto minimum = glm::vec3{std::numeric_limits<float>::max()};
        auto maximum = glm::vec3{std::numeric_limits<float>::min()};
        for (auto primitive : primitives) {
            auto extremes = primitive->getExtremes();

            auto primMinimum = std::get<0>(extremes);
            minimum.x = (primMinimum.x < minimum.x) ? primMinimum.x : minimum.x;
            minimum.y = (primMinimum.y < minimum.y) ? primMinimum.y : minimum.y;
            minimum.z = (primMinimum.z < minimum.z) ? primMinimum.z : minimum.z;

            auto primMaximum = std::get<1>(extremes);
            maximum.x = (primMaximum.x > maximum.x) ? primMaximum.x : maximum.x;
            maximum.y = (primMaximum.y > maximum.y) ? primMaximum.y : maximum.y;
            maximum.z = (primMaximum.z > maximum.z) ? primMaximum.z : maximum.z;
        }

        node.bbox = std::make_shared<BoundingBox>(BoundingBox{minimum, maximum});

        auto axis = Primitive::Axis(depth % 3);

        // comparator lambda TODO getExtremes bei jedem vergleich? simpler?
        auto compare = [&axis](std::shared_ptr<Primitive> a, std::shared_ptr<Primitive> b) {
            auto extremesA = a->getExtremes();
            auto extremesB = b->getExtremes();
            auto positionA = 0.0f;
            auto positionB = 0.0f;
            switch (axis) {
                case Primitive::Axis::x: {
                    positionA = std::get<0>(extremesA).x + std::get<1>(extremesA).x;
                    positionB = std::get<0>(extremesB).x + std::get<1>(extremesB).x;
                    break;
                }
                case Primitive::Axis::y: {
                    positionA = std::get<0>(extremesA).y + std::get<1>(extremesA).y;
                    positionB = std::get<0>(extremesB).y + std::get<1>(extremesB).y;
                    break;
                }
                case Primitive::Axis::z: {
                    positionA = std::get<0>(extremesA).z + std::get<1>(extremesA).z;
                    positionB = std::get<0>(extremesB).z + std::get<1>(extremesB).z;
                    break;
                }
            }
            return positionA < positionB;
        };

        // median, not real median for even number of elements
        auto n = primitives.size() / 2;
        std::nth_element(primitives.begin(), primitives.begin()+n, primitives.end(), compare);
        auto extremes = primitives[n]->getExtremes();
        switch (axis) {
            case Primitive::Axis::x: {
                break;
            }
            case Primitive::Axis::y: {
                break;
            }
            case Primitive::Axis::z: {
                break;
            }
        }

        auto leftHalf = std::vector<std::shared_ptr<Primitive>>{primitives.begin(), primitives.begin() + n};
        auto rightHalf = std::vector<std::shared_ptr<Primitive>>{primitives.begin() + n, primitives.end()};

        if (leftHalf.size() > 0) {
            node.leftChild = std::make_shared<KdTreeNode>(build(leftHalf, depth + 1));
        }

        if (rightHalf.size() > 0) {
            node.rightChild = std::make_shared<KdTreeNode>(build(rightHalf, depth + 1));
        }

        return node;
    }
};

#endif //FAST_RAYTRACER_KDTREE_H

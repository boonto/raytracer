//
// Created by Patrick Werner on 30.04.17.
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
    KdTreeNode(const std::shared_ptr<BoundingBox> bbox, const std::shared_ptr<KdTreeNode> leftChild,
               const std::shared_ptr<KdTreeNode> rightChild, const std::shared_ptr<Primitive> data) :
            bbox{std::move(bbox)},
            leftChild{std::move(leftChild)},
            rightChild{std::move(rightChild)},
            data{std::move(data)} {
    }

    std::shared_ptr<BoundingBox> bbox;
    std::shared_ptr<KdTreeNode> leftChild;
    std::shared_ptr<KdTreeNode> rightChild;
    std::shared_ptr<Primitive> data;
};

class KdTree {
public:
    KdTree(const std::vector<std::shared_ptr<Primitive>> primitives) :
            root(build(primitives, 0)) {
    }

    std::tuple<float, std::weak_ptr<Primitive>> intersect(const Ray &ray, unsigned int &counter) const {
        auto m = std::weak_ptr<Primitive>{};
        auto minDistance = std::numeric_limits<float>::max();
        auto intersection = std::tuple<bool, float>{false, minDistance};

        auto node = std::make_shared<KdTreeNode>(root);
        auto stack = std::stack<std::shared_ptr<KdTreeNode>>{};
        stack.push(node);

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

        return std::make_tuple(std::get<1>(intersection), m);
    }
private:
    KdTreeNode root;

    KdTreeNode build(std::vector<std::shared_ptr<Primitive>> primitives, const int depth) {
        auto node = KdTreeNode{nullptr, nullptr, nullptr, nullptr};

        if (primitives.size() == 1) {
            node.data = primitives[0];
            return node;
        } else if (primitives.size() == 0) {
            return node;
        }

        node.bbox = std::make_shared<BoundingBox>(BoundingBox{primitives});

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

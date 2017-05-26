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
               const std::shared_ptr<KdTreeNode> rightChild, const std::vector<std::shared_ptr<Primitive>> data) :
            bbox{std::move(bbox)},
            leftChild{std::move(leftChild)},
            rightChild{std::move(rightChild)},
            data{std::move(data)},
            splitLocation{0.0f},
            splitAxis{Primitive::Axis::x},
            isLeaf(false) {
    }

    std::shared_ptr<BoundingBox> bbox;
    std::shared_ptr<KdTreeNode> leftChild;
    std::shared_ptr<KdTreeNode> rightChild;
    std::vector<std::shared_ptr<Primitive>> data;
    float splitLocation;
    Primitive::Axis splitAxis;
    bool isLeaf;
};

class KdTree {
public:
    // TODO: schöner
    KdTree(const std::vector<std::shared_ptr<Primitive>> primitives) :
            root(build(primitives, 0, BoundingBox{primitives}.getMin(), BoundingBox{primitives}.getMax())) {
    }

    std::tuple<float, std::weak_ptr<Primitive>> intersect(const Ray &ray, unsigned int &counter) const {
        auto m = std::weak_ptr<Primitive>{};
        auto intersection = Primitive::Intersection{false, std::numeric_limits<float>::max()};

        auto node = std::make_shared<KdTreeNode>(root);
        auto stack = std::stack<std::shared_ptr<KdTreeNode>>{};
        stack.push(node);

        while(!stack.empty()) {
            node = stack.top();
            stack.pop();
            if (node->isLeaf) {
                for (auto const &primitive : node->data) {
                    auto newIntersection = primitive->intersect(ray, intersection.t);
                    if (newIntersection.result) {
                        m = primitive;
                        intersection = newIntersection;
                        //break; //geht
                    }
                    counter++;
                }
            } else {
                auto intersectionBbox = node->bbox->intersect(ray, node->splitLocation, node->splitAxis);
                if (intersectionBbox.result) {
                    auto near = node->leftChild;
                    auto far = node->rightChild;
                    switch (node->splitAxis) {
                        case Primitive::Axis::x:{
                            near = (node->splitLocation > ray.getOrigin().x) ? node->leftChild : node->rightChild;
                            far = (node->splitLocation > ray.getOrigin().x) ? node->rightChild : node->leftChild;
                            break;
                        }
                        case Primitive::Axis::y:{
                            near = (node->splitLocation > ray.getOrigin().y) ? node->leftChild : node->rightChild;
                            far = (node->splitLocation > ray.getOrigin().y) ? node->rightChild : node->leftChild;
                            break;
                        }
                        case Primitive::Axis::z:{
                            near = (node->splitLocation > ray.getOrigin().z) ? node->leftChild : node->rightChild;
                            far = (node->splitLocation > ray.getOrigin().z) ? node->rightChild : node->leftChild;
                            break;
                        }
                    }

                    if (intersectionBbox.tsplit >= intersectionBbox.tmax || intersectionBbox.tsplit < 0) {
                        if(near)
                        stack.push(near);
                    } else if (intersectionBbox.tsplit <= intersectionBbox.tmin) {
                        if(far)
                        stack.push(far);
                    } else {
                        if(far)
                        stack.push(far);
                        if(near)
                        stack.push(near);
                    }
                }
            }
        }
        return std::make_tuple(intersection.t, m);
    }

private:
    KdTreeNode root;

    KdTreeNode build(std::vector<std::shared_ptr<Primitive>> primitives, const int depth, glm::vec3 min, glm::vec3 max) {
        auto node = KdTreeNode{nullptr, nullptr, nullptr, std::vector<std::shared_ptr<Primitive>>{}};

        if (depth >= 5 || primitives.size() == 1) {
            node.isLeaf = true;
            node.data = primitives;
            return node;
        } else if (primitives.size() == 0) {
            node.isLeaf = true;
            return node;
        }

        node.bbox = std::make_shared<BoundingBox>(BoundingBox{min, max});
        std::cout << glm::to_string(min) << " " << glm::to_string(max) << "\n";

        auto axis = Primitive::Axis(depth % 3);
        node.splitAxis = axis;

        // comparator lambda TODO getExtremes bei jedem vergleich? simpler?
        auto compare = [&axis](std::shared_ptr<Primitive> a, std::shared_ptr<Primitive> b) {
            auto extremesA = a->getExtremes(axis);
            auto extremesB = b->getExtremes(axis);
            auto positionA = extremesA.min + extremesA.max;
            auto positionB = extremesB.min + extremesB.max;
            return positionA < positionB;
        };

        // median, not real median for even number of elements
        auto n = primitives.size() / 2;
        std::nth_element(primitives.begin(), primitives.begin()+n, primitives.end(), compare);

        //correctly split on axis TODO: better
        auto splitPosition = primitives[n]->getExtremes(axis).min;
        node.splitLocation = splitPosition;
        auto leftHalf = std::vector<std::shared_ptr<Primitive>>{};
        auto rightHalf = std::vector<std::shared_ptr<Primitive>>{};

        for (auto const& primitive : primitives) {
            auto extremes = primitive->getExtremes(axis);
            if (extremes.max <= splitPosition) {
                leftHalf.push_back(primitive);
            } else if (extremes.min >= splitPosition) {
                rightHalf.push_back(primitive);
            } else {
                leftHalf.push_back(primitive);
                rightHalf.push_back(primitive);
            }
        }

        auto splitVec = node.bbox->getMax();
        switch (axis) {
            case Primitive::Axis::x: splitVec.x = splitPosition; break;
            case Primitive::Axis::y: splitVec.y = splitPosition; break;
            case Primitive::Axis::z: splitVec.z = splitPosition; break;
        }
        if (leftHalf.size() > 0) {
            node.leftChild = std::make_shared<KdTreeNode>(build(leftHalf, depth + 1, node.bbox->getMin(), splitVec));
        }
        splitVec = node.bbox->getMin();
        switch (axis) {
            case Primitive::Axis::x: splitVec.x = splitPosition; break;
            case Primitive::Axis::y: splitVec.y = splitPosition; break;
            case Primitive::Axis::z: splitVec.z = splitPosition; break;
        }
        if (rightHalf.size() > 0) {
            node.rightChild = std::make_shared<KdTreeNode>(build(rightHalf, depth + 1, splitVec, node.bbox->getMax()));
        }

        return node;
    }
};

#endif //FAST_RAYTRACER_KDTREE_H

//
// Created by Patrick Werner on 30.04.17.
//

#ifndef FAST_RAYTRACER_KDTREE_H
#define FAST_RAYTRACER_KDTREE_H

#include <memory>
#include <utility>
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
    KdTreeNode(std::shared_ptr<BoundingBox> bbox, std::shared_ptr<KdTreeNode> leftChild,
               std::shared_ptr<KdTreeNode> rightChild, std::vector<std::shared_ptr<Primitive>> data) :
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
    explicit KdTree(const std::vector<std::shared_ptr<Primitive>>& primitives) :
            root(build(primitives, 0, BoundingBox{primitives}.getMin(), BoundingBox{primitives}.getMax())) {
    }

    std::tuple<float, std::weak_ptr<Primitive>> intersect(const Ray &ray, unsigned long long &counter) const {
        auto m = std::weak_ptr<Primitive>{};
        auto intersection = Primitive::Intersection{false, std::numeric_limits<float>::max()};

        auto node = std::make_shared<KdTreeNode>(root);
        auto stack = std::stack<std::shared_ptr<KdTreeNode>>{};
        stack.push(node);

        while (!stack.empty()) {
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
                        case Primitive::Axis::x: {
                            near = (node->splitLocation > ray.getOrigin().x) ? node->leftChild : node->rightChild;
                            far = (node->splitLocation > ray.getOrigin().x) ? node->rightChild : node->leftChild;
                            break;
                        }
                        case Primitive::Axis::y: {
                            near = (node->splitLocation > ray.getOrigin().y) ? node->leftChild : node->rightChild;
                            far = (node->splitLocation > ray.getOrigin().y) ? node->rightChild : node->leftChild;
                            break;
                        }
                        case Primitive::Axis::z: {
                            near = (node->splitLocation > ray.getOrigin().z) ? node->leftChild : node->rightChild;
                            far = (node->splitLocation > ray.getOrigin().z) ? node->rightChild : node->leftChild;
                            break;
                        }
                    }

                    if (intersectionBbox.tsplit >= intersectionBbox.tmax || intersectionBbox.tsplit < 0) {
                        if (near)
                            stack.push(near);
                    } else if (intersectionBbox.tsplit <= intersectionBbox.tmin) {
                        if (far)
                            stack.push(far);
                    } else {
                        if (far)
                            stack.push(far);
                        if (near)
                            stack.push(near);
                    }
                }
            }
        }
        return std::make_tuple(intersection.t, m);
    }

private:
    KdTreeNode root;

    KdTreeNode build(const std::vector<std::shared_ptr<Primitive>> &primitives, const int depth, const glm::vec3 min,
                     const glm::vec3 max) const {
        auto node = KdTreeNode{nullptr, nullptr, nullptr, std::vector<std::shared_ptr<Primitive>>{}};

        node.splitAxis = Primitive::Axis(depth % 3);

        node.bbox = std::make_shared<BoundingBox>(BoundingBox{min, max});

        auto nCandidates = 50;
        std::vector<float> candidates = std::vector<float>{};

        switch (node.splitAxis) {
            case Primitive::Axis::x: {
                auto start = node.bbox->getMin().x;
                auto stop = node.bbox->getMax().x;
                for (auto i = 0; i < nCandidates; ++i) {
                    candidates.push_back(start + i * (stop - start) / (nCandidates - 1.0f));
                }
                break;
            }
            case Primitive::Axis::y: {
                auto start = node.bbox->getMin().y;
                auto stop = node.bbox->getMax().y;
                for (auto i = 0; i < nCandidates; ++i) {
                    candidates.push_back(start + i * (stop - start) / (nCandidates - 1.0f));
                }
                break;
            }
            case Primitive::Axis::z: {
                auto start = node.bbox->getMin().z;
                auto stop = node.bbox->getMax().z;
                for (auto i = 0; i < nCandidates; ++i) {
                    candidates.push_back(start + i * (stop - start) / (nCandidates - 1.0f));
                }
                break;
            }
        }

        auto bestSplitCost = std::numeric_limits<float>::max();
        auto costTrav = 0.3f;
        auto costIntersect = 1.0f;
        auto w = abs(node.bbox->getMax().x - node.bbox->getMin().x);
        auto h = abs(node.bbox->getMax().z - node.bbox->getMin().z);
        auto d = abs(node.bbox->getMax().y - node.bbox->getMin().y);
        auto area = 2 * (w * h + w * d + d * h);
        auto cost = costTrav * costIntersect * area * primitives.size();

        for (auto const &candidate : candidates) {
            auto leftHalf = std::vector<std::shared_ptr<Primitive>>{};
            auto rightHalf = std::vector<std::shared_ptr<Primitive>>{};
            for (auto const &primitive : primitives) {
                auto extremes = primitive->getExtremes(node.splitAxis);
                if (extremes.max <= candidate) {
                    leftHalf.push_back(primitive);
                } else if (extremes.min >= candidate) {
                    rightHalf.push_back(primitive);
                } else {
                    leftHalf.push_back(primitive);
                    rightHalf.push_back(primitive);
                }
            }

            auto leftW = abs(node.bbox->getMax().x - node.bbox->getMin().x);
            auto leftD = abs(node.bbox->getMax().y - node.bbox->getMin().y);
            auto leftH = abs(node.bbox->getMax().z - node.bbox->getMin().z);
            auto rightW = abs(node.bbox->getMax().x - node.bbox->getMin().x);
            auto rightD = abs(node.bbox->getMax().y - node.bbox->getMin().y);
            auto rightH = abs(node.bbox->getMax().z - node.bbox->getMin().z);

            switch (node.splitAxis) {
                case Primitive::Axis::x: {
                    leftW = abs(candidate - node.bbox->getMin().x);
                    rightW = abs(node.bbox->getMax().x - candidate);
                    break;
                }
                case Primitive::Axis::y: {
                    leftD = abs(candidate - node.bbox->getMin().y);
                    rightD = abs(node.bbox->getMax().y - candidate);
                    break;
                }
                case Primitive::Axis::z: {
                    leftH = abs(candidate - node.bbox->getMin().z);
                    rightH = abs(node.bbox->getMax().z - candidate);
                    break;
                }
            }

            auto leftArea = 2 * (leftW * leftH + leftW * leftD + leftD * leftH);
            auto rightArea = 2 * (rightW * rightH + rightW * rightD + rightD * rightH);
            auto leftCost = costTrav * costIntersect * leftArea * leftHalf.size();
            auto rightCost = costTrav * costIntersect * rightArea * rightHalf.size();
            auto splitCost = leftCost + rightCost;
            if (bestSplitCost > splitCost) {
                bestSplitCost = splitCost;
                node.splitLocation = candidate;
            }
        }

        auto leftHalf = std::vector<std::shared_ptr<Primitive>>{};
        auto rightHalf = std::vector<std::shared_ptr<Primitive>>{};
        for (auto const &primitive : primitives) {
            auto extremes = primitive->getExtremes(node.splitAxis);
            if (extremes.max <= node.splitLocation) {
                leftHalf.push_back(primitive);
            } else if (extremes.min >= node.splitLocation) {
                rightHalf.push_back(primitive);
            } else {
                leftHalf.push_back(primitive);
                rightHalf.push_back(primitive);
            }
        }

        if (bestSplitCost < cost) {
            auto splitVec = node.bbox->getMax();
            switch (node.splitAxis) {
                case Primitive::Axis::x:
                    splitVec.x = node.splitLocation;
                    break;
                case Primitive::Axis::y:
                    splitVec.y = node.splitLocation;
                    break;
                case Primitive::Axis::z:
                    splitVec.z = node.splitLocation;
                    break;
            }
            if (!leftHalf.empty()) {
                node.leftChild = std::make_shared<KdTreeNode>(
                        build(leftHalf, depth + 1, node.bbox->getMin(), splitVec));
            }
            splitVec = node.bbox->getMin();
            switch (node.splitAxis) {
                case Primitive::Axis::x:
                    splitVec.x = node.splitLocation;
                    break;
                case Primitive::Axis::y:
                    splitVec.y = node.splitLocation;
                    break;
                case Primitive::Axis::z:
                    splitVec.z = node.splitLocation;
                    break;
            }
            if (!rightHalf.empty()) {
                node.rightChild = std::make_shared<KdTreeNode>(
                        build(rightHalf, depth + 1, splitVec, node.bbox->getMax()));
            }
        } else {
            node.isLeaf = true;
            node.data = primitives;
        }

        return node;
    }
};

#endif //FAST_RAYTRACER_KDTREE_H

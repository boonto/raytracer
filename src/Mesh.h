//
// Created by patrick on 27.04.17.
//

#ifndef FAST_RAYTRACER_MESH_H
#define FAST_RAYTRACER_MESH_H

#include "Triangle.h"

class Mesh {
public:
    //TODO primitive!
    std::vector<Triangle> triangles;

    Mesh(const std::vector<Triangle> triangles) : triangles{std::move(triangles)} {
    }
};


#endif //FAST_RAYTRACER_MESH_H

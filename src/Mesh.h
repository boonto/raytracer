//
// Created by Patrick Werner on 27.04.17.
//

#ifndef FAST_RAYTRACER_MESH_H
#define FAST_RAYTRACER_MESH_H

#include <utility>

#include "Primitive.h"

class Mesh {
public:
    explicit Mesh(std::vector<std::shared_ptr<Primitive>>  primitives) : primitives{std::move(primitives)} {
    }

    //const retval?
    std::vector<std::shared_ptr<Primitive>> getPrimitives() const {
        return primitives;
    }

private:
    std::vector<std::shared_ptr<Primitive>> primitives;
};


#endif //FAST_RAYTRACER_MESH_H

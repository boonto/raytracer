cmake_minimum_required(VERSION 3.7)
project(fast_raytracer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Weffc++ -pedantic")
set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES src/main.cpp src/Material.h src/Ray.h src/Model.h src/PointLight.h src/Plane.cpp src/Plane.h src/Sphere.cpp src/Sphere.h src/Camera.h src/Window.h src/Scene.h src/Mesh.h src/ObjModel.h src/Triangle.h src/KdTree.h src/BoundingBox.h src/Primitive.h)
include(FindPkgConfig)

pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(GLM REQUIRED glm)
pkg_search_module(ASSIMP REQUIRED assimp)

include_directories(${SDL2_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS} ${ASSIMP_INCLUDE_DIRS})

add_executable(fast_raytracer ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${ASSIMP_LIBRARIES})
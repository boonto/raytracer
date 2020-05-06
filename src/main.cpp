//
// Created by Patrick Werner on 30.04.17.
//

#include <iostream>
#include <chrono>
#include "Window.h"
#include "Scene.h"
#include "Raytracer.h"

// TODO: pointer to const retvals?

const unsigned int WIDTH = 2048;
const unsigned int HEIGHT = 2048;

#undef main

int main() {
    auto t0 = std::chrono::high_resolution_clock::now();

    Window window{glm::uvec2{WIDTH, HEIGHT}};
    auto scene = Scene{glm::uvec2{WIDTH, HEIGHT}};

    auto raytracer = Raytracer{std::make_shared<Scene>(scene)};

    auto t1 = std::chrono::high_resolution_clock::now();
    std::cout << "Init: " << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count()
              << " ms\n";

    auto counter = raytracer.render(window);

    auto t2 = std::chrono::high_resolution_clock::now();
    std::cout << "Render: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()
              << " ms\n";

    std::cout << "Intersections checked: " << counter << "\n";
    window.setTitle(std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()) + " ms");
    window.render();


    return EXIT_SUCCESS;
}

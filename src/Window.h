//
// Created by Patrick Werner on 21.04.17.
//

#ifndef FAST_RAYTRACER_WINDOW_H
#define FAST_RAYTRACER_WINDOW_H

#include <memory>
#include "SDL2/SDL.h"
#include "SDL2/SDL_test_images.h"
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"

class Window {
    struct SDL_Deleter {
        void operator()(SDL_Renderer *ptr) { if (ptr) SDL_DestroyRenderer(ptr); }

        void operator()(SDL_Window *ptr) { if (ptr) SDL_DestroyWindow(ptr); }
    };

public:
    explicit Window(const glm::uvec2 resolution) :
            event{},
            window{nullptr},
            renderer{nullptr} {
        SDL_Init(SDL_INIT_VIDEO);

        SDL_Window *pWindow;
        SDL_Renderer *pRenderer;

        SDL_CreateWindowAndRenderer(resolution.x, resolution.y, 0, &pWindow, &pRenderer);

        window = std::unique_ptr<SDL_Window, SDL_Deleter>{pWindow};
        renderer = std::unique_ptr<SDL_Renderer, SDL_Deleter>{pRenderer};

        SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 0);
        SDL_RenderClear(renderer.get());
    }

    ~Window() {
        SDL_Quit();
    }

    void putPixel(const int x, const int y, const glm::vec3 &color) {
        SDL_SetRenderDrawColor(renderer.get(), static_cast<Uint8>(color.r * 255.0f),
                               static_cast<Uint8>(color.g * 255.0f), static_cast<Uint8>(color.b * 255.0f), 255);
        SDL_RenderDrawPoint(renderer.get(), x, y);
    }

    void render() {
        SDL_RenderPresent(renderer.get());
        saveTexture("test.bmp");
        while (true) {
            if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
                break;
        }
    }

    void setTitle(const std::string& title) {
        SDL_SetWindowTitle(window.get(), title.data());
    }

    void saveTexture(const char *filename) {
        int w, h;
        SDL_GetWindowSize(window.get(), &w, &h);
        SDL_Surface *sshot = SDL_CreateRGBSurface(0, w, h, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
        SDL_RenderReadPixels(renderer.get(), nullptr, SDL_PIXELFORMAT_ARGB8888, sshot->pixels, sshot->pitch);
        SDL_SaveBMP(sshot, "screenshot.bmp");
        SDL_FreeSurface(sshot);
    }

private:
    SDL_Event event;
    std::unique_ptr<SDL_Window, SDL_Deleter> window;
    std::unique_ptr<SDL_Renderer, SDL_Deleter> renderer;
};

#endif //FAST_RAYTRACER_WINDOW_H

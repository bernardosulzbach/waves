// Using trigonometric functions, draws waves on the screen.
// Written by Bernardo Sulzbach in 2016 and licensed under the BSD 2-Clause.

#include "SDL.h"

#include <stdio.h>
#include <math.h>

const Uint32 WIDTH = 800;
const Uint32 HEIGHT = 800;

/**
 * Squares a number.
 */
double square(double a) {
    return a * a;
}

double distance(double x1, double y1, double x2, double y2) {
    return sqrt(square(x2 - x1) + square(y2 - y1));
}

double distance_from_center(int x, int y, int width, int height) {
    return distance(x, y, width / 2, height / 2);
}

SDL_Surface *get_empty_surface(Uint32 width, Uint32 height) {
    return SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
}

void write_waves(SDL_Window *window) {
    SDL_Renderer *renderer;
    renderer = SDL_CreateRenderer(window, -1, 0);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            double dist = distance_from_center(x, y, WIDTH, HEIGHT);
            double magic = sin(dist) + 1.0; // Translate the image to [0, 2]
            Uint8 normalized = (Uint8) (255.0 * magic / 2.0);
            SDL_RenderDrawPoint(renderer, x, y);
            SDL_SetRenderDrawColor(renderer, normalized, normalized, normalized, 0);
        }
    }
    SDL_RenderPresent(renderer);
    SDL_DestroyRenderer(renderer);
}


int main(int argc, char* argv[]) {
    SDL_Window *window;                   
    SDL_Init(SDL_INIT_VIDEO);              
    window = SDL_CreateWindow(
            "Waves",
            SDL_WINDOWPOS_CENTERED,        
            SDL_WINDOWPOS_CENTERED,         
            WIDTH,
            HEIGHT,                           
            SDL_WINDOW_OPENGL);
    // Check that the window was successfully created
    if (window == NULL) {
        // In the case that the window could not be made, write about it.
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    } else {
        // Write waves to the window.
        write_waves(window);
        SDL_Event event;
        // The window is open, therefore we enter the program loop.
        unsigned int running = 1;
        while (running) {
            while (SDL_PollEvent(&event) != 0) {
                if (event.type == SDL_QUIT) {
                    running = 0;
                }
            }
        }

        // Clean up
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
    return 0;
}

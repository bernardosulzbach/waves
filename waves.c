// Using trigonometric functions, draws waves on the screen.
// Written by Bernardo Sulzbach in 2016 and licensed under the BSD 2-Clause.

#include "SDL.h"

#include <stdio.h>
#include <math.h>
#include <time.h>

const int WIDTH = 480;
const int HEIGHT = 480;

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

typedef struct Point {
    int x;
    int y;
} Point;

typedef struct Oscillator {
    Point center;
} Oscillator;

typedef struct Universe {
    Oscillator *oscillator;
} Universe;

/**
 * Creates a Universe.
 */
Universe *create_universe() {
    Universe *universe = malloc(sizeof(Universe));
    Oscillator *oscillator = malloc(sizeof(Oscillator));
    Point origin = {0, 0};
    oscillator->center = origin;
    universe->oscillator = oscillator;
    return universe;
}

void write_waves(SDL_Window *window, SDL_Renderer *renderer, const Universe * const universe) {
    clock_t start = clock();
    printf("Started writing waves.\n");

    SDL_SetRenderDrawColor(renderer, 127, 127, 127, 0);
    SDL_RenderClear(renderer);

    // The array used for calculations.
    double intensities[HEIGHT][WIDTH];
    for (size_t i = 0; i < HEIGHT; i++) {
        for (size_t j = 0; j < WIDTH; j++) {
            intensities[i][j] = 0.0;
        }
    }

    for (size_t o_index = 0; o_index < 1; o_index++) {
        for (int x = -WIDTH / 2; x < WIDTH / 2; x++) {
            for (int y = -HEIGHT / 2; y < HEIGHT / 2; y++) {
                Oscillator *osc = universe->oscillator;
                Point center = osc->center;
                int center_x = center.x;
                int center_y = center.y;
                double dist = distance(x, y, center_x, center_y);
                double magic = sin(dist) + 1.0; // Translate the image to [0, 2]
                int array_x = x + WIDTH / 2;
                int array_y = y + HEIGHT / 2;
                intensities[array_x][array_y] = 255.0 * magic / 2.0;
            }
        }
    }

    int ms = (clock() - start) * 1000 / CLOCKS_PER_SEC;
    printf("Took %d ms to recompute.\n", ms);

    for (size_t i = 0; i < HEIGHT; i++) {
        for (size_t j = 0; j < WIDTH; j++) {
            Uint8 normalized = (Uint8) intensities[i][j];
            SDL_SetRenderDrawColor(renderer, normalized, normalized, normalized, 0);
            SDL_RenderDrawPoint(renderer, i, j);
        }
    }

    ms = (clock() - start) * 1000 / CLOCKS_PER_SEC;
    printf("Took %d ms to redraw.\n", ms);

    SDL_RenderPresent(renderer);
}

/**
 * Handles a KEYDOWN event.
 */
void handle_keydown(Universe *universe, SDL_Event event) {
    const SDL_Keycode sym = event.key.keysym.sym;
    if (sym == SDLK_UP) {
        universe->oscillator->center.y--;
    } else if (sym == SDLK_RIGHT) {
        universe->oscillator->center.x++;
    } else if (sym == SDLK_DOWN) {
        universe->oscillator->center.y++;
    } else if (sym == SDLK_LEFT) {
        universe->oscillator->center.x--;
    }
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
        SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
        // Write waves to the window.
        Universe *universe = create_universe();
        write_waves(window, renderer, universe);
        SDL_Event event;
        // The window is open, therefore we enter the program loop.
        unsigned int running = 1;
        while (running) {
            while (SDL_PollEvent(&event) != 0) {
                if (event.type == SDL_QUIT) {
                    running = 0;
                }
                else if (event.type == SDL_KEYDOWN) {
                    handle_keydown(universe, event);
                    write_waves(window, renderer, universe);
                }
            }
        }

        // Clean up
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
    return 0;
}

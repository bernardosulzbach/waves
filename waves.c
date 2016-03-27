// Using trigonometric functions, draws waves on the screen.
// Written by Bernardo Sulzbach in 2016 and licensed under the BSD 2-Clause.

#include "SDL.h"

#include <stdio.h>
#include <math.h>
#include <time.h>

const double TAU = 2 * M_PI;

/**
 * The width of the window, in pixels.
 */
const int WIDTH = 640;

/**
 * The height of the window, in pixels.
 */
const int HEIGHT = 480;

/**
 * The maximum number of oscillators.
 *
 * Currently 10 so that each key from 1 to 0 match to one oscillator.
 */
const int MAXIMUM_OSCILLATORS = 10;

const double MINIMUM_AMPLITUDE = 0.1;
const double DEFAULT_AMPLITUDE = 1.0;
const double MAXIMUM_AMPLITUDE = 2.0;

const double AMPLITUDE_TICK = 0.1;

const double DEFAULT_WAVELENGTH = 40.0;

typedef struct Point {
    int x;
    int y;
} Point;

const Point ORIGIN = {0, 0};

typedef struct Oscillator {
    Point center;
    double amplitude;
    double wavelength;
} Oscillator;

typedef struct Universe {
    Oscillator **oscillators;
} Universe;

typedef enum HighlightMode {
    HIGHLIGHT_NONE, HIGHLIGHT_DOT
} HighlightMode;

/**
 * The structure responsible for modifying an Universe from user input.
 */
typedef struct Controller {
    Universe *universe;
    size_t selection;
    HighlightMode highlight;
} Controller;

double minimum(double a, double b) {
    return a < b ? a : b;
}

double maximum(double a, double b) {
    return a > b ? a : b;
}

/**
 * Squares a number.
 */
double square(double a) {
    return a * a;
}

double distance(double x1, double y1, double x2, double y2) {
    return sqrt(square(x2 - x1) + square(y2 - y1));
}

SDL_Surface *get_empty_surface(Uint32 width, Uint32 height) {
    return SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
}

/**
 * Creates an oscillator at the origin.
 */
Oscillator *create_oscillator() {
    Oscillator *oscillator = malloc(sizeof(Oscillator));
    oscillator->center = ORIGIN;
    oscillator->amplitude = DEFAULT_AMPLITUDE;
    oscillator->wavelength = DEFAULT_WAVELENGTH;
    return oscillator;
}

void delete_oscillator(Oscillator *oscillator) {
    free(oscillator);
}

/**
 * Creates a Universe.
 */
Universe *create_universe() {
    Universe *universe = malloc(sizeof(Universe));
    Oscillator **oscillators = malloc(MAXIMUM_OSCILLATORS * sizeof(Oscillator));
    oscillators[0] = create_oscillator();
    for (int i = 1; i < MAXIMUM_OSCILLATORS; i++) {
        oscillators[i] = NULL;
    }
    universe->oscillators = oscillators;
    return universe;
}

Controller *create_controller(Universe *universe) {
    Controller *controller = malloc(sizeof(Controller));
    controller->universe = universe;
    controller->selection = 0;
    controller->highlight = HIGHLIGHT_DOT;
    return controller;
}

void write_waves(SDL_Window *window, SDL_Renderer *renderer, const Controller * const controller, const Universe * const universe) {
    clock_t start = clock();

    SDL_SetRenderDrawColor(renderer, 127, 127, 127, 0);
    SDL_RenderClear(renderer);

    // The matrix used for the calculations.
    double intensities[HEIGHT][WIDTH];
    // Initialize the array to 0.
    for (size_t i = 0; i < HEIGHT; i++) {
        for (size_t j = 0; j < WIDTH; j++) {
            intensities[i][j] = 0.0;
        }
    }

    // Calculate all values of the matrix.
    for (unsigned int index = 0; index < MAXIMUM_OSCILLATORS; index++) {
        if (universe->oscillators[index] != NULL) {
            const Oscillator *osc = universe->oscillators[index];
            Point center = osc->center;
            const int center_x = center.x;
            const int center_y = center.y;
            for (int x = -WIDTH / 2; x < WIDTH / 2; x++) {
                for (int y = -HEIGHT / 2; y < HEIGHT / 2; y++) {
                    const double dist = distance(x, y, center_x, center_y);
                    const double wave = sin(dist * TAU / osc->wavelength);
                    const double normalized = osc->amplitude * (wave + 1.0) / 2.0;
                    int array_x = x + WIDTH / 2;
                    int array_y = y + HEIGHT / 2;
                    intensities[array_y][array_x] += normalized;
                }
            }
            printf("Evaluated Oscillator #%d\n", index + 1);
        }
    }

    int ms = (clock() - start) * 1000 / CLOCKS_PER_SEC;
    printf("Took %d ms to recompute.", ms);

    double maximum_intensity = 0.0;
    for (size_t i = 0; i < HEIGHT; i++) {
        for (size_t j = 0; j < WIDTH; j++) {
            if (intensities[i][j] > maximum_intensity) {
                maximum_intensity = intensities[i][j];
            }
        }
    }

    for (size_t i = 0; i < HEIGHT; i++) {
        for (size_t j = 0; j < WIDTH; j++) {
            Uint8 normalized = (Uint8) (255 * (intensities[i][j] / maximum_intensity));
            SDL_SetRenderDrawColor(renderer, normalized, normalized, normalized, 0);
            SDL_RenderDrawPoint(renderer, j, i);
        }
    }

    if (controller->highlight == HIGHLIGHT_DOT) {
        // Make a red dot for each Oscillator.
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);
        for (unsigned int index = 0; index < MAXIMUM_OSCILLATORS; index++) {
            if (universe->oscillators[index] != NULL) {
                Point center = universe->oscillators[index]->center;
                SDL_RenderDrawPoint(renderer, center.x + WIDTH / 2, center.y + HEIGHT / 2);
            }
        }
    }

    ms = (clock() - start) * 1000 / CLOCKS_PER_SEC;
    printf(" Took %d ms to redraw.\n", ms);

    SDL_RenderPresent(renderer);
}

Oscillator *get_controller_oscillator(Controller *controller) {
    return controller->universe->oscillators[controller->selection];
}

void controller_move_up(Controller *controller) {
    get_controller_oscillator(controller)->center.y--;
}

void controller_move_left(Controller *controller) {
    get_controller_oscillator(controller)->center.x--;
}

void controller_move_down(Controller *controller) {
    get_controller_oscillator(controller)->center.y++;
}

void controller_move_right(Controller *controller) {
    get_controller_oscillator(controller)->center.x++;
}

void controller_increase_amplitude(Controller *controller) {
    Oscillator *oscillator = get_controller_oscillator(controller);
    oscillator->amplitude = minimum(oscillator->amplitude + AMPLITUDE_TICK, MAXIMUM_AMPLITUDE);
}

void controller_decrease_amplitude(Controller *controller) {
    Oscillator *oscillator = get_controller_oscillator(controller);
    oscillator->amplitude = maximum(oscillator->amplitude - AMPLITUDE_TICK, MINIMUM_AMPLITUDE);
}

void controller_select(Controller *controller, size_t target) {
    controller->selection = target;
    if (get_controller_oscillator(controller) == NULL) {
        controller->universe->oscillators[controller->selection] = create_oscillator();
    }
}

void controller_delete(Controller *controller) {
    Oscillator *oscillator = get_controller_oscillator(controller);
    delete_oscillator(oscillator);
    controller->universe->oscillators[controller->selection] = NULL;
    // Select another Oscillator to prevent a segmentation fault.
    // A flag indicating whether or not another Oscillator could be found.
    int found_oscillator = 0;
    for (unsigned int index = 0; index < MAXIMUM_OSCILLATORS && !found_oscillator; index++) {
        if (controller->universe->oscillators[index] != NULL) {
            controller->selection = index;
            found_oscillator = 1;
        }
    }
    // If we couldn't find another Oscillator, we create a new one with controller_select().
    if (!found_oscillator) {
        controller_select(controller, 0);
    }
}

/**
 * Handles a KEYDOWN event.
 *
 * Returns 0 if this function call didn't change anything.
 */
int handle_keydown(Controller *controller, SDL_Event event) {
    const SDL_Keycode sym = event.key.keysym.sym;
    if (sym == SDLK_UP) {
        controller_move_up(controller);
    } else if (sym == SDLK_RIGHT) {
        controller_move_right(controller);
    } else if (sym == SDLK_DOWN) {
        controller_move_down(controller);
    } else if (sym == SDLK_LEFT) {
        controller_move_left(controller);
    } else if (sym >= SDLK_1 && sym <= SDLK_8) {
        controller_select(controller, sym - SDLK_1);
    } else if (sym == SDLK_DELETE) {
        controller_delete(controller);
    } else if (sym == SDLK_KP_PLUS) {
        controller_increase_amplitude(controller);
    } else if (sym == SDLK_KP_MINUS) {
        controller_decrease_amplitude(controller);
    } else {
        return 0;
    }
    return 1;
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
        Controller *controller = create_controller(universe);
        write_waves(window, renderer, controller, universe);
        SDL_Event event;
        // The window is open, therefore we enter the program loop.
        unsigned int running = 1;
        while (running) {
            while (SDL_PollEvent(&event) != 0) {
                if (event.type == SDL_QUIT) {
                    running = 0;
                }
                else if (event.type == SDL_KEYDOWN) {
                    if (handle_keydown(controller, event)) {
                        write_waves(window, renderer, controller, universe);
                    }
                }
            }
        }

        // Clean up
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
    return 0;
}

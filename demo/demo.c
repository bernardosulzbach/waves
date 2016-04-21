// Using trigonometric functions, draws waves on the screen.
//
// Written by Bernardo Sulzbach in 2016 and licensed under the BSD 2-Clause.

#include "SDL.h"

#include <math.h>
#include <stdio.h>
#include <time.h>

#include "cached-geometry.h"
#include "constants.h"
#include "geometry.h"

#define DISSIPATION_START 10.0

/**
 * The width of the window, in pixels.
 */
#define WIDTH 500

/**
 * The height of the window, in pixels.
 */
#define HEIGHT 500

#define FRAMES_PER_SEC 10

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

typedef enum DissipationModel {
    NO_DISSIPATION,
    INVERSE_LINEAR_DISSIPATION,
    INVERSE_SQUARE_DISSIPATION,
    NUMBER_OF_DISSIPATION_MODELS // Helper value
} DissipationModel;

/**
 * Returns a human-readable string for a DissipationModel value.
 */
char *dissipation_model_to_string(DissipationModel model) {
    if (model == NO_DISSIPATION) {
        return "no dissipation";
    } else if (model == INVERSE_LINEAR_DISSIPATION) {
        return "inverse linear";
    } else if (INVERSE_SQUARE_DISSIPATION) {
        return "inverse square";
    } else {
        return "unknown";
    }
}

typedef struct Universe {
    Uint16 width;
    Uint16 height;
    double **value_matrix;
    DissipationModel dissipation_model;
    Oscillator **oscillators;
} Universe;

const DissipationModel DEFAULT_UNIVERSE_DISSIPATION_MODEL = NO_DISSIPATION;

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
    int rendering; // Whether or not we are rendering.
} Controller;

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
Universe *create_universe(const Uint16 width, const Uint16 height) {
    Universe *universe = malloc(sizeof(Universe));

    universe->width = width;
    universe->height = height;

    // Initialize the value matrix
    universe->value_matrix = malloc(height * sizeof(double*));
    if (universe->value_matrix) {
        for (Uint16 y = 0; y < height; y++) {
            universe->value_matrix[y] = malloc(width * sizeof(double));
        }
    }

    // Initialize the Oscillators
    Oscillator **oscillators = malloc(MAXIMUM_OSCILLATORS * sizeof(Oscillator));
    oscillators[0] = create_oscillator();
    for (int i = 1; i < MAXIMUM_OSCILLATORS; i++) {
        oscillators[i] = NULL;
    }
    universe->oscillators = oscillators;
    universe->dissipation_model = DEFAULT_UNIVERSE_DISSIPATION_MODEL;
    return universe;
}

Controller *create_controller(Universe *universe) {
    Controller *controller = malloc(sizeof(Controller));
    controller->universe = universe;
    controller->selection = 0;
    controller->highlight = HIGHLIGHT_DOT;
    controller->rendering = 1;
    return controller;
}

void reset_universe_value_matrix(const Universe * const universe) {
    for (Uint16 y = 0; y < universe->height; y++) {
        for (Uint16 x = 0; x < universe->width; x++) {
            universe->value_matrix[y][x] = 0.0;
        }
    }
}

double dissipate(double value, double distance, DissipationModel model) {
    if (model == NO_DISSIPATION) {
        return value;
    } else if (model == INVERSE_LINEAR_DISSIPATION) {
        return DISSIPATION_START * value / maximum(DISSIPATION_START, distance);
    } else {
        return DISSIPATION_START * value / square(maximum(DISSIPATION_START, distance)); // No need to ensure positiveness.
    }
}

void write_waves(SDL_Window *window, SDL_Renderer *renderer, const Controller * const controller, const Universe * const universe) {
    clock_t start = clock();
    int ms;
    if (controller->rendering) {
        reset_universe_value_matrix(universe);

        // Calculate all values of the matrix.
        for (unsigned int index = 0; index < MAXIMUM_OSCILLATORS; index++) {
            if (universe->oscillators[index] != NULL) {
                const Oscillator *osc = universe->oscillators[index];
                const Point center = osc->center;
                const int center_x = center.x;
                const int center_y = center.y;
                for (int x = -WIDTH / 2; x < WIDTH / 2; x++) {
                    for (int y = -HEIGHT / 2; y < HEIGHT / 2; y++) {
                        const double wave_value = sin_of_distance(x - center_x, y - center_y, osc->wavelength);
                        const double amplitude = (wave_value + 1.0) / 2.0;
                        // If the model is NO_DISSIPATION, distance_to_center is useless. However, I think GCC removes it then.
                        const double distance_to_center = distance_to_origin(x - center_x, y - center_y);
                        const double after_dissipation = dissipate(amplitude, distance_to_center, universe->dissipation_model);
                        const int array_x = x + WIDTH / 2;
                        const int array_y = y + HEIGHT / 2;
                        universe->value_matrix[array_y][array_x] += after_dissipation;
                    }
                }
                printf("Evaluated Oscillator #%d\n", index + 1);
            }
        }

        ms = (clock() - start) * 1000 / CLOCKS_PER_SEC;
        printf("Took %d ms to recompute.\n", ms);
        start = clock();
    }

    double maximum_intensity = 0.0;
    for (size_t i = 0; i < HEIGHT; i++) {
        for (size_t j = 0; j < WIDTH; j++) {
            if (universe->value_matrix[i][j] > maximum_intensity) {
                maximum_intensity = universe->value_matrix[i][j];
            }
        }
    }

    for (size_t i = 0; i < HEIGHT; i++) {
        for (size_t j = 0; j < WIDTH; j++) {
            Uint8 normalized = (Uint8) (255 * (universe->value_matrix[i][j] / maximum_intensity));
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
    printf("Took %d ms to redraw.\n", ms);

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

void controller_toggle_rendering(Controller *controller) {
    controller->rendering = controller->rendering ? 0 : 1;
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
    } else if (sym == SDLK_r) {
        controller_toggle_rendering(controller);
    } else if (sym == SDLK_d) {
        const DissipationModel old = controller->universe->dissipation_model;
        const DissipationModel new = (old + 1) % NUMBER_OF_DISSIPATION_MODELS;
        controller->universe->dissipation_model = new;
        const char *old_as_string = dissipation_model_to_string(old);
        const char *new_as_string = dissipation_model_to_string(new);
        printf("Toggled from '%s' to '%s'\n", old_as_string, new_as_string);
    } else {
        return 0;
    }
    return 1;
}

int main(int argc, char* argv[]) {
    init_cached_geometry();
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
        Universe *universe = create_universe(WIDTH, HEIGHT);
        Controller *controller = create_controller(universe);
        write_waves(window, renderer, controller, universe);
        SDL_Event event;
        // The window is open, therefore we enter the program loop.
        unsigned int running = 1;
        clock_t last_rendering = clock();
        while (running) {
            while (SDL_PollEvent(&event) != 0) {
                if (event.type == SDL_QUIT) {
                    running = 0;
                }
                else if (event.type == SDL_KEYDOWN) {
                    if (handle_keydown(controller, event)) {
                        const clock_t current_time = clock();
                        // Casting CPU time values to double makes sure that
                        // arithmetic operations work properly and consistently
                        // no matter what the underlying representation is.
                        const double seconds = ((double) (current_time - last_rendering)) / CLOCKS_PER_SEC;
                        if (seconds * FRAMES_PER_SEC >= 1) {
                            write_waves(window, renderer, controller, universe);
                            last_rendering = clock();
                        }
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

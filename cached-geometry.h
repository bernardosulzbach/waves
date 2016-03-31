// Fast calculation of geometric functions.
//
// The SIN_OF_DISTANCE_CACHE only covers the default wavelength.
//
// Written by Bernardo Sulzbach in 2016 and licensed under the BSD 2-Clause.

#pragma once

#include <math.h>
#include <stdio.h>

#include "constants.h"
#include "geometry.h"
#include "logger.h"

// The biggest value N such that (n, n) is in the cache.
#define SIN_OF_DISTANCE_CACHE_MAXIMUM 640

double SIN_OF_DISTANCE_CACHE[SIN_OF_DISTANCE_CACHE_MAXIMUM + 1][SIN_OF_DISTANCE_CACHE_MAXIMUM + 1];

double distance_to_origin(int x, int y) {
    return sqrt(square(x) + square(y));
}

double evaluate_sin_of_distance(int x, int y, double wavelength) {
    return sin(distance_to_origin(x, y) * TAU / wavelength);
}

double fetch_sin_of_distance(int x, int y) {
    return SIN_OF_DISTANCE_CACHE[y][x];
}

double sin_of_distance(int x, int y, double wavelength) {
    // Make both values absolute
    x = abs(x);
    y = abs(y);
    if (wavelength == DEFAULT_WAVELENGTH &&
            x <= SIN_OF_DISTANCE_CACHE_MAXIMUM &&
            y <= SIN_OF_DISTANCE_CACHE_MAXIMUM) {
        return fetch_sin_of_distance(x, y);
    } else {
        char *message;
        // asprintf() returns the number of bytes written to the string.
        if (asprintf(&message, "Failed to fetch (%d, %d) from the cache.", x, y) > 0) {
            log_message(1, message);
            free(message);
        }
        return evaluate_sin_of_distance(x, y, wavelength);
    }
}

void init_sin_of_distance() {
    for (int y = 0; y < SIN_OF_DISTANCE_CACHE_MAXIMUM; y++) {
        for (int x = 0; x < SIN_OF_DISTANCE_CACHE_MAXIMUM; x++) {
            SIN_OF_DISTANCE_CACHE[y][x] = evaluate_sin_of_distance(x, y, DEFAULT_WAVELENGTH);
        }
    }
}

/**
 * A function that must be called in order to initialize the caches of the
 * cached geometric utilities.
 */
void init_cached_geometry() {
    init_sin_of_distance();
}

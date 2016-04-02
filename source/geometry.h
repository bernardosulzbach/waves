// Helper geometric functions.
//
// Written by Bernardo Sulzbach in 2016 and licensed under the BSD 2-Clause.

#pragma once

#include <math.h>

#include "constants.h"

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

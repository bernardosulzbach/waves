// The minimalist logger used by the project.
//
// Written by Bernardo Sulzbach in 2016 and licensed under the BSD 2-Clause.

#pragma once

#include <stdio.h>

/**
 * Logs the provided message to the log file with the specified badness level.
 *
 * This function is somewhat slow so it shouldn't be used inside loops.
 *
 * This function returns 0 if the write succeeded.
 */
int log_message(short badness, char *message) {
    FILE *log_file = fopen("log.txt", "a");
    if (log_file == NULL) {
        printf("Could not open the log file!\n");
    } else {
        fprintf(log_file, "Badness %4d: %s\n", badness, message);
        fclose(log_file);
        return 0;
    }
    return 1;
}

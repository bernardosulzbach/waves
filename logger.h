// The minimalist logger used by the project.
//
// Written by Bernardo Sulzbach in 2016 and licensed under the BSD 2-Clause.

#pragma once

#include <stdio.h>
#include <string.h>

int validate_tags(char **tags, size_t tag_count) {
    return 1;
}

/**
 * Merge an array of tags into a single string.
 */
char *merge_tags(char **tags, size_t tag_count) {
    if (tags == NULL) {
        return "";
    }
    if (!validate_tags(tags, tag_count)) {
        return "INVALID_TAGS";
    }
    const size_t separator_length = 1; // A single ASCII whitespace
    size_t required_size = 0;
    // Will add one more than required, but it's OK because of the null character.
    for (size_t i = 0; i < tag_count; i++) {
        required_size += separator_length;
        required_size += strlen(tags[i]);
    }
    char *merge = malloc(required_size);
    memset(merge, '\0', required_size);
    for (size_t i = 0; i < tag_count; i++) {
        if (i > 0) {
            strcat(merge, " ");
        }
        strcat(merge, tags[i]);
    }
    return merge;
}


/**
 * Logs the provided message to the log file with the specified level.
 *
 * The currently available levels are:
 *   1 - Information
 *   2 - Warning
 *
 * This function returns 0 if the write succeeded.
 */
int log_message(short level, char *message, char **tags, size_t tag_count) {
    FILE *log_file = fopen("log.txt", "a");
    if (log_file == NULL) {
        printf("Could not open the log file!\n");
    } else {
        char *level_string = NULL;
        if (level == 1) {
            level_string = "INFO";
        } else if (level == 2) {
            level_string = "WARN";
        }
        if (level_string != NULL) {
            if (tag_count > 0) {
                char *tag_string = merge_tags(tags, tag_count);
                fprintf(log_file, "%s [%s]: %s\n", level_string, tag_string, message);
                free(tag_string);
            } else {
                fprintf(log_file, "%s: %s\n", level_string, message);
            }
            fclose(log_file);
            return 0;
        } else {
            printf("Got unkown log level: %d!\n", level);
        }
    }
    return 1;
}

#ifndef MPP_AUTOMATON_IO_H
#define MPP_AUTOMATON_IO_H

#include <stdio.h>
#include <stdlib.h>

#include "population_utils.h"

#define PIXELS_PER_LINE 32


/**
 *
 *
 * @param filename
 * @param population
 * @param height
 * @param width
 */
void to_pbm(char *filename, cell *population, unsigned int height, unsigned int width) {
    FILE *file;

    int cursor, value;

    file = fopen(filename, "w");

    fprintf(file, "P1\n");
    fprintf(file, "%d %d\n", width - 2, height - 2);

    cursor = 0;

    for (unsigned int i = 1; i < height - 1; i++) {
        for (unsigned int j = 1; j < width - 1; j++) {
            cursor++;

            value = 1;

            if (population[i * width + j] == 1) {
                value = 0;
            }

            if (cursor == 1) {
                fprintf(file, "%1d", value);
            } else if (cursor < PIXELS_PER_LINE) {
                fprintf(file, " %1d", value);
            } else {
                fprintf(file, " %1d\n", value);
                cursor = 0;
            }
        }
    }

    if (cursor != 0) {
        fprintf(file, "\n");
    }

    fclose(file);
}


#endif //MPP_AUTOMATON_IO_H

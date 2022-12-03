#ifndef MPP_AUTOMATON_POPIO_H
#define MPP_AUTOMATON_POPIO_H

#include <stdio.h>
#include <stdlib.h>

#define PIXELS_PER_LINE 32


void to_pbm(char *filename, char *population, unsigned int height, unsigned int width) {
    FILE *file;

    int cursor, value;

    file = fopen(filename, "w");

    fprintf(file, "P1\n");
    fprintf(file, "%d %d\n", width, height);

    cursor = 0;

    for (unsigned int i = 0; i < height; i++) {
        for (unsigned int j = 0; j < width; j++) {
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


#endif //MPP_AUTOMATON_POPIO_H

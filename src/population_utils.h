#ifndef MPP_AUTOMATON_POPULATION_UTILS_H
#define MPP_AUTOMATON_POPULATION_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3


/// cell type represents individual cell in the population.
typedef char cell;

/**
 * Inserts column into 2D array in-place using offset. Unsafe - results in segmentation fault if insertion index falls
 * outside array boundaries.
 *
 * @param mat       Target 2D array to be modified in-place.
 * @param col       Source array to be inserted.
 * @param width     Row width.
 * @param len       Length of the source array.
 * @param pos       Position of the column.
 * @param offset    Offset to be added.
 */
void insert_column(cell *mat, cell *col, unsigned int width, unsigned int len, unsigned int pos, unsigned int offset) {
    for (unsigned int i = 0; i < len; i++) {
        mat[(i + offset) * width + pos] = col[i];
    }
}

/**
 * Inserts row into 2D array in-place using offset. Unsafe - results in segmentation fault if insertion index falls
 * outside array boundaries.
 *
 * @param mat       Target 2D array to be modified in-place.
 * @param col       Source array to be inserted.
 * @param width     Row width.
 * @param len       Length of the source array.
 * @param pos       Position of the column.
 * @param offset    Offset to be added.
 */
void insert_row(cell *mat, cell *col, unsigned int width, unsigned int len, unsigned int pos, unsigned int offset) {
    for (unsigned int i = 0; i < len; i++) {
        mat[pos * width + i + offset] = col[i];
    }
}

void insert_halo(cell *mat, cell *halo, unsigned int height, unsigned int width, unsigned int len, int pos) {
    switch (pos) {
        case UP:
            insert_row(mat, halo, width, len, 0, 1);
            break;
        case DOWN:
            insert_row(mat, halo, width, len, height - 1, 1);
            break;
        case LEFT:
            insert_column(mat, halo, width, len, 0, 1);
            break;
        case RIGHT:
            insert_column(mat, halo, width, len, width - 1, 1);
            break;
    }
}

/**
 * Extracts single column from 2D array using offset. This function is unsafe, and will result in segmentation fault if
 * requested column falls outside the array boundary.
 *
 * @param mat       2D array.
 * @param width     Row width.
 * @param len       Length of the column to be extracted.
 * @param pos       Column number.
 * @param offset    Offset to apply.
 * @return          Pointer to 1D array representing single column.
 */
void copy_column(cell *mat, cell *col, unsigned int width, unsigned int len, unsigned int pos, unsigned int offset) {
    for (unsigned int i = 0; i < len; i++) {
        col[i] = mat[(i + offset) * width + pos];
    }
}

/**
 * Extracts single row from 2D array using offset. This function is unsafe, and will result in segmentation fault if
 * requested row falls outside the array boundary.
 *
 * @param mat       2D array.
 * @param width     Row width.
 * @param len       Length of the row to be extracted.
 * @param pos       Row number.
 * @param offset    Offset to apply.
 * @return          Pointer to 1D array representing single row.
 */
void copy_row(cell *mat, cell *row, unsigned int width, unsigned int len, unsigned int pos, unsigned int offset) {
    for (unsigned int i = 0; i < len; i++) {
        row[i] = mat[pos * width + i + offset];
    }
}

void copy_halo(cell *mat, cell *buf, unsigned int height, unsigned width, int pos) {
    switch (pos) {
        case UP:
            copy_row(mat, buf, width, width - 2, 1, 1);
            break;
        case DOWN:
            copy_row(mat, buf, width, width - 2, height - 2, 1);
            break;
        case LEFT:
            copy_column(mat, buf, width, height - 2, 1, 1);
            break;
        case RIGHT:
            copy_column(mat, buf, width, height - 2, width - 2, 1);
            break;
    }
}

/**
 * Returns cell's next state given the sum of cell's value and it's nearest neighbours.
 *
 * @param sum   Sum of nearest neighbours.
 * @return      Next state.
 */
cell update_cell(cell sum) {
    return (sum == 2 || sum == 4 || sum == 5) ? 1 : 0;
}

/**
 * Helper function that extracts and sums cell's and nearest neighbour values needed to compute cell's next state.
 *
 * @param mat   Population of cells.
 * @param i     Row index.
 * @param j     Columns index.
 * @param w     Row width.
 * @return      Sum of cell's value and its nearest neighbours.
 */
static inline cell compute_state_sum(cell *mat, unsigned int i, unsigned int j, unsigned int w) {
    return mat[i * w + j] + mat[i * w + j - 1] + mat[i * w + j + 1] + mat[(i - 1) * w + j] + mat[(i + 1) * w + j];
}

/**
 * Computes state of the simulation at next time step using augmented population of cells. Update is performed in-place
 * using a buffer.
 *
 * @param mat       1D representation of an augmented population of cells.
 * @param buf       1D buffer that will contain augmented population at next time step.
 * @param height    Height of the augmented population.
 * @param width     Width of the augmented population.
 * @return          Number of live cells in the next time step.
 */
unsigned int update_population(cell *mat, cell *buf, unsigned int height, unsigned int width) {
    unsigned int tally = 0;
    cell next_state;

    for (unsigned int i = 1; i < height - 1; i++) {
        for (unsigned int j = 1; j < width - 1; j++) {
            next_state = update_cell(compute_state_sum(mat, i, j, width));
            tally += next_state;
            buf[i * width + j] = next_state;
        }
    }

    return tally;
}

/***
 * Sets halos in augmented population of cells to zeros.
 *
 * @param mat       Augmented population of cells.
 * @param height    Height of the population.
 * @param width     Width of the population.
 */
void reset_halos(cell *mat, unsigned int height, unsigned int width) {
    unsigned int i;

    // Reset columns.
    for (i = 1; i < height - 1; i++) {
        mat[i * width] = 0;
        mat[i * width + width - 1] = 0;
    }

    // Reset rows.
    for (i = 1; i < width - 1; i++) {
        mat[i] = 0;
        mat[(width - 1) * width + i] = 0;
    }
}


/**
 * Draws samples from uniform distribution.
 *
 * @return  Float in the range [0, 1] sampled from uniform distribution.
 */
static inline float uniform() {
    return (float) rand() / (float) (RAND_MAX);
}

/**
 * Wraps default population initialization strategy.
 *
 * @param p Probability of a cell being alive.
 * @return  Cell.
 */
static inline cell fuzzer(float p) {
    return (uniform() < p) ? 1 : 0;
}

/**
 * Initializes augmented population of cells using default strategy.
 *
 * @param mat       Augmented population of cells.
 * @param height    Height of the population.
 * @param width     Width of the population.
 * @param p         Probability of a cell being alive.
 * @return          Total number of live cells.
 */
unsigned int randomize_augmented_population(cell *mat, unsigned int height, unsigned int width, float p) {
    unsigned int alive = 0;

    for (unsigned int i = 1; i < height - 1; i++) {
        for (unsigned int j = 1; j < width - 1; j++) {
            mat[i * width + j] = fuzzer(p);
            alive += mat[i * width + j];
        }
    }

    return alive;
}

/**
 * Generates random augmented population of cells using uniform distribution.
 *
 * @param height    Height of the population.
 * @param width     Width of the population.
 * @param p         Probability of a cell being alive.
 * @return          Pointer representing augmented population of cells.
 */
cell *random_augmented_population(unsigned int height, unsigned int width, float p) {
    cell *population = malloc((height + 2) * (width + 2) * sizeof(cell));

    randomize_augmented_population(population, height, width, p);
    reset_halos(population, height, width);

    return population;
}

#endif //MPP_AUTOMATON_POPULATION_UTILS_H

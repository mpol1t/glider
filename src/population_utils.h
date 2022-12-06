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


/**
 * Cell type definition.
 */
#define MPI_CELL MPI_CHAR
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

/**
 *
 *
 * @param mat
 * @param halo
 * @param width
 * @param len
 */
void insert_upper_halo(cell *mat, cell *halo, unsigned int width, unsigned int len) {
    insert_row(mat, halo, width, len, 0, 1);
}

/**
 *
 *
 * @param mat
 * @param halo
 * @param height
 * @param width
 * @param len
 */
void insert_lower_halo(cell *mat, cell *halo, unsigned int height, unsigned int width, unsigned int len) {
    insert_row(mat, halo, width, len, height - 1, 1);
}

/**
 *
 *
 * @param mat
 * @param halo
 * @param width
 * @param len
 */
void insert_left_halo(cell *mat, cell *halo, unsigned int width, unsigned int len) {
    insert_column(mat, halo, width, len, 0, 1);
}

/**
 *
 *
 * @param mat
 * @param halo
 * @param width
 * @param len
 */
void insert_right_halo(cell *mat, cell *halo, unsigned int width, unsigned int len) {
    insert_column(mat, halo, width, len, width - 1, 1);
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

/**
 *
 *
 * @param mat
 * @param buf
 * @param height
 * @param width
 */
void copy_upper_halo(cell *mat, cell *buf, unsigned int height, unsigned width) {
    copy_row(mat, buf, width, width - 2, 1, 1);
}

/**
 *
 * @param mat
 * @param buf
 * @param height
 * @param width
 */
void copy_lower_halo(cell *mat, cell *buf, unsigned int height, unsigned width) {
    copy_row(mat, buf, width, width - 2, height - 2, 1);
}

/**
 *
 *
 * @param mat
 * @param buf
 * @param height
 * @param width
 */
void copy_left_halo(cell *mat, cell *buf, unsigned int height, unsigned width) {
    copy_column(mat, buf, width, height - 2, 1, 1);
}

/**
 *
 *
 * @param mat
 * @param buf
 * @param height
 * @param width
 */
void copy_right_halo(cell *mat, cell *buf, unsigned int height, unsigned width) {
    copy_column(mat, buf, width, height - 2, width - 2, 1);
}

/**
 * Returns cell's next state given the sum of cell's value and it's nearest neighbours.
 *
 * @param sum   Sum of nearest neighbours.
 * @return      Next state.
 */
inline cell mpp_update_cell(cell sum) {
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
inline cell mpp_compute_state_sum(cell *mat, unsigned int i, unsigned int j, unsigned int w) {
    return mat[i * w + j] + mat[i * w + j - 1] + mat[i * w + j + 1] + mat[(i - 1) * w + j] + mat[(i + 1) * w + j];
}

/**
 * Computes state of the simulation at next time step using augmented population of cells. Update is performed in-place
 * using a buffer. Stats buffer is updated in-place with the number of live cells and the number of cells that changed
 * state.
 *
 * @param mat       1D representation of an augmented population of cells.
 * @param buf       1D buffer that will contain augmented population at next time step.
 * @param height    Height of the augmented population.
 * @param width     Width of the augmented population.
 */
void update_population(
        cell *mat,
        cell *buf,
        unsigned long long *cells_alive,
        unsigned long long *cells_delta,
        unsigned int height,
        unsigned int width,
        cell (*update_fn_ptr)(cell),
        cell (*state_fn_ptr)(cell *, unsigned int, unsigned int, unsigned int)
) {
    unsigned long long delta = 0, alive = 0;
    cell next_state;

    for (unsigned int i = 1; i < height - 1; i++) {
        for (unsigned int j = 1; j < width - 1; j++) {
            next_state = update_fn_ptr(state_fn_ptr(mat, i, j, width));
            alive += next_state;

            if (buf[i * width + j] != next_state) {
                delta += 1;
            }

            buf[i * width + j] = next_state;
        }
    }

    *cells_alive = alive;
    *cells_delta = delta;
}

/***
 * Sets halos in augmented population of cells to zeros.
 *
 * @param mat       Augmented population of cells.
 * @param height    Height of the population.
 * @param width     Width of the population.
 */
void reset_halos(cell *pop, unsigned int height, unsigned int width) {
    unsigned int i;

    // Reset columns
    for (i = 0; i < height; i++) {
        pop[i * width] = 0;                 // First column
        pop[i * width + width - 1] = 0;     // Last column
    }

    // Reset rows
    for (i = 0; i < width; i++) {
        pop[i] = 0;                 // First row
        pop[(height - 1) * width + i] = 0;                  // Last row
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
unsigned long long randomize_augmented_population(cell *mat, unsigned int height, unsigned int width, float p) {
    unsigned long long alive = 0;

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
unsigned long long random_augmented_population(cell *buf, unsigned int height, unsigned int width, float p) {
//    cell *population = malloc((height + 2) * (width + 2) * sizeof(cell));

    unsigned long long live_cell_count = randomize_augmented_population(buf, height, width, p);
    reset_halos(buf, height, width);

    return live_cell_count;
}

#endif //MPP_AUTOMATON_POPULATION_UTILS_H

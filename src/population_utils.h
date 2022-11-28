//
// Created by s1548787@sms.ed.ac.uk on 28/11/22.
//

#ifndef MPP_AUTOMATON_POPULATION_UTILS_H
#define MPP_AUTOMATON_POPULATION_UTILS_H


/// cell type represents individual cell in the population.
typedef char cell;


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
cell *extract_column(cell *mat, unsigned int width, unsigned int len, unsigned int pos, unsigned int offset) {
    cell *column = malloc(len * sizeof(cell));

    for (int i = 0; i < len; i++) {
        column[i] = mat[(i + offset) * width + pos];
    }

    return column;
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
cell *extract_row(cell *mat, unsigned int width, unsigned int len, unsigned int pos, unsigned int offset) {
    cell *row = malloc(len * sizeof(cell));

    for (int i = 0; i < len; i++) {
        row[i] = mat[pos * width + i + offset];
    }

    return row;
}

/**
 * Extracts left halo from augmented cell population.
 *
 * @param mat       2D array representing cell population.
 * @param height    Height of the population.
 * @param width     Width of the population.
 * @return          Pointer to 1D array that contains left halo.
 */
cell *extract_left_halo(cell *mat, unsigned int height, unsigned width) {
    return extract_column(mat, width, height - 2, 1, 1);
}

/**
 * Extracts right halo from augmented cell population.
 *
 * @param mat       2D array representing cell population.
 * @param height    Height of the population.
 * @param width     Width of the population.
 * @return          Pointer to 1D array that contains right halo.
 */
cell *extract_right_halo(cell *mat, unsigned int height, unsigned width) {
    return extract_column(mat, width, height - 2, width - 2, 1);
}

/**
 * Extracts upper halo from augmented cell population.
 *
 * @param mat       2D array representing cell population.
 * @param height    Height of the population.
 * @param width     Width of the population.
 * @return          Pointer to 1D array that contains upper halo.
 */
cell *extract_upper_halo(cell *mat, unsigned int height, unsigned width) {
    return extract_row(mat, width, width - 2, 1, 1);
}

/**
 * Extracts lower halo from augmented cell population.
 *
 * @param mat       2D array representing cell population.
 * @param height    Height of the population.
 * @param width     Width of the population.
 * @return          Pointer to 1D array that contains lower halo.
 */
cell *extract_lower_halo(cell *mat, unsigned int height, unsigned width) {
    return extract_row(mat, width, width - 2, height - 2, 1);
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

#endif //MPP_AUTOMATON_POPULATION_UTILS_H

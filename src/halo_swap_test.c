#include <assert.h>
#include <stdio.h>
#include <stdbool.h>

#include "test_utils.h"
#include "population_utils.h"
#include "automaton.h"
#include "arg_parser.h"


/**
 * Helper function that generates 2d array of constant values.
 *
 * @param buf       Target array.
 * @param height    Array height.
 * @param width     Array width.
 * @param value     Constant to be used.
 */
void generate_constant_population(cell *buf, int height, int width, cell value) {
    for (int i = 1; i < height - 1; i++) {
        for (int j = 1; j < width - 1; j++) {
            buf[i * width + j] = value;
        }
    }
}


/**
 * Helper function that generates population, resets and swaps halos.
 *
 * @param pop   Population of cells.
 * @param sim   SimulationData struct.
 */
void generate_reset_and_swap(cell *pop, SimulationData *sim) {
    generate_constant_population(pop, sim->local_augmented_height, sim->local_augmented_width, sim->rank);
    reset_halos(pop, sim->local_augmented_height, sim->local_augmented_width);

    swap_halos(pop, sim->swap_buffer, sim);
}

/**
 * Copies halos in-place using provided buffers.
 *
 * @param pop   Population of cells.
 * @param up    Upper halo buffer.
 * @param down  Lower halo buffer.
 * @param left  Left halo buffer.
 * @param right Right halo buffer.
 * @param sim   SimulationData struct.
 */
void copy_halos(cell *pop, cell *up, cell *down, cell *left, cell *right, SimulationData *sim) {
    copy_row(pop, up, sim->local_augmented_width, sim->local_width, 0, 1);
    copy_row(pop, down, sim->local_augmented_width, sim->local_width, sim->local_augmented_height - 1, 1);
    copy_column(pop, left, sim->local_augmented_width, sim->local_width, 0, 1);
    copy_column(pop, right, sim->local_augmented_width, sim->local_width, sim->local_augmented_width - 1, 1);
}

/**
 * Tests halo swapping in a 2d grid composed of nine processes.
 *
 * @param args
 */
void TESTCASE_swap_halos(Arguments *args) {
    MPI_Init(NULL, NULL);

    SimulationData sim = init_simulation_data(args);

    if (sim.n_proc != 9) {
        MPI_Finalize();
    }

    cell * pop = malloc(sim.local_augmented_height * sim.local_augmented_width * sizeof(cell));

    cell * left = calloc(sim.local_height, sizeof(cell));
    cell * right = calloc(sim.local_height, sizeof(cell));
    cell * up = calloc(sim.local_width, sizeof(cell));
    cell * down = calloc(sim.local_width, sizeof(cell));

    generate_reset_and_swap(pop, &sim);
    copy_halos(pop, up, down, left, right, &sim);

    if (sim.x_coordinate == 0 && sim.y_coordinate == 1) {
        // Up
        all_equal(up, sim.local_width, 7);
        all_equal(down, sim.local_width, 4);
        all_equal(left, sim.local_height, 0);
        all_equal(right, sim.local_height, 2);
    } else if (sim.x_coordinate == 1 && sim.y_coordinate == 0) {
        // Left
        all_equal(up, sim.local_width, 0);
        all_equal(down, sim.local_width, 6);
        all_equal(left, sim.local_height, 0);
        all_equal(right, sim.local_height, 4);
    } else if (sim.x_coordinate == 1 && sim.y_coordinate == 1) {
        // Center
        all_equal(up, sim.local_width, 1);
        all_equal(down, sim.local_width, 7);
        all_equal(left, sim.local_height, 3);
        all_equal(right, sim.local_height, 5);
    } else if (sim.x_coordinate == 1 && sim.y_coordinate == 2) {
        // Right
        all_equal(up, sim.local_width, 2);
        all_equal(down, sim.local_width, 8);
        all_equal(left, sim.local_height, 4);
        all_equal(right, sim.local_height, 0);
    } else if (sim.x_coordinate == 2 && sim.y_coordinate == 1) {
        // Down
        all_equal(up, sim.local_width, 4);
        all_equal(down, sim.local_width, 1);
        all_equal(left, sim.local_height, 6);
        all_equal(right, sim.local_height, 8);
    }

    free(left);
    free(right);
    free(up);
    free(down);

    if (sim.rank == CONTROLLER_RANK) {
        printf("All tests passed!\n");
    }

    MPI_Finalize();
}


int main(int argc, char **argv) {
    Arguments args = parse_args(argc, argv);

    TESTCASE_swap_halos(&args);

    return 0;
}

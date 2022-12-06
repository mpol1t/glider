#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <mpi.h>

#include "arg_parser.h"
#include "automaton.h"
#include "population_utils.h"
#include "io.h"


/**
 * Runs controller process.
 *
 * @param sim               Simulation data.
 * @param fst_generation    Buffer containing first generation of cells.
 * @param snd_generation    Buffer containing second generation of cells.
 */
void run_controller(SimulationData *sim, cell *fst_generation, cell *snd_generation) {
    unsigned long long local_live_cell_count, global_live_cell_count;
    cell * tmp_generation;

    print_worker_data(sim);

    for (unsigned int i = 0; i < sim->args->max_steps; i++) {
        // Check if running on a single process to avoid deadlock.
        if (sim->n_proc > 1) {
            swap_halos(fst_generation, sim->swap_buffer, sim);
        }

        // Compute next generation.
        local_live_cell_count = update_population(
                fst_generation,
                snd_generation,
                sim->local_augmented_height,
                sim->local_augmented_width,
                &mpp_update_cell,
                &mpp_compute_state_sum
        );

        // Swap generations.
        tmp_generation = fst_generation;
        fst_generation = snd_generation;
        snd_generation = tmp_generation;

        MPI_Allreduce(&local_live_cell_count, &global_live_cell_count, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, sim->comm);

        if (i % sim->args->print_interval == 0) {
            print_interval_data(i, global_live_cell_count);
        }

        if (sim->args->early_stopping) {
            if (check_lower_threshold(global_live_cell_count, sim->lower_early_stopping_threshold)) {
                print_on_lower_threshold_touch();
                break;
            }

            if (check_upper_threshold(global_live_cell_count, sim->upper_early_stopping_threshold)) {
                print_on_upper_threshold_touch();
                break;
            }
        }

    }
}

/**
 *  Runs worker process.
 *
 * @param sim               Simulation data.
 * @param fst_generation    Buffer containing first generation of cells.
 * @param snd_generation    Buffer containing second generation of cells.
 */
void run_worker(SimulationData *sim, cell *fst_generation, cell *snd_generation) {
    unsigned long long local_live_cell_count, global_live_cell_count;
    cell * tmp_generation;

    print_worker_data(sim);

    for (unsigned int i = 0; i < sim->args->max_steps; i++) {
        swap_halos(fst_generation, sim->swap_buffer, sim);

        // Compute next generation.
        local_live_cell_count = update_population(
                fst_generation,
                snd_generation,
                sim->local_augmented_height,
                sim->local_augmented_width,
                &mpp_update_cell,
                &mpp_compute_state_sum
        );

        // Swap generations.
        tmp_generation = fst_generation;
        fst_generation = snd_generation;
        snd_generation = tmp_generation;

        MPI_Allreduce(&local_live_cell_count, &global_live_cell_count, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, sim->comm);

        if (sim->args->early_stopping) {
            if (check_lower_threshold(global_live_cell_count, sim->lower_early_stopping_threshold)) {
                break;
            }

            if (check_upper_threshold(global_live_cell_count, sim->upper_early_stopping_threshold)) {
                break;
            }
        }
    }
}


int main(int argc, char *argv[]) {
    MPI_Init(NULL, NULL);

    unsigned long long local_live_cell_count, initial_live_cell_count;

    Arguments args = parse_args(argc, argv);
    SimulationData simulation = init_simulation_data(&args);

    if (simulation.rank == CONTROLLER_RANK) {
        print_simulation_data(&simulation);
    }

    int *seeds = malloc(simulation.n_proc * sizeof(int));

    // Generate seeds.
    if (simulation.n_proc > 1) {
        init_seeds(args.seed, simulation.n_proc, seeds);

        // Initialize local random number generator.
        simulation.local_seed = seeds[simulation.rank];
    } else {
        simulation.local_seed = args.seed;
    }

    srand(simulation.local_seed);

    // Initialize local population of cells.
    cell * fst_generation = malloc(simulation.local_augmented_height * simulation.local_augmented_width * sizeof(cell));
    cell * snd_generation = malloc(simulation.local_augmented_height * simulation.local_augmented_width * sizeof(cell));
    local_live_cell_count = random_augmented_population(
            fst_generation,
            simulation.local_augmented_height,
            simulation.local_augmented_width,
            args.prob
    );

    // Reduce local live cell counts into a global live cell count.
    MPI_Allreduce(&local_live_cell_count, &initial_live_cell_count, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM,
                  simulation.comm);

    if (simulation.rank == CONTROLLER_RANK) {
        printf("automaton: rho = %.5f, live cells = %llu, actual density = %.5f\n", args.prob, initial_live_cell_count,
               (double) initial_live_cell_count / (double) (args.length * args.length));
    }

    // Compute early stopping thresholds.
    simulation.lower_early_stopping_threshold = initial_live_cell_count * LOWER_THRESHOLD_RATIO;;
    simulation.upper_early_stopping_threshold = initial_live_cell_count * UPPER_THRESHOLD_RATIO;

    if (simulation.rank == CONTROLLER_RANK) {
        run_controller(&simulation, fst_generation, snd_generation);
    } else {
        run_worker(&simulation, fst_generation, snd_generation);
    }

    if (args.write_to_file) {
        char filename[100];

        snprintf(filename, 100, "cell_%d_%d.pbm", simulation.x_coordinate, simulation.y_coordinate);

        printf("automaton: rank %d is saving data to file...\n", simulation.rank);
        to_pbm(filename, fst_generation, simulation.local_augmented_height, simulation.local_augmented_width);
    }

    // Free resources.
    free(seeds);
    free(fst_generation);
    free(snd_generation);

    free_swap_buffer(simulation.swap_buffer);
    free(simulation.swap_buffer);

    MPI_Finalize();

    return 0;
}
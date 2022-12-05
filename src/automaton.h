#ifndef MPP_AUTOMATON_AUTOMATON_H
#define MPP_AUTOMATON_AUTOMATON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <mpi.h>

#include "population_utils.h"
#include "arg_parser.h"

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

#define UPPER_THRESHOLD_RATIO 3.0 / 2.0
#define LOWER_THRESHOLD_RATIO 2.0 / 3.0

#define CONTROLLER_RANK 0
#define REORDER false


const int PERIODICITY[] = {1, 0};


/**
 *
 */
typedef struct {
    int halo_height;
    int halo_width;

    MPI_Request *recv_buf;
    MPI_Request *send_buf;

    MPI_Status *recv_status_buf;
    MPI_Status *send_status_buf;

    cell *up_send;
    cell *down_send;
    cell *left_send;
    cell *right_send;

    cell *up_recv;
    cell *down_recv;
    cell *left_recv;
    cell *right_recv;
} SwapBuffer;


/**
 *
 */
typedef struct {
    /**
     * Global variables.
     */
    unsigned int n_proc;
    unsigned int rows;
    unsigned int cols;

    int global_seed;

    unsigned long long lower_early_stopping_threshold;
    unsigned long long upper_early_stopping_threshold;

    /**
     * Local variables.
     */
    int left_neighbour;
    int right_neighbour;
    int upper_neighbour;
    int lower_neighbour;

    int local_seed;

    unsigned int x_coordinate;
    unsigned int y_coordinate;

    unsigned int local_width;
    unsigned int local_height;
    unsigned int local_augmented_width;
    unsigned int local_augmented_height;

    int rank;

    MPI_Comm comm;
    SwapBuffer *swap_buffer;
    Arguments *args;
} SimulationData;


/**
 *
 *
 * @param halo_width
 * @param halo_height
 * @return
 */
SwapBuffer *init_swap_buffer(unsigned int halo_width, unsigned int halo_height) {
    SwapBuffer *buf = malloc(sizeof(SwapBuffer));

    buf->halo_width = halo_width;
    buf->halo_height = halo_height;

    // Halo buffers must initiated in case there is no neighbour to swap halos with.
    buf->up_send = calloc(halo_width, sizeof(cell));
    buf->down_send = calloc(halo_width, sizeof(cell));
    buf->left_send = calloc(halo_height, sizeof(cell));
    buf->right_send = calloc(halo_height, sizeof(cell));

    buf->up_recv = calloc(halo_width, sizeof(cell));
    buf->down_recv = calloc(halo_width, sizeof(cell));
    buf->left_recv = calloc(halo_height, sizeof(cell));
    buf->right_recv = calloc(halo_height, sizeof(cell));

    buf->recv_buf = malloc(4 * sizeof(MPI_Request));
    buf->send_buf = malloc(4 * sizeof(MPI_Request));

    buf->recv_status_buf = malloc(4 * sizeof(MPI_Status));
    buf->send_status_buf = malloc(4 * sizeof(MPI_Status));

    return buf;
}


/**
 *
 *
 * @param buf
 */
void free_swap_buffer(SwapBuffer *buf) {
    free(buf->recv_buf);
    free(buf->send_buf);
    free(buf->recv_status_buf);
    free(buf->send_status_buf);
    free(buf->up_send);
    free(buf->down_send);
    free(buf->left_send);
    free(buf->right_send);
    free(buf->up_recv);
    free(buf->down_recv);
    free(buf->left_recv);
    free(buf->right_recv);
}


/**
 *
 *
 * @param live_cells
 * @param lower_threshold
 * @return
 */
inline bool check_lower_threshold(unsigned long long live_cells, unsigned long long lower_threshold) {
    return live_cells < lower_threshold;
}


/**
 *
 *
 * @param live_cells
 * @param upper_threshold
 * @return
 */
inline bool check_upper_threshold(unsigned long long live_cells, unsigned long long upper_threshold) {
    return live_cells > upper_threshold;
}


/**
 *
 *
 * @param length
 * @param pos
 * @param n
 * @return
 */
int get_chunk_size(int length, int pos, int n) {
    return (pos + 1 == n) ? length - (floor(length / n) * (n - 1)) : floor(length / n);
}


/**
 *
 *
 * @param args
 * @return
 */
SimulationData init_simulation_data(Arguments *args) {
    MPI_Comm topology;

    int n_proc, left_neighbour, right_neighbour, upper_neighbour, lower_neighbour, rank, local_width, local_height, local_augmented_width, local_augmented_height;

    int shape[2] = {0, 0};
    int coordinates[2] = {0, 0};

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &n_proc);


    // Compute grid shape and create cartesian topology.
    MPI_Dims_create(n_proc, 2, shape);
    MPI_Cart_create(MPI_COMM_WORLD, 2, shape, PERIODICITY, REORDER, &topology);

    // Find neighbours.
    MPI_Cart_shift(topology, 1, -1, &rank, &left_neighbour);
    MPI_Cart_shift(topology, 1, 1, &rank, &right_neighbour);
    MPI_Cart_shift(topology, 0, -1, &rank, &upper_neighbour);
    MPI_Cart_shift(topology, 0, 1, &rank, &lower_neighbour);

    // Find cartesian coordinates of this process.
    MPI_Cart_coords(topology, rank, 2, coordinates);

    // Compute population shape.
    local_width = get_chunk_size(args->length, coordinates[1], shape[1]);
    local_height = get_chunk_size(args->length, coordinates[0], shape[0]);

    local_augmented_width = local_width + 2;
    local_augmented_height = local_height + 2;

    SwapBuffer *swap_buffer = init_swap_buffer(local_width, local_height);

    SimulationData data = {
            .args                           = args,
            .rank                           = rank,
            .comm                           = topology,
            .rows                           = shape[0],
            .cols                           = shape[1],
            .n_proc                         = n_proc,
            .global_seed                    = args->seed,
            .swap_buffer                    = swap_buffer,
            .x_coordinate                   = coordinates[0],
            .y_coordinate                   = coordinates[1],
            .local_width                    = local_width,
            .local_height                   = local_height,
            .left_neighbour                 = left_neighbour,
            .right_neighbour                = right_neighbour,
            .upper_neighbour                = upper_neighbour,
            .lower_neighbour                = lower_neighbour,
            .local_augmented_width          = local_augmented_width,
            .local_augmented_height         = local_augmented_height,
    };

    return data;
}

/**
 *
 *
 * @param pop
 * @param recv
 * @param send
 * @param halo_len
 * @param height
 * @param width
 * @param target
 * @param recv_req
 * @param send_req
 * @param comm
 * @param copy_halo_fn_ptr
 */
static inline void swap_halo(
        cell *pop,
        cell *recv,
        cell *send,
        unsigned int halo_len,
        unsigned int height,
        unsigned int width,
        int target,
        MPI_Request *recv_req,
        MPI_Request *send_req,
        MPI_Comm comm,
        void (*copy_halo_fn_ptr)(cell *, cell *, unsigned int, unsigned int)
) {
    MPI_Irecv(recv, halo_len, MPI_CELL, target, MPI_ANY_TAG, comm, recv_req);
    copy_halo_fn_ptr(pop, send, height, width);
    MPI_Issend(send, halo_len, MPI_CELL, target, 0, comm, send_req);
}


/**
 *
 *
 * @param pop
 * @param buf
 * @param sim
 */
void swap_halos(cell *pop, SwapBuffer *buf, SimulationData *sim) {
    // Swap upper halos.
    swap_halo(
            pop,
            buf->up_recv,
            buf->up_send,
            buf->halo_width,
            sim->local_augmented_height,
            sim->local_augmented_width,
            sim->upper_neighbour,
            &(buf->recv_buf[UP]),
            &(buf->send_buf[UP]),
            sim->comm,
            &copy_upper_halo
    );
    // Swap left halos.
    swap_halo(
            pop,
            buf->left_recv,
            buf->left_send,
            buf->halo_height,
            sim->local_augmented_height,
            sim->local_augmented_width,
            sim->left_neighbour,
            &(buf->recv_buf[LEFT]),
            &(buf->send_buf[LEFT]),
            sim->comm,
            &copy_left_halo
    );
    // Swap lower halos.
    swap_halo(
            pop,
            buf->down_recv,
            buf->down_send,
            buf->halo_width,
            sim->local_augmented_height,
            sim->local_augmented_width,
            sim->lower_neighbour,
            &(buf->recv_buf[DOWN]),
            &(buf->send_buf[DOWN]),
            sim->comm,
            &copy_lower_halo
    );
    swap_halo(
            pop,
            buf->right_recv,
            buf->right_send,
            buf->halo_height,
            sim->local_augmented_height,
            sim->local_augmented_width,
            sim->right_neighbour,
            &(buf->recv_buf[RIGHT]),
            &(buf->send_buf[RIGHT]),
            sim->comm,
            &copy_right_halo
    );

    MPI_Waitall(4, buf->recv_buf, buf->recv_status_buf);    // Receive.
    MPI_Waitall(4, buf->send_buf, buf->send_status_buf);    // Send.

    // Insert halos.
    insert_left_halo(pop, buf->left_recv, sim->local_augmented_width, buf->halo_height);
    insert_right_halo(pop, buf->right_recv, sim->local_augmented_width, buf->halo_height);
    insert_upper_halo(pop, buf->up_recv, sim->local_augmented_width, buf->halo_height);
    insert_lower_halo(pop, buf->down_recv, sim->local_augmented_height, sim->local_augmented_width, buf->halo_height);
}


/**
 *
 *
 * @param seed
 * @param n
 * @param seeds
 */
void init_seeds(int seed, int n, int *seeds) {
    srand(seed);

    for (int i = 0; i < n; i++) {
        seeds[i] = rand();
    }
}


/**
 *
 *
 * @param sim
 */
static inline void print_worker_data(SimulationData *sim) {
    printf("automaton: rank = %d, shape = [%d, %d], coordinates = (%d, %d), seed = %u\n", sim->rank,
           sim->local_height,
           sim->local_width, sim->x_coordinate, sim->y_coordinate, sim->local_seed);
}


/**
 *
 *
 * @param sim
 */
static inline void print_simulation_data(SimulationData *sim) {
    printf("automaton: L = %d, rho = %.5f, seed = %d, maxstep = %d\n", sim->args->length, sim->args->prob,
           sim->local_seed, sim->args->max_steps);
}


/**
 *
 *
 * @param step
 * @param global_live_cell_count
 */
static inline void print_interval_data(unsigned int step, unsigned long long global_live_cell_count) {
    printf("automaton: number of live cells on step %d is %llu\n", step, global_live_cell_count);
}


/**
 *
 */
static inline void print_on_lower_threshold_touch() {
    printf("automaton: global cell count dropped below lower threshold\n");
}


/**
 *
 */
static inline void print_on_upper_threshold_touch() {
    printf("automaton: global cell count exceeded upper threshold\n");
}


#endif //MPP_AUTOMATON_AUTOMATON_H

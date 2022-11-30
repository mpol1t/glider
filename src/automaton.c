#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <mpi.h>

#include "arg_parser.h"
#include "population_utils.h"

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

#define TAG 0

const int CONTROLLER_RANK = 0;
const int PERIODS[2] = {1, 0};
const bool REORDER = false;


typedef struct {
    int height;
    int width;
    int rows;
    int columns;
    MPI_Comm comm;
} Grid;


typedef struct {
    int left;
    int right;
    int up;
    int down;
} Neighbours;


typedef struct {
    unsigned int x_coordinate;
    unsigned int y_coordinate;
    unsigned int height;
    unsigned int width;
    unsigned int rank;

    int seed;

    Neighbours neighbours;
} Process;


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


int get_chunk_size(int length, int pos, int n) {
    return (pos + 1 == n) ? length - (floor(length / n) * (n - 1)) : floor(length / n);
}


Neighbours find_neighbours(MPI_Comm comm, int source_rank) {
    int left, right, up, down;

    MPI_Cart_shift(comm, 1, -1, &source_rank, &left);
    MPI_Cart_shift(comm, 1, 1, &source_rank, &right);
    MPI_Cart_shift(comm, 0, -1, &source_rank, &up);
    MPI_Cart_shift(comm, 0, 1, &source_rank, &down);

    Neighbours neighbours = {
            .left   = left,
            .right  = right,
            .up     = up,
            .down   = down,
    };

    return neighbours;
}


Grid get_grid(MPI_Comm comm, int n_proc, int height, int width) {
    int dims[2] = {0, 0};

    MPI_Comm topology;

    MPI_Dims_create(n_proc, 2, dims);
    MPI_Cart_create(comm, 2, dims, PERIODS, REORDER, &topology);

    Grid grid = {
            .height     = height,
            .width      = width,
            .rows       = dims[0],
            .columns    = dims[1],
            .comm       = topology,
    };

    return grid;
}


Process get_process_data(Grid grid, int rank, int seed) {
    unsigned int width, height, *coordinates = malloc(2 * sizeof(int));

    MPI_Cart_coords(grid.comm, rank, 2, coordinates);

    height = get_chunk_size(grid.height, coordinates[0], grid.rows) + 2;
    width = get_chunk_size(grid.width, coordinates[1], grid.columns) + 2;

    Neighbours neighbours = find_neighbours(grid.comm, rank);

    Process process = {
            .x_coordinate   = coordinates[0],
            .y_coordinate   = coordinates[1],
            .height         = height,
            .width          = width,
            .rank           = rank,
            .neighbours     = neighbours,
            .seed           = seed,
    };

    return process;
}


int *random_seeds(int seed, int n) {
    int *seeds = malloc(n * sizeof(int));

    srand(seed);

    for (int i = 0; i < n; i++) {
        seeds[i] = rand();
    }

    return seeds;
}

void print_process_data(Process data) {
    printf(
            "coords=(%d,%d), shape=(%d,%d), rank=%d, seed: %d, L=%d, R=%d, U=%d, D=%d\n",
            data.x_coordinate,
            data.y_coordinate,
            data.height,
            data.width,
            data.rank,
            data.seed,
            data.neighbours.left,
            data.neighbours.right,
            data.neighbours.up,
            data.neighbours.down
    );
}

SwapBuffer *new_swap_buffer(int halo_height, int halo_width) {
    SwapBuffer *buf = malloc(sizeof(SwapBuffer));

    buf->halo_width = halo_width;
    buf->halo_height = halo_height;

    buf->up_send = malloc(halo_width * sizeof(cell));
    buf->down_send = malloc(halo_width * sizeof(cell));
    buf->left_send = malloc(halo_height * sizeof(cell));
    buf->right_send = malloc(halo_height * sizeof(cell));

    buf->up_recv = malloc(halo_width * sizeof(cell));
    buf->down_recv = malloc(halo_width * sizeof(cell));
    buf->left_recv = malloc(halo_height * sizeof(cell));
    buf->right_recv = malloc(halo_height * sizeof(cell));

    buf->recv_buf = malloc(4 * sizeof(MPI_Request));
    buf->send_buf = malloc(4 * sizeof(MPI_Request));

    buf->recv_status_buf = malloc(4 * sizeof(MPI_Status));
    buf->send_status_buf = malloc(4 * sizeof(MPI_Status));

    return buf;
}

static inline void swap_halo(
        cell *pop,
        cell *recv,
        cell *send,
        int halo_len,
        int height,
        int width,
        int target,
        MPI_Request *recv_req,
        MPI_Request *send_req,
        MPI_Comm comm,
        int pos
) {
    MPI_Irecv(recv, halo_len, MPI_CHAR, target, MPI_ANY_TAG, comm, recv_req);
    copy_halo(pop, send, height, width, pos);
    MPI_Issend(send, halo_len, MPI_CHAR, target, 0, comm, send_req);
}


void swap_halos(cell *pop, SwapBuffer *buf, Process *ps, MPI_Comm comm) {
    swap_halo(pop, buf->up_recv, buf->up_send, buf->halo_width, ps->height, ps->width, ps->neighbours.up,
              &(buf->recv_buf[UP]), &(buf->send_buf[UP]), comm, UP);
    swap_halo(pop, buf->left_recv, buf->left_send, buf->halo_height, ps->height, ps->width, ps->neighbours.left,
              &(buf->recv_buf[LEFT]), &(buf->send_buf[LEFT]), comm, LEFT);
    swap_halo(pop, buf->down_recv, buf->down_send, buf->halo_width, ps->height, ps->width, ps->neighbours.down,
              &(buf->recv_buf[DOWN]), &(buf->send_buf[DOWN]), comm, DOWN);
    swap_halo(pop, buf->right_recv, buf->right_send, buf->halo_height, ps->height, ps->width,
              ps->neighbours.right, &(buf->recv_buf[RIGHT]), &(buf->send_buf[RIGHT]), comm, RIGHT);

    MPI_Waitall(4, buf->recv_buf, buf->recv_status_buf);    // Receive.
    MPI_Waitall(4, buf->send_buf, buf->send_status_buf);    // Send.

    insert_halo(pop, buf->left_recv, ps->height, ps->width, buf->halo_height, LEFT);
    insert_halo(pop, buf->right_recv, ps->height, ps->width, buf->halo_height, RIGHT);
    insert_halo(pop, buf->up_recv, ps->height, ps->width, buf->halo_width, UP);
    insert_halo(pop, buf->down_recv, ps->height, ps->width, buf->halo_width, DOWN);
}


int main(int argc, char *argv[]) {
    MPI_Init(NULL, NULL);

    Arguments args = parse_args(argc, argv);

    int rank, seed, population_size, tally, global_tally = 0;

    Grid grid = get_grid(MPI_COMM_WORLD, args.n_proc, args.height, args.width);
    MPI_Comm_rank(grid.comm, &rank);

    int *seeds = random_seeds(args.seed, args.n_proc);
    MPI_Scatter(seeds, 1, MPI_INT, &seed, 1, MPI_INT, CONTROLLER_RANK, grid.comm);

    Process process = get_process_data(grid, rank, seed);

    if (rank == CONTROLLER_RANK) {
        printf("Topology: (%d,%d)\n", grid.rows, grid.columns);
    }

    print_process_data(process);

    // ------
    population_size = process.height * process.width * sizeof(int);

    cell * before = malloc(population_size);
    cell * after = malloc(population_size);
    cell * tmp = NULL;

    double ratio;

    srand(seed);

    before = random_augmented_population(process.height, process.width, args.prob);

    SwapBuffer *swap_buffer = new_swap_buffer(process.height - 2, process.width - 2);

    MPI_Status *tally_status;
    MPI_Request *tally_request;

    if (rank == CONTROLLER_RANK) {
        for (unsigned int i = 0; i < args.max_steps; i++) {
            swap_halos(before, swap_buffer, &process, grid.comm);
            tally = update_population(before, after, process.height, process.width);

            tmp = before;
            before = after;
            after = tmp;

            MPI_Reduce(&tally, &global_tally, 1, MPI_INT, MPI_SUM, CONTROLLER_RANK, grid.comm);

            ratio = global_tally / (double) (args.height * args.width);

            if (i % args.print_interval == 0) {
                printf("rank: %d, step: %d, cells_alive: %d, ratio: %.4f\n", rank, i, global_tally, ratio);
            }
        }
    } else {
        for (unsigned int i = 0; i < args.max_steps; i++) {
            swap_halos(before, swap_buffer, &process, grid.comm);
            tally = update_population(before, after, process.height, process.width);

            tmp = before;
            before = after;
            after = tmp;

            MPI_Reduce(&tally, &global_tally, 1, MPI_INT, MPI_SUM, CONTROLLER_RANK, grid.comm);
        }
    }


    MPI_Finalize();

    return 0;
}
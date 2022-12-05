#include <assert.h>
#include <stdio.h>
#include <stdbool.h>

#include "test_utils.h"
#include "population_utils.h"
#include "automaton.h"

#define DEAD 0
#define ALIVE 1


/**
 *
 */
void TESTCASE_update_cell_alive() {
    assert(update_cell(DEAD + DEAD + DEAD + ALIVE + ALIVE) == ALIVE);
    assert(update_cell(DEAD + DEAD + ALIVE + DEAD + ALIVE) == ALIVE);
    assert(update_cell(DEAD + DEAD + ALIVE + ALIVE + DEAD) == ALIVE);
    assert(update_cell(DEAD + ALIVE + DEAD + DEAD + ALIVE) == ALIVE);
    assert(update_cell(DEAD + ALIVE + DEAD + ALIVE + DEAD) == ALIVE);
    assert(update_cell(DEAD + ALIVE + ALIVE + DEAD + DEAD) == ALIVE);
    assert(update_cell(DEAD + ALIVE + ALIVE + ALIVE + ALIVE) == ALIVE);
    assert(update_cell(ALIVE + DEAD + DEAD + DEAD + ALIVE) == ALIVE);
    assert(update_cell(ALIVE + DEAD + DEAD + ALIVE + DEAD) == ALIVE);
    assert(update_cell(ALIVE + DEAD + ALIVE + DEAD + DEAD) == ALIVE);
    assert(update_cell(ALIVE + DEAD + ALIVE + ALIVE + ALIVE) == ALIVE);
    assert(update_cell(ALIVE + ALIVE + DEAD + DEAD + DEAD) == ALIVE);
    assert(update_cell(ALIVE + ALIVE + DEAD + ALIVE + ALIVE) == ALIVE);
    assert(update_cell(ALIVE + ALIVE + ALIVE + DEAD + ALIVE) == ALIVE);
    assert(update_cell(ALIVE + ALIVE + ALIVE + ALIVE + DEAD) == ALIVE);
    assert(update_cell(ALIVE + ALIVE + ALIVE + ALIVE + ALIVE) == ALIVE);
}

/**
 *
 */
void TESTCASE_update_cell_dead() {
    assert(update_cell(DEAD + DEAD + DEAD + DEAD + DEAD) == DEAD);
    assert(update_cell(DEAD + DEAD + DEAD + DEAD + ALIVE) == DEAD);
    assert(update_cell(DEAD + DEAD + DEAD + ALIVE + DEAD) == DEAD);
    assert(update_cell(DEAD + DEAD + ALIVE + DEAD + DEAD) == DEAD);
    assert(update_cell(DEAD + DEAD + ALIVE + ALIVE + ALIVE) == DEAD);
    assert(update_cell(DEAD + ALIVE + DEAD + DEAD + DEAD) == DEAD);
    assert(update_cell(DEAD + ALIVE + DEAD + ALIVE + ALIVE) == DEAD);
    assert(update_cell(DEAD + ALIVE + ALIVE + DEAD + ALIVE) == DEAD);
    assert(update_cell(DEAD + ALIVE + ALIVE + ALIVE + DEAD) == DEAD);
    assert(update_cell(ALIVE + DEAD + DEAD + DEAD + DEAD) == DEAD);
    assert(update_cell(ALIVE + DEAD + DEAD + ALIVE + ALIVE) == DEAD);
    assert(update_cell(ALIVE + DEAD + ALIVE + DEAD + ALIVE) == DEAD);
    assert(update_cell(ALIVE + DEAD + ALIVE + ALIVE + DEAD) == DEAD);
    assert(update_cell(ALIVE + ALIVE + DEAD + DEAD + ALIVE) == DEAD);
    assert(update_cell(ALIVE + ALIVE + DEAD + ALIVE + DEAD) == DEAD);
    assert(update_cell(ALIVE + ALIVE + ALIVE + DEAD + DEAD) == DEAD);
}

/**
 *
 */
void TESTCASE_reset_halos() {
    unsigned int N = 5;
    unsigned int M = N - 2;

    cell buf[N * N];

    for (int i = 0; i < N * N; i++) {
        buf[i] = i;
    }

    reset_halos(buf, N, N);

    cell left[M], right[M], up[M], down[M];

    // Check upper halo
    copy_row(buf, up, N, M, 0, 1);
    all_equal(up, M, 0);

    // Check lower halo
    copy_row(buf, down, N, M, N - 1, 1);
    all_equal(down, M, 0);

    // Check left halo
    copy_column(buf, left, N, M, 0, 1);
    all_equal(left, M, 0);

    // Check right halo
    copy_column(buf, right, N, M, 0, 1);
    all_equal(right, M, 0);
}

/**
 *
 */
void TESTCASE_compute_state_sum_zero() {
    cell buf[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    assert(compute_state_sum(buf, 1, 1, 3) == 0);
}

/**
 *
 */
void TESTCASE_compute_state_sum_five() {
    cell buf[9] = {0, 1, 0, 1, 1, 1, 0, 1, 0};
    assert(compute_state_sum(buf, 1, 1, 3) == 5);
}

/**
 *
 */
void TESTCASE_insert_halos() {
    unsigned int N = 5;
    unsigned int M = N - 2;

    cell buf[N * N];

    for (int i = 0; i < N * N; i++) {
        buf[i] = 0;
    }

    cell up[] = {1, 1, 1};
    cell right[] = {2, 2, 2};
    cell down[] = {3, 3, 3};
    cell left[] = {4, 4, 4};

    // Check upper halo.
    insert_upper_halo(buf, up, N, M);
    all_equal(up, M, 1);

    // Check right halo.
    insert_upper_halo(buf, right, N, M);
    all_equal(right, M, 2);

    // Check lower halo.
    insert_upper_halo(buf, down, N, M);
    all_equal(down, M, 3);

    // Check left halo.
    insert_upper_halo(buf, left, N, M);
    all_equal(left, M, 4);
}

/**
 *
 */
void TESTCASE_check_lower_threshold_pos() {
    assert(check_lower_threshold(0, 1) == true);
}

/**
 *
 */
void TESTCASE_check_lower_threshold_neg() {
    assert(check_lower_threshold(2, 1) == false);
}

/**
 *
 */
void TESTCASE_check_upper_threshold_pos() {
    assert(check_upper_threshold(2, 1) == true);
}

/**
 *
 */
void TESTCASE_check_upper_threshold_neg() {
    assert(check_upper_threshold(1, 2) == false);
}

/**
 *
 */
void TESTCASE_get_chunk_size_aligned() {
    assert(get_chunk_size(4, 0, 2) == 2);
}

/**
 *
 */
void TESTCASE_get_chunk_size_misaligned() {
    assert(get_chunk_size(7, 1, 2) == 4);
}


int main(int argc, char const *argv[]) {
    TESTCASE_update_cell_alive();
    TESTCASE_update_cell_dead();
    TESTCASE_reset_halos();
    TESTCASE_compute_state_sum_zero();
    TESTCASE_compute_state_sum_five();
    TESTCASE_insert_halos();
    TESTCASE_check_lower_threshold_pos();
    TESTCASE_check_lower_threshold_neg();
    TESTCASE_check_upper_threshold_pos();
    TESTCASE_check_upper_threshold_neg();
    TESTCASE_get_chunk_size_aligned();
    TESTCASE_get_chunk_size_misaligned();

    printf("All tests passed!\n");

    return 0;
}

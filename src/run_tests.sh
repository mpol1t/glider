#!/bin/bash

mpirun -n 1 ./population_utils_test
mpirun -n 9 ./halo_swap_test -l 30 0

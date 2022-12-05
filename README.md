## About The Project
This project contains MPI-based distributed 2D automaton written for MPP course at UoE, taught as part of HPC MSc. The project is written in C programming language and relies heavily on MPI library to implement message passing logic. The automaton attempts to minimize inter-process communications by minimizing serial component, and by shifting as much responsibility from controller process to workers. For example, the controller doesn't distribute the work among workers, but each process generates a sub-problem and commnicates with its neighbours only to swap halos. The only synchronization takes place when processes have to coordinate early stopping. Communication between processes is simplified due to reliance on a 2D cartesian topology. Moreover, the controller is not solely responsible for IO, but each process saves it's sub-problem to disk. As a result, saved PBM images have to be assembled using a dedicated script.
## Compilation
To compile, please run `make` inside `src/` directory. Additional python libraries must be install to allow PBM file postprocessing. To install python dependencies, please run `pip3 install opencv-python numpy`
## Tests
Project includes two test suites. `population_utils_test.c` tests serial compoenent, while `halo_swap_test.c` tests halo swapping. Both suites should be compiled using `mpicc -O3 -Wall -o outfile infile`. In order to run tests, please execute `./run_tests.sh`

## Running
To print usage, please run `./automaton --help`:

```
Usage: automaton [OPTION...] [SEED]...
MPI-based distributed 2D automaton.

  -i, --print_interval=NUM   Number of steps between printing stats.
  -l, --length=NUM           Side length.
  -m, --max_steps=NUM        Maximum number of steps.
  -p, --prob=NUM             Probability of a cell being alive.
  -w, --write_to_file=NUM    If 1, saves output to cell.pbm.
  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version

Mandatory or optional arguments to long options are also mandatory or optional
for any corresponding short options.

Report bugs to s1548787@sms.ed.ac.uk.
```

For example, `mpirun -n 8 ./automaton -i 10 -l 1000 -m 10000 -w 1 333` will run the automaton using 8 processes and will print statistics every 10 iterations. Moreover, the side length of the 2D array will be 1000, PBM files are to be written to disk and initial seed will be 333.

To assemble `cell.pbm` image, please run `python3 pbm_processor.py` inside the directory with pbm partitions.

# Glider

Glider is a distributed two-dimensional automation system written in C and utilizing MPI (Message Passing Interface). It provides a distributed solution for running and visualizing cellular automaton simulations.

## Table of Contents
1. [Building the Project](#building-the-project)
2. [Usage](#usage)
3. [Example](#example)

## Building the Project

The project can be compiled using the provided `Makefile` in the `src/` directory.

```
cd src/ && make
```

## Usage

To print the usage, run the `automaton` executable with the `--help` argument:

```
./automaton --help
```

This will output:


```
Usage: automaton [OPTION...] [SEED]...
MPI-based distributed 2D cellular automaton.

  -e, --early_stopping=NUM   If 0, early stopping is suppres![output](https://github.com/mpol1t/glider/assets/113708438/0ecdea7b-fe13-45dd-8802-6f6a87e1c8fe)
sed.
  -i, --print_interval=NUM   Number of steps between printing stats.
  -l, --length=NUM           Side length.
  -m, --max_steps=NUM        Maximum number of steps.![output](https://github.com/mpol1t/glider/assets/113708438/9428cfb9-24d0-406d-a59b-c7ef0a5f34a5)

  -p, --prob=NUM             Probability of a cell being alive.
  -w, --write_to_file=NUM    If 0, final IO is suppressed.
  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version
```

Mandatory or optional arguments to long options are also mandatory or optional for any corresponding short options.

## Example

For example, to run the automaton using 8 processes and to print statistics every 10 iterations, with a side length of the 2D array of 1000, use:

```
mpirun -n 8 ./automaton -i 10 -l 500 -m 1000 -w 1 3
```

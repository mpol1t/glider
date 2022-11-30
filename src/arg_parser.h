#ifndef MPP_AUTOMATON_ARG_PARSER_H
#define MPP_AUTOMATON_ARG_PARSER_H


#include <argp.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


#define DEFAULT_PROB 0.5
#define DEFAULT_HEIGHT 10000
#define DEFAULT_WIDTH 10000
#define DEFAULT_N_PROC 8
#define DEFAULT_MAX_STEPS 1000
#define DEFAULT_PRINT_INTERVAL 100
#define DEFAULT_SEED 0

#define MAX_FILENAME_LEN 100


const char *argp_program_version = "automaton 0.0.1";
const char *argp_program_bug_address = "s1548787@sms.ed.ac.uk";
static char doc[] = "Blah blah blah...";
static char args_doc[] = "[FILENAME]...";

static struct argp_option options[] = {
        {"prob",           'p', "NUM", 0, "Probability of a cell being alive."},
        {"height",         'h', "NUM", 0, "Height of the simulation environment."},
        {"width",          'w', "NUM", 0, "Width of the simulation environment."},
        {"n_proc",         'n', "NUM", 0, "Number of processes to use."},
        {"max_steps",      'm', "NUM", 0, "Maximum number of steps."},
        {"print_interval", 'i', "NUM", 0, "Number of steps between printing stats."},
        {"seed",           's', "NUM", 0, "Random number generator seed."},
        {0}
};


typedef struct {
    float prob;
    int height;
    int width;
    int n_proc;
    int max_steps;
    int print_interval;
    int seed;
    char filename[MAX_FILENAME_LEN];
} Arguments;


static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    Arguments *arguments = state->input;

    float prob;

    switch (key) {
        case 'p':
            prob = atof(arg);

            if (prob < 0 || prob > 1) {
                argp_usage(state);
            } else {
                arguments->prob = prob;
            }

            break;
        case 'h':
            arguments->height = atoi(arg);
            break;
        case 'w':
            arguments->width = atoi(arg);
            break;
        case 'n':
            arguments->n_proc = atoi(arg);
            break;
        case 'm':
            arguments->max_steps = atoi(arg);
            break;
        case 'i':
            arguments->print_interval = atoi(arg);
            break;
        case 's':
            arguments->seed = atoi(arg);
            break;
        case ARGP_KEY_ARG:
            if (state->arg_num > 1) {
                argp_usage(state);
            }
            strcpy(arguments->filename, arg);
            break;
        case ARGP_KEY_END:
            if (state->arg_num < 1) {
                argp_usage(state);
            }
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc};


Arguments default_args() {
    Arguments args = {
            .prob             = DEFAULT_PROB,
            .height           = DEFAULT_HEIGHT,
            .width            = DEFAULT_WIDTH,
            .n_proc           = DEFAULT_N_PROC,
            .max_steps        = DEFAULT_MAX_STEPS,
            .print_interval   = DEFAULT_PRINT_INTERVAL,
            .seed             = DEFAULT_SEED,
    };

    return args;
}

Arguments parse_args(int argc, char *argv[]) {
    Arguments args = default_args();

    argp_parse(&argp, argc, argv, 0, 0, &args);

    return args;
}


#endif //MPP_AUTOMATON_ARG_PARSER_H

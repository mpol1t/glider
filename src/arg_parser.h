#ifndef MPP_AUTOMATON_ARG_PARSER_H
#define MPP_AUTOMATON_ARG_PARSER_H


#include <argp.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


#define DEFAULT_PROB 0.49
#define DEFAULT_LENGTH 768
#define DEFAULT_MAX_STEPS 7680
#define DEFAULT_PRINT_INTERVAL 100
#define DEFAULT_WRITE_TO_FILE 1


const char *argp_program_version = "automaton 0.0.1";
const char *argp_program_bug_address = "s1548787@sms.ed.ac.uk";
static char doc[] = "Blah blah blah...";
static char args_doc[] = "[SEED]...";

static struct argp_option options[] = {
        {"prob",           'p', "NUM", 0, "Probability of a cell being alive."},
        {"length",         'l', "NUM", 0, "Side length."},
        {"max_steps",      'm', "NUM", 0, "Maximum number of steps."},
        {"print_interval", 'i', "NUM", 0, "Number of steps between printing stats."},
        {"write_to_file",  'w', "NUM", 0, "If true, saves output to cell.pbm."},
        {0}
};


typedef struct {
    double prob;
    int length;
    int max_steps;
    int print_interval;
    int seed;
    int write_to_file;
} Arguments;


static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    Arguments *arguments = state->input;

    double prob;

    switch (key) {
        case 'p':
            prob = atof(arg);

            if (prob < 0 || prob > 1) {
                argp_usage(state);
            } else {
                arguments->prob = prob;
            }

            break;
        case 'l':
            arguments->length = atoi(arg);
            break;
        case 'm':
            arguments->max_steps = atoi(arg);
            break;
        case 'i':
            arguments->print_interval = atoi(arg);
            break;
        case 'w':
            arguments->write_to_file = atoi(arg);
            break;
        case ARGP_KEY_ARG:
            if (state->arg_num > 1) {
                argp_usage(state);
            }
            arguments->seed = atoi(arg);
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
            .length           = DEFAULT_LENGTH,
            .max_steps        = DEFAULT_MAX_STEPS,
            .print_interval   = DEFAULT_PRINT_INTERVAL,
            .write_to_file    = DEFAULT_WRITE_TO_FILE,
    };

    return args;
}

Arguments parse_args(int argc, char *argv[]) {
    Arguments args = default_args();

    argp_parse(&argp, argc, argv, 0, 0, &args);

    return args;
}


#endif //MPP_AUTOMATON_ARG_PARSER_H

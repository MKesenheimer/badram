/**
 * 
*/

#include<stdbool.h>

#include "mem_range_repo.h"
#include "proc_iomem_parser.h"
#include "helpers.h"
#include "readalias.h"
#include "readalias_ioctls.h"
#include <argp.h>

//cli arguments
struct arguments {
    uint64_t start;
    uint64_t end;
    bool read;
    char* write_string;
    int verbosity;
};

/**
 * @brief main function
 * @returns 0 on success
*/
int run(struct arguments args) {
    
    return 0;
}

const char* argp_program_version = "read_write_test";
const char* argp_program_bug_address = "matthias.kesenheimer@syss.de";
static char doc[] = "Tool to test if the specified alias functions work for addresses in the memory range";
static char args_doc[] = "--start ADDRESS --end ADDRESS [--read] [--write STRING]";
static struct argp_option options[] = {
    {"start", 1, "ADDRESS", 0, "Start memory address to read from or write to (i.e. 0x00).\n", 0},
    {"end", 2, "ADDRESS", 0, "End memory address to read from (i.e. 0xdeadbeef).\n", 0},
    {"read", 3, 0, 0, "Whether to read from the memory range.\n", 0},
    {"write", 4, "STRING", 0, "String to write to the memory range.\n", 0},
    {"verbosity", 5, "NUMBER", 0, "Verbosity level (0: default, 1: error, 2: warn, 3: info, 4: debug).\n", 0},
    {0},
};


static error_t parse_opt(int key, char* arg, struct argp_state* state) {
    struct arguments* args = state->input;
    switch(key) {
        case 1:
            args->start = atoi(arg);
            if (args->start == 0 && strcmp(arg, "0") != 0) {
                perror("atoi");
                exit(EXIT_FAILURE);
            }
            break;
        case 2:
            args->end = atoi(arg);
            if (args->end == 0 && strcmp(arg, "0") != 0) {
                perror("atoi");
                exit(EXIT_FAILURE);
            }
            break;
        case 3:
            args->read = true;
            break;
        case 4:
            args->write_string = arg;
            break;
        case 5:
            args->verbosity = atoi(arg);
            if (args->verbosity == 0 && strcmp(arg, "0") != 0) {
                perror("atoi");
                exit(EXIT_FAILURE);
            }
            break;
        case ARGP_KEY_END:
            if(args->alias_file_path == NULL) {
                printf("Missing alias file path option\n");
                argp_usage(state);
                return ARGP_ERR_UNKNOWN;
            }
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = {
    options,
    parse_opt,
    args_doc,
    doc,
    0,
    0,
    0,
};

int main(int argc, char** argv) {
    struct arguments args = {
        .start = 0,
        .end = 0,
        .read = false,
        .write_string = NULL,
        .verbosity = 0
    };
    if(argp_parse(&argp, argc, argv, 0, 0, &args)) {
        printf("Failed to parse arguments\n");
        return -1;
    }
    return run(args);
}

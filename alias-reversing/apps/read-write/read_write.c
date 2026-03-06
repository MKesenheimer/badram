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
    uint64_t address;
    uint64_t length;
    bool read;
    char* write_string;
    bool access_reserved;
    int verbosity;
};

/**
 * @brief 
 * @brief mr :
 * @return 0 on success.
*/
int read_mem_range(mem_range_t mr, struct arguments args) {
    uint64_t aligned_start = mr.start;
    if (aligned_start & 0xfff) {
        aligned_start = (aligned_start + 4096) & 0xfff;
    }
    if (aligned_start >= mr.end) {
        err_log("Weird small memory range: MemRange{.start = 0x%09jx .end=0x%09jx} and aligned_start=0x%09jx\n",
            mr.start, mr.end, aligned_start);
        return -1;
    }

    printf("%d\n", args.access_reserved);

    return 0;
}

/**
 * @brief main function
 * @returns 0 on success
*/
int run(struct arguments args) {
    printf("address = 0x%lx\n", args.address);

    mem_range_t mr;
    mr.start = args.address;
    mr.end = args.address + args.length;
    char* name = "Range to read from.\n";
    memcpy(mr.name, name, strlen(name));

    double mr_size_gib = (double)(mr.end - mr.start)/(1<<30);
    double mr_size_mib = (double)(mr.end - mr.start)/(1<<20);
    double mr_size_kib = (double)(mr.end - mr.start)/(1<<10);
    double mr_size = (double)(mr.end - mr.start);
    printf("size = %f (GiB)\n", mr_size_gib);
    printf("size = %f (MiB)\n", mr_size_mib);
    printf("size = %f (kiB)\n", mr_size_kib);
    printf("size = %f (B)\n", mr_size);

    read_mem_range(mr, args);

    return 0;
}

const char* argp_program_version = "read_write_test";
const char* argp_program_bug_address = "matthias.kesenheimer@syss.de";
static char doc[] = "Tool to test if the specified alias functions work for addresses in the memory range";
static char args_doc[] = "--start ADDRESS --end ADDRESS [--read] [--write STRING]";
static struct argp_option options[] = {
    {"address", 1, "ADDRESS", 0, "Start memory address to read from or write to (i.e. 0x00).\n", 0},
    {"length", 2, "LENGTH", 0, "Number of bytes to read (i.e. 256).\n", 0},
    {"read", 3, 0, 0, "Whether to read from the memory range.\n", 0},
    {"write", 4, "STRING", 0, "String to write to the memory range.\n", 0},
    {"access-reserved", 5, 0, 0, "Allow accessing reserved memory ranges. Might lead to crashes\n", 0},
    {"verbosity", 6, "NUMBER", 0, "Verbosity level (0: default, 1: error, 2: warn, 3: info, 4: debug).\n", 0},
    {0},
};


static error_t parse_opt(int key, char* arg, struct argp_state* state) {
    struct arguments* args = state->input;
    static int argc_provided = 0;
    char *endptr;
    switch(key) {
        case 1:
            args->address = strtoul(arg, &endptr, 16);
            if (endptr == arg || *endptr != '\0') {
                fprintf(stderr, "Invalid address value\n");
                exit(EXIT_FAILURE);
            }
            ++argc_provided;
            break;
        case 2:
            args->length = strtoul(arg, &endptr, 16);
            if (endptr == arg || *endptr != '\0') {
                fprintf(stderr, "Invalid address value\n");
                exit(EXIT_FAILURE);
            }
            ++argc_provided;
            break;
        case 3:
            args->read = true;
            ++argc_provided;
            break;
        case 4:
            args->write_string = arg;
            ++argc_provided;
            break;
        case 5:
            args->access_reserved = true;
            break;
        case 6:
            args->verbosity = atoi(arg);
            if (args->verbosity == 0 && strcmp(arg, "0") != 0) {
                perror("atoi");
                exit(EXIT_FAILURE);
            }
            break;
        case ARGP_KEY_END:
            if (argc_provided == 0) {
                printf("Error: No arguments provided\n");
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
        .address = 0,
        .length = 0xff,
        .read = true,
        .write_string = NULL,
        .access_reserved = false,
        .verbosity = 0
    };
    if(argp_parse(&argp, argc, argv, 0, 0, &args)) {
        printf("Failed to parse arguments\n");
        return -1;
    }
    return run(args);
}

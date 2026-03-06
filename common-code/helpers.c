#include "include/helpers.h"
#include "include/proc_iomem_parser.h"
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

static int randomfd = -1;

int get_rand_bytes(void *p, size_t len) {
  if (randomfd == -1) {
    randomfd = open("/dev/urandom", O_RDWR);
    if (randomfd < 0) return randomfd;
  }

  size_t nb_read = read(randomfd, p, len);

  return nb_read != len;
}

void hexdump_proto1(uint8_t* a, const size_t n) {
  hexdump_proto2(a, n, 64);
}

void hexdump_proto2(uint8_t* a, const size_t n, const size_t bytes_per_row) {
  bool line_all_zero = true;
  bool previous_line_all_zero = false;
  bool print_once = false;
  printf("%p: ", a);
  for (size_t i = 0; i < n; i++) {
    if (a[i]) {
      if (!previous_line_all_zero) printf("\x1b[31m%02x \x1b[0m", a[i]);
      line_all_zero = false;
      print_once = false;
    }
    else if (a[i] == 0 && !previous_line_all_zero) {
      printf("%02x ", a[i]);
    }

    // insert new line characters
    if ((i % bytes_per_row) == (bytes_per_row - 1)) {
      if (!previous_line_all_zero) {
        printf("\n%p: ", &a[i]);
      }
      if (previous_line_all_zero && line_all_zero && !print_once) {
        printf("...");
        print_once = true;
      }
      previous_line_all_zero = line_all_zero;
      line_all_zero = true;
    }
  }
  printf("\n");
}

int do_stroul(char *str, int base, uint64_t *result) {
    (*result) = strtoul(str, NULL, base);
    // if commented in, we cannot enter zero, as uses zero as an error case. it's just stupid
    /*if ((*result) == 0) {
      printf("line %d: failed to convert %s to uint64_t\n", __LINE__, str);
      return 0;
    }*/
    if ((*result) == ULLONG_MAX && errno == ERANGE) {
        err_log( "failed to convert %s to uint64_t. errno was ERANGE\n", str);
        return -1;;
    }
    return 0;
}

int get_alias(uint64_t pa, mem_range_t* mrs, uint64_t* alias_masks, size_t len, uint64_t* out_alias) {
  for (size_t i = 0; i < len; i++) {
    if ((pa >= mrs[i].start) && (pa < mrs[i].end)) {
      *out_alias = pa ^ alias_masks[i];
      return 0;
    }
  }
  return -1;
}

uint8_t* hex_string_to_bytes(const char* hex_string, size_t* length) {
    for (size_t i = 0; i < strlen(hex_string); i++) {
        if (!isxdigit((unsigned char)hex_string[i])) {
            return NULL;
        }
    }
    
    if (strlen(hex_string) % 2 != 0) {
        return NULL;
    }
    
    *length = strlen(hex_string);
    uint8_t* bytes = calloc(*length / 2, sizeof(uint8_t));
    
    for (size_t i = 0; i < *length; i+=2) {
        bytes[i/2] = ((hex_string[i] >= 'A' ? (hex_string[i] - 'A' + 10) : (hex_string[i] - '0')) << 4) |
                  ((hex_string[i + 1] >= 'A' ? (hex_string[i + 1] - 'A' + 10) : (hex_string[i + 1] - '0')));
    }
    
    return bytes;
}
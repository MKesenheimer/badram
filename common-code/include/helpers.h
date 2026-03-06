#ifndef HELPERS_H
#define HELPERS_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#include "proc_iomem_parser.h"

#define err_log(fmt, ...) fprintf(stderr, "%s:%d : " fmt, __FILE__, __LINE__, ##__VA_ARGS__);

/**
 * @brief Read len random bytes from urandom into p
*/
int get_rand_bytes(void *p, size_t len);

/**
 * @brief Print next n bytes of a
*/
void hexdump_proto1(uint8_t* a, const size_t n);
void hexdump_proto2(uint8_t* a, const size_t n, const size_t bytes_per_row);

/* internal wrappers */
static inline void hexdump_wrap2(uint8_t* a, size_t n) {
    hexdump_proto1(a, n);
}

static inline void hexdump_wrap3(uint8_t* a, size_t n, const size_t bpr) {
    hexdump_proto2(a, n, bpr);
}
/* dispatcher helpers */
#define _HEX_GET(_1,_2,_3,NAME,...) NAME

#define hexdump(...) \
    _HEX_GET(__VA_ARGS__, hexdump3, hexdump2)(__VA_ARGS__)

/* overloads */
#define hexdump2(a,n) \
    _Generic((a), \
        uint8_t*: hexdump_wrap2 \
    )(a,n)

#define hexdump3(a,n,bpr) \
    _Generic((a), \
        uint8_t*: hexdump_wrap3 \
    )(a,n,bpr)

/**
 * @brief Wrapper with error handling around strtoul
 *
 * @param str string to parse as number
 * @param base as described in strtoul doc
 * @param result result param
 * @return 0 on success
 */
int do_stroul(char *str, int base, uint64_t *result);


/**
 * @brief Compute the alias for the given pa
 * @brief mrs: memory range with known alias masks
 * @brief alias_masks: value to xor to an addr from the corresponding memory range to get the aliased pa
 * @rief len: length of mrs and alias_masks (i.e. both have this length)
 * @brief out_alias: Output param, filled with the alias pa
 * @returns: 0 on success
*/
int get_alias(uint64_t pa, mem_range_t* mrs, uint64_t* alias_masks, size_t len, uint64_t* out_alias);

#endif

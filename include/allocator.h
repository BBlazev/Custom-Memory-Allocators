#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>

#define public  __attribute__((visibility("default")))
#define private static
#define packed  __attribute__((__packed__))
#define unused  __attribute__((__unused__))

#define Maxwords ((1024 * 1024 * 1024 / 4) - 1)

#define ErrorNoError    0
#define ErrorNoMemory   1
#define ErrorUnknown    2
#define ErrorDoubleFree 4

typedef uint32_t word;

struct packed s_header {
    word w        : 30;
    bool alloced  : 1;
    bool unused reserved : 1;
};
typedef struct packed s_header header;

#define returnError(x) do { \
    errno = (x);            \
    return (void *) 0;      \
} while (false)

#define findBlock(x) findBlock_((header *) memspace, (x))

#define alloc_k(x) alloc((x) * 1024)
#define alloc_m(x) alloc((x) * (1024 * 1024))
#define alloc_g(x) alloc((x) * (1024 * 1024 * 1024))

extern void *memspace;

public  void  *alloc(uint32_t bytes);
public  bool   free_it(void *address);
public  void   show(void);

header *findBlock_(header *hdr, word allocation);
void   *make_allocation(word words, header *hdr);
void    show_(header *hdr);

void zero(uint8_t *str, uint16_t size);

#endif

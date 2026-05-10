#include "allocator.h"

#if !defined(POLICY_FIRST_FIT) && !defined(POLICY_BEST_FIT) && \
    !defined(POLICY_WORST_FIT) && !defined(POLICY_NEXT_FIT)
#  define POLICY_FIRST_FIT
#endif

#if defined(POLICY_FIRST_FIT)

header *findBlock_(header *hdr, word allocation) {
    word n = 0;

    while (true) {
        if (n + allocation > Maxwords - 2)
            returnError(ErrorNoMemory);

        if (!hdr->w) return hdr;
        if (!hdr->alloced && hdr->w >= allocation) return hdr;

        n  += hdr->w;
        hdr = (header *)((char *) hdr + hdr->w * 4 + 4);
    }
}

#elif defined(POLICY_BEST_FIT)

header *findBlock_(header *hdr, word allocation) {
    header *best_block = NULL;
    word    best_size  = UINT32_MAX;

    while (true) {
        if (!hdr->w) break;

        if (!hdr->alloced && hdr->w >= allocation && hdr->w < best_size) {
            best_block = hdr;
            best_size  = hdr->w;
        }

        hdr = (header *)((char *) hdr + hdr->w * 4 + 4);
    }

    if (best_block) return best_block;
    return hdr;
}

#elif defined(POLICY_WORST_FIT)

header *findBlock_(header *hdr, word allocation) {
    header *largest_block = NULL;
    word    largest_size  = 0;

    while (true) {
        if (!hdr->w) break;

        if (!hdr->alloced && hdr->w >= allocation && hdr->w > largest_size) {
            largest_block = hdr;
            largest_size  = hdr->w;
        }

        hdr = (header *)((char *) hdr + hdr->w * 4 + 4);
    }

    if (largest_block) return largest_block;
    return hdr;
}

#elif defined(POLICY_NEXT_FIT)

header *findBlock_(header *heap_start, word allocation) {
    static header *rover = NULL;
    if (rover == NULL) rover = heap_start;

    header *start    = rover;
    header *hdr      = rover;
    header *sentinel = NULL;
    bool    wrapped  = false;

    while (true) {
        if (wrapped && hdr == start) {
            if (hdr->w == 0) {
                rover = (header *)((char *) hdr + allocation * 4 + 4);
                return hdr;
            }
            if (sentinel) {
                rover = (header *)((char *) sentinel + allocation * 4 + 4);
                return sentinel;
            }
            return NULL;
        }

        if (hdr->w == 0) {
            if (!wrapped) {
                sentinel = hdr;
                hdr      = heap_start;
                wrapped  = true;
                continue;
            }
            rover = (header *)((char *) hdr + allocation * 4 + 4);
            return hdr;
        }

        if (!hdr->alloced && hdr->w >= allocation) {
            rover = (header *)((char *) hdr + hdr->w * 4 + 4);
            return hdr;
        }

        hdr = (header *)((char *) hdr + hdr->w * 4 + 4);
    }
}

#endif

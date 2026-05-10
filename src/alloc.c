#include "allocator.h"

void zero(uint8_t *str, uint16_t size) {
    uint8_t *p;
    uint16_t counter;
    for (counter = 0, p = str; counter < size; counter++, p++)
        *p = 0;
}

void *make_allocation(word words, header *hdr) {
    void *ret;
    size_t bytes_in;
    word words_in;

    bytes_in = (char *) hdr - (char *) memspace;
    words_in = ((word) bytes_in) / 4 + 1;

    if (words > (Maxwords - words_in))
        returnError(ErrorNoMemory);

    if (!hdr->alloced && hdr->w > 0) {
    } else {
        hdr->w = words;
        header *next = (header *)((char *) hdr + hdr->w * 4 + 4);
        next->w = 0;
        next->alloced = false;
    }

    hdr->alloced = true;
    ret = (char *) hdr + 4;
    return ret;
}

public void *alloc(uint32_t bytes) {
    word words;
    header *hdr;
    void *mem;

    words = (bytes % 4 == 0) ? bytes / 4 : bytes / 4 + 1;

    if (words > Maxwords)
        returnError(ErrorNoMemory);

    hdr = findBlock(words);
    if (!hdr) return (void *) 0;

    mem = make_allocation(words, hdr);
    if (!mem) return (void *) 0;

    return mem;
}

public bool free_it(void *address) {
    header *p;
    uint16_t n;

    p = (header *)((char *) address - 4);

    if (!p->w || !p->alloced) {
        errno = ErrorDoubleFree;
        return false;
    }

    n = p->w * 4;
    zero((uint8_t *) address, n);
    p->alloced = false;
    return true;
}

void show_(header *hdr) {
    header *p;
    uint32_t counter;

    for (counter = 1, p = hdr; p->w; counter++) {
        printf("Alloc %u = %u %s words\n",
               counter, p->w, p->alloced ? "alloced" : "free");
        p = (header *)((char *) p + p->w * 4 + 4);
    }
}

public void show(void) {
    show_((header *) memspace);
}

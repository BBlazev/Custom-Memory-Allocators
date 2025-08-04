#include <stdio.h>
#include "alloc.h"

extern void* memspace;

private header *findBlock_(header *hdr, word allocation, word n, 
                          header *original_cursor, int *wrap_count){

    bool ok;
    void *mem;
    header *hdr_;
    word n_;

    if ((n + allocation) > (Maxwords - 2))
        returnError(ErrorNoMemory);

    if (hdr == original_cursor && *wrap_count > 0) {
        return NULL;  
    }

    ok = (hdr->w == 0) || (!(hdr->alloced) && hdr->w >= allocation);

    if (ok) {
        return hdr;  
    }

    if (hdr->w != 0) {
        mem = (void *) hdr + (hdr->w * 4) + 4;
        hdr_ = (header*) mem;
        n_ = n + hdr->w;
        return findBlock_(hdr_, allocation, n_, original_cursor, wrap_count);

    } 
    else 
    {
        if (*wrap_count == 0) {
            (*wrap_count)++;
            return findBlock_((header*)memspace, allocation, 0, original_cursor, wrap_count);
        } else {
            return hdr;  
        }
    }
}

private header *findBlock_with_cursor(word allocation) {
    static header *cursor = NULL;
    int search_counter = 0;  
    
    if (cursor == NULL) {
        cursor = (header*)memspace;
    }
    
    header *result = findBlock_(cursor, allocation, 0, cursor, &search_counter);
    

    if (result && result->w > 0) {  
        cursor = result;
    }
    
    return result;
}

private void* make_allocation(word words, header *hdr){
    void *ret;
    size_t bytes_in;  
    word words_in;
    
    bytes_in = ((char*)hdr - (char*)memspace);
    words_in = (((word)bytes_in)/4)+1;
    
    if(words > (Maxwords - words_in))
        returnError(ErrorNoMemory);
    
    if (!hdr->alloced && hdr->w > 0) {
        printf("REUSING: Block of %d words for %d word allocation (wasting %d words)\n", 
               hdr->w, words, hdr->w - words);
    } else {
        hdr->w = words;
        printf("EXPANDING: New block of %d words at end of heap\n", words);
    }
    
    hdr->alloced = true;
    ret = ((void*) hdr) + 4;

    return ret;
}

public void *alloc(int32 bytes){
    word words;
    header * hdr;
    void *mem;
    
    words = (!(bytes % 4)) ? bytes / 4 : (bytes / 4) + 1;

    hdr = findBlock_with_cursor(words);
    if(!hdr)
        return (void*) 0;

    if(words > Maxwords)
        returnError(ErrorNoMemory);
        
    mem = make_allocation(words, hdr);
    if(!mem) return (void*)0;
        
    return mem;  
}

private void show_(header *hdr){
    header *p;
    void *mem;
    int32 counter;

    for(counter = 1, p = hdr; p->w; counter++) {
        printf("Alloc %d = %d %s words\n", counter, p->w, (p->alloced) ? "alloced " : "free");
        mem = (void*)p + (p->w * 4) + 4;  
        p = (header*)mem;
    }

    return;
}

public bool free_it(void *address){
    header *p;
    int16 n;

    p = (header*)((char*)address - 4);
    (!(p->w) || (!(p->alloced))) ? ({returnError(ErrorDoubleFree); }) : (void)0;

    n = (p->w * 4);
    zero((int8*) address, n);
    p->alloced = false;

    return true;
}

int main(int argc, char* argv[]) {
    int8 *ptrs[15];  
    
    printf("Memspace: %p\n", memspace);
    
    printf("=== INITIALIZING HEAP ===\n");
    memset(memspace, 0, 4096);
    header *first_header = (header*)memspace;
    first_header->w = 0;
    first_header->alloced = false;
    first_header->reserved = 0;
    
    printf("\n=== PHASE 1: INITIAL ALLOCATIONS ===\n");
    printf("Creating allocations to demonstrate next-fit spreading...\n");
    
    ptrs[0] = alloc(40);   // 10 words
    ptrs[1] = alloc(20);   // 5 words  
    ptrs[2] = alloc(60);   // 15 words
    ptrs[3] = alloc(16);   // 4 words
    ptrs[4] = alloc(80);   // 20 words
    
    printf("\nAfter initial allocations:\n");
    show();
    
    printf("\n=== PHASE 2: CREATING GAPS ===\n");
    printf("Freeing some blocks to create fragmentation...\n");
    
    free_it(ptrs[1]);  
    free_it(ptrs[3]);  
    
    printf("After freeing (gaps of 5 and 4 words created):\n");
    show();
    
    printf("\n=== PHASE 3: NEXT-FIT BEHAVIOR TEST ===\n");
    printf("Now allocating small blocks - next-fit should reuse gaps in order encountered...\n");
    
    printf("\nAllocating 8 bytes (2 words) - should find first suitable gap:\n");
    ptrs[5] = alloc(8);  
    printf("After allocation:\n");
    show();
    
    printf("\nAllocating 12 bytes (3 words) - should continue from last position:\n");
    ptrs[6] = alloc(12);  
    printf("After allocation:\n");
    show();
    
    printf("\nAllocating 16 bytes (4 words) - should continue searching forward:\n");
    ptrs[7] = alloc(16);  
    printf("After allocation:\n");
    show();
    
    printf("\n=== PHASE 4: WRAPAROUND DEMONSTRATION ===\n");
    printf("Allocating more blocks to show wraparound behavior...\n");
    
    ptrs[8] = alloc(24);  
    printf("After wraparound allocation:\n");
    show();
    
    
    return 0;
}
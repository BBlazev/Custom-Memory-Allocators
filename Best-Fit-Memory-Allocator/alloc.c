#include <stdio.h>
#include "alloc.h"

extern void* memspace;

private header *findBlock_(header *hdr,word allocation, word n){

    bool ok;
    void *mem;
    header *hdr_;
    word n_;
    static header* best_block = NULL;
    static int32 best_size = 999999999;

    if (n == 0) {  
        best_block = NULL;
        best_size = 999999999;
    }

    if ((n+ allocation) > (Maxwords -2))
        returnError(ErrorNoMemory);

    ok = false;
    ok = (hdr->w == 0) ? true :
        (!(hdr->alloced)) && (hdr->w >= allocation) ? true : false;

    if(ok) {
        if (hdr->w == 0) {
            if (best_block == NULL) {
                best_block = hdr;
                best_size = 999999999;  
            }
        } else {
            if (hdr->w < best_size) {
                best_block = hdr;
                best_size = hdr->w;
            }
        }
    }
    if(hdr->w !=0){
        mem = (void *) hdr + (hdr->w*4) + 4;
        hdr_ = (header*) mem;
        n_ = n + hdr->w;
        return findBlock_(hdr_,allocation, n_);
    }

    return best_block;
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
    }
    
    hdr->alloced = true;
    ret = ((void*) hdr) + 4;

    return ret;
}


public void *alloc(int32 bytes){
    word words;
    header * hdr;
    void *mem, *ret; 
    
    words = (!(bytes % 4)) ? bytes / 4 : (bytes / 4) + 1;

    hdr = findBlock(words);
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
    zero((int8*) address,n);
    p->alloced = false;

    return true;

}

int main(int argc, char* argv[]) {
    int8 *ptrs[10];  
    
    printf("Memspace: %p\n", memspace);
    
    printf("=== INITIALIZING HEAP ===\n");
    memset(memspace, 0, 4096);
    header *first_header = (header*)memspace;
    first_header->w = 0;
    first_header->alloced = false;
    first_header->reserved = 0;
    
    printf("\n=== PHASE 1: INITIAL ALLOCATIONS ===\n");
    ptrs[0] = alloc(40);   // 10 words
    ptrs[1] = alloc(12);   // 3 words  
    ptrs[2] = alloc(80);   // 20 words
    ptrs[3] = alloc(24);   // 6 words
    ptrs[4] = alloc(160);  // 40 words
    ptrs[5] = alloc(16);   // 4 words
    
    printf("After initial allocations:\n");
    show();
    
    printf("\n=== PHASE 2: CREATING FRAGMENTATION ===\n");
    printf("Freeing blocks to create gaps of different sizes...\n");
    
    free_it(ptrs[1]);  // Free 3-word block
    free_it(ptrs[3]);  // Free 6-word block  
    free_it(ptrs[0]);  // Free 10-word block
    free_it(ptrs[5]);  // Free 4-word block
    
    printf("After freeing (should show free blocks of sizes: 10, 3, 6, 4):\n");
    show();
    
    printf("\n=== PHASE 3: TESTING BEST-FIT BEHAVIOR ===\n");
    
    printf("Allocating 8 bytes (2 words)...\n");
    printf("Available free blocks: 10, 3, 6, 4 words\n");
    printf("Best-fit should choose: 3-word block (smallest that fits)\n");
    ptrs[6] = alloc(8);  
    
    printf("After allocation:\n");
    show();
    
    printf("\nAllocating 20 bytes (5 words)...\n");
    printf("Best-fit should choose: 6-word block (smallest that fits)\n");
    ptrs[7] = alloc(20);  
    
    printf("After allocation:\n");
    show();
    
    printf("\nAllocating 12 bytes (3 words)...\n");
    printf("Best-fit should choose: 4-word block (smallest remaining that fits)\n");
    ptrs[8] = alloc(12);  
    printf("After allocation:\n");
    show();
    
    printf("\nAllocating 32 bytes (8 words)...\n");
    printf("Best-fit should choose: 10-word block (only one left that fits)\n");
    ptrs[9] = alloc(32);  
    
    printf("Final state:\n");
    show();

 
    
    return 0;
}
#include <stdio.h>
#include "alloc.h"

extern void* memspace;

private header *findBlock_(header *hdr,word allocation, word n){

    bool ok;
    void *mem;
    header *hdr_;
    word n_;
    static header* largest_block = NULL;
    static int32 largest_size = 0;

    if (n == 0) {  
        largest_block = NULL;
        largest_size = 0;
    }

    if ((n+ allocation) > (Maxwords -2))
        returnError(ErrorNoMemory);

    ok = false;
    ok = (hdr->w == 0) ? true :
        (!(hdr->alloced)) && (hdr->w >= allocation) ? true : false;

    if(ok) {
        if (hdr->w == 0) {
            if (largest_block == NULL) {
                largest_block = hdr;
                largest_size = 0;  
            }
        } else {
            if (hdr->w > largest_size) {
                largest_block = hdr;
                largest_size = hdr->w;
            }
        }
    }
    if(hdr->w !=0){
        mem = (void *) hdr + (hdr->w*4) + 4;
        hdr_ = (header*) mem;
        n_ = n + hdr->w;
        return findBlock_(hdr_,allocation, n_);
    }

    return largest_block;
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
    void *mem; 
    
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
    int8 *ptrs[15];  
    
    printf("Memspace: %p\n", memspace);
    
    printf("=== INITIALIZING HEAP ===\n");
    memset(memspace, 0, 8192);  
    header *first_header = (header*)memspace;
    first_header->w = 0;
    first_header->alloced = false;
    first_header->reserved = 0;
    
    printf("\n=== PHASE 1: BUILDING MEMORY LANDSCAPE ===\n");
    ptrs[0] = alloc(200);  // 50 words
    ptrs[1] = alloc(32);   // 8 words  
    ptrs[2] = alloc(48);   // 12 words
    ptrs[3] = alloc(120);  // 30 words
    ptrs[4] = alloc(24);   // 6 words
    ptrs[5] = alloc(80);   // 20 words
    ptrs[6] = alloc(16);   // 4 words
    ptrs[7] = alloc(60);   // 15 words
    
    printf("Created 8 allocations of various sizes:\n");
    show();
    
    printf("\n=== PHASE 2: CREATING STRATEGIC FRAGMENTATION ===\n");
    printf("Freeing specific blocks to create worst-fit test scenario...\n");
    
    free_it(ptrs[0]);  // Free 50-word block (LARGEST)
    free_it(ptrs[3]);  // Free 30-word block (LARGE) 
    free_it(ptrs[5]);  // Free 20-word block (MEDIUM)
    free_it(ptrs[1]);  // Free 8-word block (SMALL)
    free_it(ptrs[4]);  // Free 6-word block (SMALLEST)
    
    printf("After strategic freeing - Available free blocks: 50, 30, 20, 8, 6 words\n");
    show();
    
    printf("\n=== PHASE 3: WORST-FIT BEHAVIOR DEMONSTRATION ===\n");
    
    printf("\n--- Test 1: Small allocation (2 words needed) ---\n");
    printf("Available: 50, 30, 20, 8, 6 words\n");
    printf("WORST-FIT should choose: 50-word block (maximum waste!)\n");
    printf("Expected waste: 48 words\n");
    ptrs[8] = alloc(8);  
    printf("After allocation:\n");
    show();
    
    printf("\n--- Test 2: Medium allocation (7 words needed) ---\n"); 
    printf("WORST-FIT should choose: 30-word block (largest remaining)\n");
    printf("Expected waste: 23 words\n");
    ptrs[9] = alloc(28);  
    printf("After allocation:\n");
    show();
    
    printf("\n--- Test 3: Another small allocation (3 words needed) ---\n");
    printf("WORST-FIT should choose: 20-word block (largest remaining)\n"); 
    printf("Expected waste: 17 words\n");
    ptrs[10] = alloc(12);  
    printf("After allocation:\n");
    show();
    
    printf("\n--- Test 4: Tiny allocation (1 word needed) ---\n");
    printf("WORST-FIT should choose: 8-word block (larger than 6-word)\n");
    printf("Expected waste: 7 words\n");
    ptrs[11] = alloc(4); 
    printf("After allocation:\n");
    show();


    return 0;
}
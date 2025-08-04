#include <stdio.h>
#include "alloc.h"

extern void* memspace;

private header *findBlock_(header *hdr,word allocation, word n){

    bool ok;
    void *mem;
    header *hdr_;
    word n_;


    if ((n+ allocation) > (Maxwords -2))
        returnError(ErrorNoMemory);

    ok = false;
    ok =    (!(hdr->w)) ? true : 
            (!(hdr->alloced)) && (hdr->w >= allocation) ? true : false;

    if(ok) return  hdr;

    else {
        mem = (void *) hdr + (hdr->w*4) + 4;
        hdr_ = (header*) mem;
        n_ = n + hdr->w;

        return findBlock_(hdr_, allocation, n_);
    }

    returnError(ErrorUnknown);
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
    int8 *x, *x2, *x3, *x4;
    bool ret;

    printf("Memspace: 0x%x\n", (int*) memspace);
    
    x = alloc(5);   
    x2 = alloc(2000);
    x3 = alloc(1);
    
    printf("\n--- After all allocations ---\n");
    show();
    
    ret = free_it(x2);
    printf("\n--- After freeing x2 ---\n");
    show();

    x4 = alloc(1560);
 
    printf("\n--- After allocating x4 ---\n");
    show();

    return 0;
}
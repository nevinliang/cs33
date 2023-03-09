/*
 * mm.c - The fastest, least memory-efficient malloc package...almost :D
 *
 * I implemented an explicit free list with LIFO maintenance structure.
 * Essentially, LIFO structure combined with a first-fit placement policy
 * was what I did in order to perform memory allocation and implement this
 * mm.c file. Freeing a block took constant time, and coalescing was also
 * in constant time because of boundary tags. An explicit free list was
 * implemented with a doubly linked list including pred and succ pointer
 * so that coalescing with a previous block would not require it to loop
 * through the entire list before retuning to the beginning. Explicit free
 * lists also only include pointers to free blocks, so the amount of time
 * needed to allocate is not proportional to the total number of blocks
 * which is actually very unrealistic.
 *
 * My solution takes many lines from the textbook's implementation of implicit
 * free lists, and builds on that in order to create an explicit free list.
 * Chapter 9.9.12 as well as 9.9.13's definition of explicit free lists was
 * used, as well as solution code to do printblock and heapcheck.
 *
 * Afterthoughts: Debugging the code was probably one of the hardest things I have
 * ever done in this class. Attempting to do explicit free lists was not that bad
 * once I understood the cogs behind this type of memory allocation structure.
 * The details behind amount to increase or decrease points by was a pain to figure out.
 * Also, many times there were segmentation faults which drove me crazy. All in all, though
 * this project made me a lot more familiar with how memory is allocated freed and reallocated
 * as well as why some structures are better than others.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "ateam",
    /* First member's full name */
    "Nevin Liang 705575353",
    /* First member's email address */
    "nliang868@g.ucla.edu",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/* constants needed */
#define WSIZE 4
#define DSIZE 8
#define CHUNKSIZE (1<<12)

#define MAX(x, y) ((x > y) ? x : y)

/* pack size and allocated byte into 1 word */
#define PACK(size, alloc) (size | alloc)

/* R/W word at address p */
#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = val)

/* get size and allocated fields from address p */
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

/* compute address of header and footer from block pointer bp */
#define HDRP(bp) ((char *)(bp) - WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* compute address of next and prev blocks given block pointer bp */
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

/* get next and previous pointers */
#define NEXT_PTR(bp) (*(char **)(bp + WSIZE))
#define PREV_PTR(bp) (*(char **)(bp))

/* Global variables */
static char *heap_listp = 0;  /* Pointer to first block */
static char *free_listp = 0;

/* Function prototypes for internal helper routines */
static void *extend_heap(size_t words);
static void place(void *bp, size_t asize);
static void *find_fit(size_t asize);
static void *coalesce(void *bp);

/* list methods */
static void insert_list(void *ptr);
static void remove_list(void *ptr);

/*
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    /* Create the initial empty heap */
    if ((heap_listp = mem_sbrk(8 * WSIZE)) == (void *) - 1) //line:vm:mm:begininit
        return -1;
    PUT(heap_listp, 0);                          /* Alignment padding */
    PUT(heap_listp + (1*WSIZE), PACK(DSIZE, 1)); /* Prologue header */
    PUT(heap_listp + (2*WSIZE), PACK(DSIZE, 1)); /* Prologue footer */
    PUT(heap_listp + (3*WSIZE), PACK(0, 1));     /* Epilogue header */
    // send the free_listp to the original header pointer but DSIZE
    // ahead to account for prologue block

    heap_listp += (2 * WSIZE);
    free_listp = heap_listp + DSIZE;

    /* Extend the empty heap with a free block of CHUNKSIZE bytes */
    if (extend_heap(4) == NULL)
        return -1;

    return 0;
}

/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    size_t asize;      /* Adjusted block size */
    size_t extendsize; /* Amount to extend heap if no fit */
    char *bp;

    // recall the
    if (heap_listp == 0){
        mm_init();
    }

    /* Ignore spurious requests */
    if (size == 0)
        return NULL;

    /* Adjust block size to include overhead and alignment reqs. */
    if (size <= DSIZE)                                                  //line:vm:mm:sizeadjust1
        asize = 2*DSIZE;                                                //line:vm:mm:sizeadjust2
    else
        asize = DSIZE * ((size + (DSIZE) + (DSIZE-1)) / DSIZE);         //line:vm:mm:sizeadjust3

    /* Search the free list for a fit */
    if ((bp = find_fit(asize)) != NULL) {                                  //line:vm:mm:findfitcall
        place(bp, asize);                                                  //line:vm:mm:findfitplace
        return bp;
    }

    /* No fit found. Get more memory and place the block */
    extendsize = MAX(asize,CHUNKSIZE);                                     //line:vm:mm:growheap1
    if ((bp = extend_heap(extendsize/WSIZE)) == NULL)
        return NULL;                                                      //line:vm:mm:growheap2
    place(bp, asize);                                                     //line:vm:mm:growheap3
    
    /* mm_check(1); */
    return bp;
}

/*
 * mm_free - frees a block starting at pointer bp
 */
void mm_free(void *bp)
{
    // if the pointer is null theres nothing to fre
    if (bp == 0)
        return;

    // size of headerpointer of bp
    size_t size = GET_SIZE(HDRP(bp));

    // if heap hasnt been initialized yet
    if (heap_listp == 0) {
        mm_init();
    }

    //  fix the header and footer pointer of bp
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));

    // coalesce with any before or after
    coalesce(bp);
}

/*
 * mm_realloc - reallocates blocks starting at pointer ptr and size size
 */
void *mm_realloc(void *ptr, size_t size)
{

    size_t oldsize;
    void* newptr;

    /* If size == 0 then this is just free, and we return NULL. */
    if (size == 0) {
        mm_free(ptr);
        return 0;
    }

    /* Copy old data */
    oldsize = GET_SIZE(HDRP(ptr));
    size_t newsize = size + DSIZE; // 2 words for header and footer

    /* if newsize is less than oldsize then we just return bp */
    if (newsize <= oldsize) {
        return ptr;
    }
    size_t all = oldsize + GET_SIZE(HDRP(NEXT_BLKP(ptr)));
    /* if newsize is greater than oldsize */
    if (!GET_ALLOC(HDRP(NEXT_BLKP(ptr))) && all >= newsize) {
        /* next block is free and the size of the two blocks is greater than or equal the new size  */
        /* then we only need to combine both the blocks  */
        remove_list(NEXT_BLKP(ptr));
        PUT(HDRP(ptr), PACK(all, 1));
        PUT(FTRP(ptr), PACK(all, 1));
        return ptr;
    }
    /* executes when newize is not greater than oldsize total */
    newptr = mm_malloc(newsize);
    place(newptr, newsize);
    /* copy memory */
    memcpy(newptr, ptr, newsize);
    mm_free(ptr);
    return newptr;
}

/*
 * The remaining routines are internal helper routines
 */

/*
 * coalesce - Boundary tag coalescing. Return ptr to coalesced block
 * splits into 4 cases depending on status of prev and next blocks
 */
static void *coalesce(void *bp)
{
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp))) || PREV_BLKP(bp) == bp;
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    // 4 cases for 4 cases of coalesce

    if (prev_alloc && next_alloc) {            /* Case 1 */
        // do nothing
    }

    else if (prev_alloc && !next_alloc) {      /* Case 2 */
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        remove_list(NEXT_BLKP(bp));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size,0));
    }

    else if (!prev_alloc && next_alloc) {      /* Case 3 */
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        bp = PREV_BLKP(bp);
        remove_list(bp);
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
    }

    else {                                     /* Case 4 */
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) +
                GET_SIZE(HDRP(NEXT_BLKP(bp)));
        remove_list(PREV_BLKP(bp));
        remove_list(NEXT_BLKP(bp));
        bp = PREV_BLKP(bp);
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
    }
    insert_list(bp);
    return bp;
}

/*
 * extend_heap - Extend heap with free block and return its block pointer
 */
static void *extend_heap(size_t words)
{
    char *bp;
    size_t size;

    /* Allocate an even number of words to maintain alignment */
    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE; //line:vm:mm:beginextend

    if ((long)(bp = mem_sbrk(size)) == -1)
        return NULL;                                        //line:vm:mm:endextend

    /* Initialize free block header/footer and the epilogue header */
    PUT(HDRP(bp), PACK(size, 0));         /* Free block header */   //line:vm:mm:freeblockhdr
    PUT(FTRP(bp), PACK(size, 0));         /* Free block footer */   //line:vm:mm:freeblockftr
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); /* New epilogue header */ //line:vm:mm:newepihdr

    /* Coalesce if the previous block was free */
    return coalesce(bp);                                          //line:vm:mm:returnblock
}

/*
 * place - Place block of asize bytes at start of free block bp
 *         and split if remainder would be at least minimum block size
 */
static void place(void *bp, size_t asize)
{
    // get size of header of bp
    size_t csize = GET_SIZE(HDRP(bp));

    // calculate difference in size and pack it into header and footer pointers
    if ((csize - asize) >= (2*DSIZE)) {
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        remove_list(bp);
        // same with the next block after bp
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(csize-asize, 0));
        PUT(FTRP(bp), PACK(csize-asize, 0));
        coalesce(bp);
    }
    // if the difference in size is not 2 * DSIZE
    else {
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
        remove_list(bp);
    }
}

/*
 * find_fit - Find a fit for a block with asize bytes
 */
static void *find_fit(size_t asize)
{
    // use first fit to loop through the entire block finding a place to fit a block
    // of size asize
    void *bp;
    for (bp = free_listp; GET_ALLOC(HDRP(bp)) == 0; bp = NEXT_PTR(bp)) {
        if (asize <= (size_t)GET_SIZE(HDRP(bp)) ) {
            // return the place to insert
            return bp;
        }
    }
    return NULL;
}

/*
 * insert_list - LIFO insertion for free list
 */
static void insert_list(void *ptr) {
    // insert at beginning of list
    NEXT_PTR(ptr) = free_listp;
    PREV_PTR(free_listp) = ptr;
    PREV_PTR(ptr) = NULL;
    free_listp = ptr;
}

/*
 * remove_list - LIFO removal for free list
 */
static void remove_list(void *ptr) {
    // if the previous is null then just insert it at the beginning
    if (PREV_PTR(ptr) == NULL) {
        free_listp = NEXT_PTR(ptr);
    }
    else {
        // otherwise use a linkedlist algorithm to remove
        NEXT_PTR(PREV_PTR(ptr)) = NEXT_PTR(ptr);
    }
    // connect last and next :))
    PREV_PTR(NEXT_PTR(ptr)) = PREV_PTR(ptr);
}

/* CHECKER FUNCTIONS. NOT VITAL BUT PART OF SPECS */

/*
 * mm_check - wrapper function for checkheap (calling verbose)
 */
int mm_check() {
    // just call the parent func with verbose = 1 :))
    checkheap(1);
}

static void printblock(void *bp)
{
    size_t hsize, halloc, fsize, falloc;

    // first check heap
    checkheap(0);
    // get vars for header and footer
    hsize = GET_SIZE(HDRP(bp));
    halloc = GET_ALLOC(HDRP(bp));
    fsize = GET_SIZE(FTRP(bp));
    falloc = GET_ALLOC(FTRP(bp));

    // if its empty then print it out
    if (hsize == 0) {
        printf("%p: EOL\n", bp);
        return;
    }

    // otherwise print out details of block
    printf("%p: header: [%ld:%c] footer: [%ld:%c]\n", bp,
           hsize, (halloc ? 'a' : 'f'),
           fsize, (falloc ? 'a' : 'f'));
}

static void checkblock(void *bp)
{
    // check for alignment and header/footer matching
    if ((size_t)bp % 8)
        printf("Error: %p is not doubleword aligned\n", bp);
    if (GET(HDRP(bp)) != GET(FTRP(bp)))
        printf("Error: header does not match footer\n");
}

/*
 * checkheap - Minimal check of the heap for consistency
 * verbose is the boolean to print out the blocks. verbose is set to 1 in wrapper func
 */
void checkheap(int verbose)
{
    char *bp = heap_listp;

    // verbose = print more detailed information
    if (verbose)
        printf("Heap (%p):\n", heap_listp);

    // check prologue
    if ((GET_SIZE(HDRP(heap_listp)) != DSIZE) || !GET_ALLOC(HDRP(heap_listp)))
        printf("Bad prologue header\n");
    checkblock(heap_listp);

    // check rest of the blocks
    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
        if (verbose)
            printblock(bp);
        checkblock(bp);
    }

    // check if prev and next for free lists match
    char *flp;
    while (flp != NULL) {
        if (NEXT_PTR(NEXT_PTR(flp)) != NULL && PREV_PTR(PREV_PTR(flp)) != NULL) {
            // jump to next in free list and check prev of that or vice versa
            if (PREV_PTR(PREV_PTR(NEXT_PTR(NEXT_PTR(flp)))) != NEXT_PTR(NEXT_PTR(PREV_PTR(PREV_PTR(flp))))) {
                printf("prev and next dont match");
            }
        }
        flp = NEXT_PTR(NEXT_PTR(flp));
    }

    // check epilogue block
    if (verbose)
        printblock(bp);
    if ((GET_SIZE(HDRP(bp)) != 0) || !(GET_ALLOC(HDRP(bp))))
        printf("Bad epilogue header\n");
}




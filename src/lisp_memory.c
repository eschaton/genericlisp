/*
    File:       lisp_memory.c

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#include "lisp_memory.h"

#include "lisp_atom.h"
#include "lisp_string.h"
#include "lisp_utilities.h"

#if LISP_USE_STDLIB
#include <stdlib.h>
#endif


/** The raw heap is the actual C allocation, in case it needs to be adjusted for Lisp. */
static void *raw_heap = NULL;

/** The Lisp heap is where all Lisp allocations come from, keep track of where it starts. */
static void *lisp_heap_start = NULL;

/** The size of the Lisp heap. */
static uintptr_t lisp_heap_size = 0;

/** Keep track of the current point in the Lisp heap, so we know where the next allocation comes from. */
static void *lisp_heap_cur = NULL;

/** The page size. */
const uintptr_t lisp_page_size = 4096;

/** The page shift (number of bits in the page size). */
const uintptr_t lisp_page_shift = 12;

/** The page mask (to get the page from an address). */
#if __LP64__
const uintptr_t lisp_page_mask = 0xFFFFFFFFFFFFF000;
#else
const uintptr_t lisp_page_mask = 0xFFFFF000;
#endif


/** Collect garbage. */
static void lisp_heap_garbage_collect(void);


void lisp_heap_initialize(uintptr_t size)
{
#if LISP_USE_STDLIB
    /* Assume page alignment from calloc. */
    raw_heap = calloc(size, sizeof(uint8_t));
#else
#warning Implement lisp_heap_initialize without stdlib.
#endif
    lisp_heap_start = raw_heap;
    lisp_heap_size = size;
    lisp_heap_cur = lisp_heap_start;
}


void lisp_heap_finalize(void)
{
    /* Dispose of the heaps and reset the values. */
#if LISP_USE_STDLIB
    free(raw_heap);
#else
#warning Implement lisp_heap_finalize without stdlib.
#endif

    raw_heap = NULL;
    lisp_heap_start = NULL;
    lisp_heap_size = 0;
    lisp_heap_cur = NULL;
}


void lisp_heap_garbage_collect(void)
{
#warning lisp_heap_garbage_collect: Implement.
    exit(1);
}


#if !LISP_DEBUG_ALLOCATION

lisp_object_t lisp_object_allocate(lisp_tag_t tag, uintptr_t size, void **raw_value)
{
    /*
     Compute the size to allocate, which will be a multiple of 16 to ensure
     an alignment that leaves bits for tagging.
     */
    uintptr_t alloc_size = lisp_round_to_next_multiple(size, 16);

    /*
     Get the heap start and the current point in the heap as unsigned
     integers so we can do math with them.
     */
    uintptr_t lisp_heap_cur_value = (uintptr_t) lisp_heap_cur;
    uintptr_t lisp_heap_start_value = (uintptr_t) lisp_heap_start;

    /*
     Determine whether this allocation would overflow the heap. If it would
     then run the garbage collector first.
     */
    if ((lisp_heap_cur_value + alloc_size) >= (lisp_heap_start_value + lisp_heap_size)) {
        lisp_heap_garbage_collect();
    }

    /*
     Allocate the object itself as by adjusting the current heap pointer to reflect
     the allocation, and use the old heap pointer to represent the allocated object.
     */
    uintptr_t new_heap_value = lisp_heap_cur_value + alloc_size;
    lisp_heap_cur = (void *) new_heap_value;

    /* Mix in the requested tag. */
    uintptr_t object_value = lisp_heap_cur_value | tag;

    /*
     Return the new, empty object, both as a fully tagged Lisp object and
     as a "raw" object suitable for filling in by a create function.
     */
    lisp_object_t object = (lisp_object_t) object_value;
    if (raw_value != NULL) {
        *raw_value = (void *)lisp_heap_cur_value;
    }
    return object;
}

#else

lisp_object_t lisp_object_allocate(lisp_tag_t tag, uintptr_t size, void **raw_value)
{
    void *allocation = malloc(size);
    uintptr_t lisp_heap_cur_value = (uintptr_t)allocation;
    uintptr_t object_value = lisp_heap_cur_value | tag;
    lisp_object_t object = (lisp_object_t) object_value;
    if (raw_value != NULL) {
        *raw_value = (void *)allocation;
    }
    return object;
}

#endif /* LISP_DEBUG_ALLOCATION */

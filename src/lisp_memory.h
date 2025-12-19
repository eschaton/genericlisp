/*
    File:       lisp_memory.h

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#ifndef __lisp_memory__
#define __lisp_memory__ 1


#include "lisp_types.h"


/**
 Initialize the Lisp heaps to a specific size.
 */
LISP_EXTERN void lisp_heap_initialize(uintptr_t size);

/**
 Finalize the Lisp heap.
 */
LISP_EXTERN void lisp_heap_finalize(void);


/**
 Allocate an object on the heap of the specified size.

 All allocations are rounded to up to the next 16-byte boundary, both to
 accommodate the four tag bits that Lisp uses and to ensure best
 performance on more modern systems at the cost of a slightly higher
 working set size.

 - Parameters:
   - tag: The tag to apply to the new object.
   - size: The size of the new object.
   - raw_value: If not `NULL`, receives the raw pointer to the new object to fill in.
 */
LISP_EXTERN lisp_object_t lisp_object_allocate(lisp_tag_t tag, uintptr_t size, void **raw_value);


#endif  /* __lisp_memory__ */

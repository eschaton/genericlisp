/*
    File:       lisp_vector.h

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#ifndef __lisp_vector__
#define __lisp_vector__ 1


#include "lisp_types.h"


/**
 A Lisp vector represents a contiguous memory array of Lisp objects.

 Typically these will all be of the same type, but that's not a strict
 requirement.

 - Note: The objects in a vector *are* subject to garbage collection.
         This is the primary distinction between a vector and an
         interior pointer.
 */
typedef struct lisp_vector {
    /** The values in the vector. */
    lisp_object_t *values;

    /** How many values the vector can hold before reallocation. */
    uintptr_t capacity;

    /** The number of values in the vector. */
    uintptr_t count;
} *lisp_vector_t;


/** Get the raw vector value of the given Lisp object. */
LISP_EXTERN lisp_vector_t lisp_vector_get_value(lisp_object_t object);

/** Prints the vector to the given output stream. */
LISP_EXTERN lisp_object_t lisp_vector_print(lisp_object_t environment,
                                            lisp_object_t stream,
                                            lisp_vector_t vector_value);

/**
 Compare two vectors for equality.

 Two vectors are equal if their size is equal and all of their contents
 are also equal.
 */
LISP_EXTERN lisp_object_t lisp_vector_equal(lisp_object_t a, lisp_object_t b);


#endif  /* __lisp_vector__ */

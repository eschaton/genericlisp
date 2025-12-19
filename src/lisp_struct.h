/*
    File:       lisp_struct.h

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#ifndef __lisp_struct__
#define __lisp_struct__ 1


#include "lisp_types.h"


/**
 A Lisp structure.
 */
typedef struct lisp_struct {
    /** A raw pointer to the actual data. */
    void *value;

    /** The size of the struct, in bytes. */
    uintptr_t size;

    /** The type information for the struct. */
    uintptr_t type;
} *lisp_struct_t;

/** Creates a Lisp struct object from the given values. */
LISP_EXTERN lisp_object_t lisp_struct_create(void *value,
                                             uintptr_t size,
                                             uintptr_t type);

/** Gets the struct value of the given Lisp object.  */
LISP_EXTERN lisp_struct_t lisp_struct_get_value(lisp_object_t object);

/** Prints the struct to the given output stream. */
LISP_EXTERN lisp_object_t lisp_struct_print(lisp_object_t environment,
                                            lisp_object_t stream,
                                            lisp_struct_t struct_value);

/** Compares two struct objects. */
LISP_EXTERN lisp_object_t lisp_struct_equal(lisp_object_t a, lisp_object_t b);


#endif  /* __lisp_struct__ */

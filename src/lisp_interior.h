/*
    File:       lisp_interior.h

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#ifndef __lisp_interior__
#define __lisp_interior__ 1


#include "lisp_types.h"


/**
 A Lisp interior pointer represents raw storage allocated on the heap
 whose contents are *not* subject to garbage collection.
 */
typedef void *lisp_interior_t;

/**
 Create an interior pointer object on the heap with the given size, and
 return a raw pointer to the storage as well.
 */
lisp_object_t lisp_interior_create(uintptr_t size, void **underlying);

/** Get the interior pointer value of the given Lisp object. */
LISP_EXTERN lisp_interior_t lisp_interior_get_value(lisp_object_t object);

/** Prints the interior pointer to the given output stream. */
LISP_EXTERN lisp_object_t lisp_interior_print(lisp_object_t stream, lisp_interior_t interior_value);

/**
 Compares two interior pointers for equality.

 Interior pointers are only equal if they are identical, since their
 internal structure is opaque to Lisp. This is distinct from structures
 and vectors, whose internal structure is comprehensible to Lisp.
 */
LISP_EXTERN lisp_object_t lisp_interior_equal(lisp_object_t a, lisp_object_t b);


#endif  /* __lisp_interior__ */

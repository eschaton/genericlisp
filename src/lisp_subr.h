/*
    File:       lisp_subr.h

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#ifndef __lisp_subr__
#define __lisp_subr__ 1


#include "lisp_types.h"


/**
 A Lisp `callable` is a function pointer that can be invoked within the
 system.
 */
typedef lisp_object_t (*lisp_callable)(lisp_object_t environment, lisp_object_t arguments);


/**
 A Lisp `SUBR` represents a compiled or kernel function (or
 _subroutine_) that the system can apply to arguments to produce a
 result. It is distinguished from an `EXPR` which is a Lisp list (or
 _expression_) that the system can also apply to arguments to produce a
 result.

 A `SUBR` is heap-allocated because it contains (rather than just
 represents) the pointer to executable code as well as potentially other
 information needed by the system to invoke it.
 */
typedef struct lisp_subr {
    lisp_callable function;
    lisp_object_t name;
} *lisp_subr_t;


/** Create a Lisp `SUBR` object with the given function and name. */
LISP_EXTERN lisp_object_t lisp_subr_create(lisp_callable function, lisp_object_t name);

/** Gets the `SUBR` value of the given Lisp object.  */
LISP_EXTERN lisp_subr_t lisp_subr_get_value(lisp_object_t object);

/** Prints the `SUBR` to the given output stream. */
LISP_EXTERN lisp_object_t lisp_subr_print(lisp_object_t stream, lisp_subr_t subr_value);

/** Compares two `SUBR` objects. */
LISP_EXTERN lisp_object_t lisp_subr_equal(lisp_object_t a, lisp_object_t b);

/** Call the `SUBR` in an environment with an argument list. */
LISP_EXTERN lisp_object_t lisp_subr_call(lisp_object_t subr, lisp_object_t environment, lisp_object_t arguments);


#endif  /* __lisp_subr__ */

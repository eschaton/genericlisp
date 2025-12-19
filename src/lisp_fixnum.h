/*
    File:       lisp_fixnum.h

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#ifndef __lisp_fixnum__
#define __lisp_fixnum__ 1


#include "lisp_types.h"


/**
    A fixnum represents an integer in the inclusive range

        [ -2^(BITS-(TAG+SIGN)),
           2^(BITS-(TAG+SIGN))-1 ]

    where `BITS` is 32 or 64, `TAG` is 4 bits, and `SIGN` is 1. This
    means a fixnum can represent a 28-bit signed quantity on a 32-bit
    system and a 60-bit signed quantity on a 64-bit system.
 */
typedef intptr_t lisp_fixnum_t;


/** Creates a Lisp fixnum with the given value. */
LISP_EXTERN lisp_object_t lisp_fixnum_create(lisp_fixnum_t value);

/** Gets the fixnum value of the given Lisp object. */
LISP_EXTERN intptr_t lisp_fixnum_get_value(lisp_object_t object);

/** Prints the fixnum to the given output stream. */
LISP_EXTERN lisp_object_t lisp_fixnum_print(lisp_object_t stream, lisp_fixnum_t fixnum_value);

/** Checks two fixnum for equality. */
LISP_EXTERN lisp_object_t lisp_fixnum_equal(lisp_object_t a, lisp_object_t b);


#endif  /* __lisp_fixnum__ */

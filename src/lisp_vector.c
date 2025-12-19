/*
    File:       lisp_vector.c

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#include "lisp_vector.h"

#include "lisp_environment.h"
#include "lisp_printing.h"
#include "lisp_string.h"


lisp_vector_t lisp_vector_get_value(lisp_object_t object)
{
    uintptr_t raw_value = lisp_object_get_raw_value(object);
    return (lisp_vector_t)raw_value;
}


lisp_object_t lisp_vector_print(lisp_object_t environment,
                                lisp_object_t stream,
                                lisp_vector_t vector_value)
{
    lisp_char_print_quoted(stream, char_octothorpe, lisp_NIL);
    lisp_char_print_quoted(stream, char_paren_open, lisp_NIL);
    const uintptr_t c = vector_value->count;
    for (uintptr_t i = 0; i < c; i++) {
        lisp_print(environment, stream, vector_value->values[i]);
        if ((i != 0) && (i == (c - 1))) {
            lisp_char_print_quoted(stream, char_space, lisp_NIL);
        }
    }
    lisp_char_print_quoted(stream, char_paren_close, lisp_NIL);

    return lisp_T;
}


lisp_object_t lisp_vector_equal(lisp_object_t a, lisp_object_t b)
{
    lisp_vector_t a_value = lisp_vector_get_value(a);
    lisp_vector_t b_value = lisp_vector_get_value(b);

    /*
     For two vectors to be equal, they must have both equal lengths and
     equal components. Their capacities are not taken into account.
     */

    if (a_value->count != b_value->count) {
        return lisp_NIL;
    }

    /* If the vectors share value pointers, they're definitely equal. */
    if (a_value->values == b_value->values) {
        return lisp_T;
    }

    /* Iterate over the contents and check each for equality. */
    for (uintptr_t i = 0; i < a_value->count; i++) {
        if (lisp_equal(a_value->values[i], b_value->values[i]) == lisp_NIL) {
            return lisp_NIL;
        }
    }

    return lisp_T;
}

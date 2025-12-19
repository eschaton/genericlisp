/*
    File:       lisp_subr.c

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#include "lisp_subr.h"

#include "lisp_environment.h"
#include "lisp_memory.h"
#include "lisp_printing.h"
#include "lisp_string.h"


lisp_object_t lisp_subr_create(lisp_callable function, lisp_object_t name)
{
    lisp_subr_t underlying;
    lisp_object_t object = lisp_object_allocate(lisp_tag_subr, sizeof(struct lisp_subr), (void **)&underlying);

    underlying->function = function;
    underlying->name = name;

    return object;
}


lisp_subr_t lisp_subr_get_value(lisp_object_t object)
{
    uintptr_t raw_value = lisp_object_get_raw_value(object);
    return (lisp_subr_t)raw_value;
}


lisp_object_t lisp_subr_print(lisp_object_t stream, lisp_subr_t subr_value)
{
    /*
     Print a SUBR as #'name. (This will eventually also be the syntax to
     reference the `SUBR` or `EXPR` of a symbol without falling back to its
     `APVAL`.)
     */
    lisp_char_print_quoted(stream, char_octothorpe, lisp_NIL);
    lisp_char_print_quoted(stream, char_single_quote, lisp_NIL);
    lisp_string_t name_value = lisp_string_get_value(subr_value->name);
    return lisp_string_print_quoted(stream, name_value, lisp_NIL);
}


lisp_object_t lisp_subr_equal(lisp_object_t a, lisp_object_t b)
{
    lisp_subr_t a_value = lisp_subr_get_value(a);
    lisp_subr_t b_value = lisp_subr_get_value(b);

    /*
     Two subroutines are equal if they have the same function pointer and
     their names are equal.
     */
    if (a_value->function != b_value->function) {
        return lisp_NIL;
    }

    if (lisp_equal(a_value->name, b_value->name) == lisp_NIL) {
        return lisp_NIL;
    }

    return lisp_T;
}


lisp_object_t lisp_subr_call(lisp_object_t subr, lisp_object_t environment, lisp_object_t arguments)
{
    lisp_subr_t subr_value = lisp_subr_get_value(subr);
    lisp_callable function = subr_value->function;
    return (*function)(environment, arguments);
}

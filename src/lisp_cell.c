/*
    File:       lisp_cell.c

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#include "lisp_cell.h"

#include "lisp_environment.h"
#include "lisp_memory.h"
#include "lisp_printing.h"
#include "lisp_string.h"

#if LISP_USE_STDLIB
#include <stdarg.h>
#include <stdlib.h>
#endif


lisp_object_t lisp_cell_cons(lisp_object_t car, lisp_object_t cdr)
{
    lisp_cell_t cell;
    lisp_object_t object = lisp_object_allocate(lisp_tag_cell, sizeof(struct lisp_cell), (void **)&cell);
    cell->car = car;
    cell->cdr = cdr;
    return object;
}


lisp_object_t lisp_cell_car(lisp_object_t cell)
{
    if (cell == lisp_NIL) return lisp_NIL;
    if (lisp_cellp(cell) == lisp_NIL) return lisp_NIL;
    lisp_cell_t cell_value = lisp_cell_get_value(cell);
    return cell_value->car;
}


lisp_object_t lisp_cell_cdr(lisp_object_t cell)
{
    if (cell == lisp_NIL) return lisp_NIL;
    if (lisp_cellp(cell) == lisp_NIL) return lisp_NIL;
    lisp_cell_t cell_value = lisp_cell_get_value(cell);
    return cell_value->cdr;
}


lisp_object_t lisp_cell_rplaca(lisp_object_t cell, lisp_object_t newcar)
{
    lisp_cell_t cell_value = lisp_cell_get_value(cell);
    cell_value->car = newcar;
    return cell;
}


lisp_object_t lisp_cell_rplacd(lisp_object_t cell, lisp_object_t newcdr)
{
    lisp_cell_t cell_value = lisp_cell_get_value(cell);
    cell_value->cdr = newcdr;
    return cell;
}


lisp_object_t lisp_cell_list(lisp_object_t car, ...)
{
    lisp_object_t list = lisp_cell_cons(car, lisp_NIL);
    lisp_object_t list_cur = list;
    int done = 0;

    va_list ap;
    va_start(ap, car);
    do {
        lisp_object_t list_cdr = va_arg(ap, lisp_object_t);
        if (list_cdr == lisp_NIL) {
            lisp_cell_rplacd(list_cur, lisp_NIL);
            done = 1;
        } else {
            lisp_object_t new_cdr = lisp_cell_cons(list_cdr, lisp_NIL);
            lisp_cell_rplacd(list_cur, new_cdr);
            list_cur = new_cdr;
        }
    } while (!done);
    va_end(ap);

    return list;
}


lisp_cell_t lisp_cell_get_value(lisp_object_t object)
{
    uintptr_t raw_value = lisp_object_get_raw_value(object);
    void *ptr_value = (void *) raw_value;
    return (lisp_cell_t)ptr_value;
}


lisp_object_t lisp_cell_print(lisp_object_t environment, lisp_object_t stream, lisp_cell_t cell_value)
{
    return lisp_cell_print_dotted(environment, stream, cell_value, lisp_T);
}


static lisp_object_t lisp_print_for_cell(lisp_object_t environment,
                                         lisp_object_t stream,
                                         lisp_object_t object,
                                         lisp_object_t compress_dots)
{
    switch (lisp_object_get_tag(object)) {
        case lisp_tag_cell: {
            lisp_cell_t cell_value = lisp_cell_get_value(object);
            return lisp_cell_print_dotted(environment, stream, cell_value, compress_dots);
        } break;

        case lisp_tag_char: {
            lisp_char_t char_value = lisp_char_get_value(object);
            return lisp_char_print_quoted(stream, char_value, lisp_T);
        } break;

        case lisp_tag_string: {
            lisp_string_t string_value = lisp_string_get_value(object);
            return lisp_string_print_quoted(stream, string_value, lisp_T);
        } break;

        default:
            return lisp_print(environment, stream, object);
    }
}

/**
 Print a Lisp cell (typically a list) in a legible format, with dot compression.
 */
static lisp_object_t lisp_cell_print_readably(lisp_object_t environment,
                                              lisp_object_t stream,
                                              lisp_cell_t cell_value)
{
    lisp_char_print_quoted(stream, char_paren_open, lisp_NIL);
    {
        /* Print the car in a way that's appropriate for its type. */
        lisp_print_for_cell(environment, stream, cell_value->car, lisp_T);

        /* Traverse the CDR chain, printing until done. */
        for (lisp_object_t rest = cell_value->cdr; rest != lisp_NIL; rest = lisp_cell_cdr(rest)) {
            /*
             There are three cases to handle here. The first (end of list) is
             handled by the for loop's end condition.
             */
            if (rest == lisp_NIL) {
                /* If the current cell's CDR is NIL, just be done. */
            } else if (lisp_cellp(rest) != lisp_NIL) {
                /*
                 If the current cell's CDR is a cell, so print a space, then its
                 CAR, and then let the loop continue.
                 */
                lisp_char_print_quoted(stream, char_space, lisp_NIL);
                lisp_object_t rest_car = lisp_cell_car(rest);
                lisp_print_for_cell(environment, stream, rest_car, lisp_T);
            } else {
                /*
                 The current cell's CDR is something else, print it after a space, a
                 dot, and another space (e.g. structurally). Then be done, because
                 the CDR of a non-cell is always NIL.
                 */
                lisp_char_print_quoted(stream, char_space, lisp_NIL);
                lisp_char_print_quoted(stream, char_period, lisp_NIL);
                lisp_char_print_quoted(stream, char_space, lisp_NIL);
                lisp_print_for_cell(environment, stream, rest, lisp_T);
            }
        }
    }
    lisp_char_print_quoted(stream, char_paren_close, lisp_NIL);

    return lisp_T;
}

/**
 Print a Lisp cell (typically a list) in a structural format, e.g.
 `(CAR . CDR)`.
 */
static lisp_object_t lisp_cell_print_structurally(lisp_object_t environment,
                                                  lisp_object_t stream,
                                                  lisp_cell_t cell_value)
{
    if (cell_value == NULL) {
        lisp_object_t NULL_string = lisp_string_create_c("NULL");
        lisp_string_t NULL_string_value = lisp_string_get_value(NULL_string);
        return lisp_string_print_quoted(stream, NULL_string_value, lisp_NIL);
    }

    lisp_char_print_quoted(stream, char_paren_open, lisp_NIL);
    {
        lisp_print_for_cell(environment, stream, cell_value->car, lisp_NIL);
        lisp_char_print_quoted(stream, char_space, lisp_NIL);
        lisp_char_print_quoted(stream, char_period, lisp_NIL);
        lisp_char_print_quoted(stream, char_space, lisp_NIL);
        lisp_print_for_cell(environment, stream, cell_value->cdr, lisp_NIL);
    }
    lisp_char_print_quoted(stream, char_paren_close, lisp_NIL);

    return lisp_T;
}

lisp_object_t lisp_cell_print_dotted(lisp_object_t environment, lisp_object_t stream, lisp_cell_t cell_value, lisp_object_t compress_dots)
{
    if (compress_dots == lisp_NIL) {
        return lisp_cell_print_structurally(environment, stream, cell_value);
    } else {
        return lisp_cell_print_readably(environment, stream, cell_value);
    }
}


lisp_object_t lisp_cell_equal(lisp_object_t a, lisp_object_t b)
{
    if ((lisp_equal(lisp_cell_car(a), lisp_cell_car(b)) != lisp_NIL) &&
        (lisp_equal(lisp_cell_cdr(a), lisp_cell_cdr(b)) != lisp_NIL))
    {
        return lisp_T;
    } else {
        return lisp_NIL;
    }
}

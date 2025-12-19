/*
    File:       lisp_cell.h

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#ifndef __lisp_cell__
#define __lisp_cell__ 1


#include "lisp_types.h"


/**
 A Lisp cell.

 _Cells_ are the fundametal building block of most Lisp data structures,
 usually representing a singly-linked list where the first half of the
 cell, called the `CAR`, contains some data and the second half of the
 cell, called the `CDR`, contains a pointer to the next cell.
 */
typedef struct lisp_cell {
    lisp_object_t car;
    lisp_object_t cdr;
} *lisp_cell_t;


/** Construct a Lisp cell with the given `CAR` and `CDR`. */
LISP_EXTERN lisp_object_t lisp_cell_cons(lisp_object_t car, lisp_object_t cdr);

/** Gets the `CAR` of the given cell. */
LISP_EXTERN lisp_object_t lisp_cell_car(lisp_object_t cell);

/** Gets the `CDR` of the given cell. */
LISP_EXTERN lisp_object_t lisp_cell_cdr(lisp_object_t cell);

/** Replaces the `CAR` of the given cell, returning the cell. */
LISP_EXTERN lisp_object_t lisp_cell_rplaca(lisp_object_t cell, lisp_object_t newcar);

/** Replaces the `CDR` of the given cell, returning the cell. */
LISP_EXTERN lisp_object_t lisp_cell_rplacd(lisp_object_t cell, lisp_object_t newcdr);

/**
 Constructs a series of Lisp cells where each `car` points to a
 provided value and each `cdr` points to the next cell, until a `cdr`
 contains `lisp_nil`. The arguments are terminated by `lisp_nil`.
 */
LISP_EXTERN lisp_object_t lisp_cell_list(lisp_object_t car, ...);

/**  Gets the cell value of the given Lisp object. */
LISP_EXTERN lisp_cell_t lisp_cell_get_value(lisp_object_t object);

/** Prints the cell to the given output stream, with dot compression. */
LISP_EXTERN lisp_object_t lisp_cell_print(lisp_object_t environment,
                                          lisp_object_t stream,
                                          lisp_cell_t cell_value);

/**
 Prints the cell to the given output stream, with control over the
 compression of dots.
 */
LISP_EXTERN lisp_object_t lisp_cell_print_dotted(lisp_object_t environment,
                                                 lisp_object_t stream,
                                                 lisp_cell_t cell_value,
                                                 lisp_object_t compress_dots);

/**
 Compares two cells.

 Two cells are equal if their `CAR` and `CDR` are equal, recursively.
 */
LISP_EXTERN lisp_object_t lisp_cell_equal(lisp_object_t a, lisp_object_t b);


#endif  /* __lisp_cell__ */

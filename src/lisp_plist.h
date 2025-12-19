/*
    File:       lisp_plist.h

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#ifndef __lisp_plist__
#define __lisp_plist__ 1


#include "lisp_types.h"


/**
 Creates a property list.

 A *property list* is a list whose every `car` is a cell, with an
 atomic symbol in its `car` and a direct value in its `cdr`.

 One or more cells may be passed to this function, terminated by the
 C `NULL` value. This is necessary because technically `lisp_nil` is
 a valid value for a cell (the empty list).
 */
LISP_EXTERN lisp_object_t lisp_plist_create(lisp_object_t cell, ...);

/**
 Gets the value for the given \a symbol in the property list \a plist.
 */
LISP_EXTERN lisp_object_t lisp_plist_get(lisp_object_t plist,
                                         lisp_object_t symbol);

/**
 Sets the \a value for the given \a symbol in the property list
 / \a plist, returning the given \a value.
 */
LISP_EXTERN lisp_object_t lisp_plist_set(lisp_object_t plist,
                                         lisp_object_t symbol,
                                         lisp_object_t value);

/**
 Removes the entry for the given \a symbol from the property list.
 */
LISP_EXTERN lisp_object_t lisp_plist_remprop(lisp_object_t plist,
                                             lisp_object_t symbol);

/**
 Find the cell in the plist whose `CAR` has the given symbol.

 Searches through the plist for a cell whose `CAR` is equal to the given
 symbol, and either returns that cell or the final cell in the plist.
 This ensures that if a cell is not found, if the next operation is to
 add one, it can be done without iterating over the entire plist again.

 - Returns: Returns `1` if a symbol is found, with the address of that
 cell in `entry`; or `0` if a symbol is not found, with the address of
 the final cell in the plist in `entry` for ease of append.
 */
LISP_EXTERN int lisp_plist_find_entry(lisp_object_t plist,
                                      lisp_object_t symbol,
                                      lisp_object_t *entry);


#endif  /* __lisp_plist__ */

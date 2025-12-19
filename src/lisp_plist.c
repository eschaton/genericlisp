/*
    File:       lisp_plist.c

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#include "lisp_plist.h"

#include "lisp_atom.h"
#include "lisp_cell.h"
#include "lisp_environment.h"
#include "lisp_printing.h"

#if LISP_USE_STDLIB
#include <stdarg.h>
#include <stdlib.h>
#endif


lisp_object_t lisp_plist_create(lisp_object_t entry, ...)
{
    lisp_object_t plist = lisp_cell_cons(entry, lisp_NIL);
    lisp_object_t plist_cur = plist;
    int done = 0;

    va_list ap;
    va_start(ap, entry);
    do {
        lisp_object_t next_entry = va_arg(ap, lisp_object_t);
        if (next_entry != NULL) {
            /*
             There's another entry, put it in its own cell and append it to the
             current cell by changing the current cell's CDR.
             */
            lisp_object_t next_cell = lisp_cell_cons(next_entry, lisp_NIL);
            lisp_cell_rplacd(plist_cur, next_cell);
            plist_cur = next_cell;
        } else {
            /*
             The current cell always has a CDR of NIL, so we don't need to change
             it, we can just declare ourselves done.
             */
            done = 1;
        }
    } while (!done);
    va_end(ap);

    return plist;
}


int lisp_plist_find_entry(lisp_object_t plist,
                          lisp_object_t symbol,
                          lisp_object_t *entry)
{
    lisp_object_t plist_cur = plist;

    /* Iterate over all of the pairs in the list until found or end. */
    while (1) {
        lisp_object_t check_cell = lisp_cell_car(plist_cur);

        /*
         If the CAR of the cell to check is a match for our symbol, then put
         the that cell in *entry and return 1.
         */
        lisp_object_t potential_symbol = lisp_cell_car(check_cell);
        if (lisp_equal(symbol, potential_symbol) != lisp_NIL) {
            *entry = check_cell;
            return 1;
        }

        /*
         If we're not at the end of the plist, prepare to check the next
         cell in the plist.

         At the end of the plist, plist_cur's CDR will be NIL and we should
         put plist_cur in *entry and return 0 to indicate not found.
         */
        lisp_object_t next_cell = lisp_cell_cdr(plist_cur);
        if (next_cell != lisp_NIL) {
            plist_cur = next_cell;
        } else {
            *entry = plist_cur;
            return 0;
        }
    }
}


lisp_object_t lisp_plist_get(lisp_object_t plist,
                             lisp_object_t symbol)
{
    /* Look for an entry whose key is the given symbol. */
    lisp_object_t found_entry = NULL;
    int found = lisp_plist_find_entry(plist, symbol, &found_entry);

    if (found) {
        /* Get the value for the existing entry. */
        return lisp_cell_cdr(found_entry);
    } else {
        /* A non-existent entry is the same as NIL. */
        return lisp_NIL;
    }
}


lisp_object_t lisp_plist_set(lisp_object_t plist,
                             lisp_object_t symbol,
                             lisp_object_t value)
{
    /* Look for an entry whose key is the given symbol. */
    lisp_object_t found_entry = NULL;
    int found = lisp_plist_find_entry(plist, symbol, &found_entry);

    if (found) {
        /* Set a new value for the existing entry. */
        lisp_cell_rplacd(found_entry, value);
    } else {
        /* Append a new (symbol . value) entry to the plist. */
        lisp_object_t new_pair = lisp_cell_cons(symbol, value);
        lisp_object_t new_entry = lisp_cell_cons(new_pair, lisp_NIL);
        lisp_cell_rplacd(found_entry, new_entry);
    }
    return value;
}


lisp_object_t lisp_plist_remprop(lisp_object_t plist,
                                 lisp_object_t symbol)
{
#warning lisp_plist_remprop: Implement for real.
    /*
     Just set the value of the given property to nil. That will have the
     same effect as removing the property's cell, less efficiently.
     */
    lisp_plist_set(plist, symbol, lisp_NIL);
    return lisp_NIL;
}

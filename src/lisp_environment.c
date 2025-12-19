/*
    File:       lisp_environment.c

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#include "lisp_environment.h"

#include "lisp_atom.h"
#include "lisp_cell.h"
#include "lisp_evaluation.h"
#include "lisp_memory.h"
#include "lisp_plist.h"
#include "lisp_stream.h"
#include "lisp_string.h"
#include "lisp_subr.h"

#include "lisp_built_in_sforms.h"
#include "lisp_built_in_streams.h"
#include "lisp_built_in_subrs.h"


/* Well-known atoms that are always in a root environment. */

lisp_object_t lisp_T = NULL;
lisp_object_t lisp_NIL = NULL;

lisp_object_t lisp_TERMINAL_IO = NULL;
lisp_object_t lisp_STANDARD_INPUT = NULL;
lisp_object_t lisp_STANDARD_OUTPUT = NULL;

lisp_object_t lisp_PNAME = NULL;
lisp_object_t lisp_EXPR = NULL;
lisp_object_t lisp_SUBR = NULL;
lisp_object_t lisp_APVAL = NULL;

static lisp_object_t lisp_SI_PARENT_ENVIRONMENT = NULL;


lisp_object_t lisp_environment_create(lisp_object_t parent)
{
    /*
     Create an "empty" environemnt plist "manually", for easy bootstrapping:

     ((%SI:*PARENT-ENVIRONMENT* . ((APVAL . PARENT))))

     Note that the parent pointer doesn't need a PNAME, it's already in the
     root environment.
     */

    lisp_object_t parent_apval_cell = lisp_cell_cons(lisp_APVAL, parent);
    lisp_object_t parent_plist = lisp_plist_create(parent_apval_cell, NULL);
    lisp_object_t parent_cell = lisp_cell_cons(lisp_SI_PARENT_ENVIRONMENT, parent_plist);
    lisp_object_t environment = lisp_plist_create(parent_cell, NULL);

    return environment;
}


void lisp_environment_dispose(lisp_object_t environment)
{
    /*
     Just do nothing for now; environments are GC'd just like everything
     else, and there's no other teardown supported by Lisp. (We also don't
     have bidirectional links between environments, so we don't need to go
     to any lengths to break those.)
     */
}


lisp_object_t lisp_environment_parent(lisp_object_t environment)
{
    /*
     This function goes to the environment plist directly, rather than use
     environment symbol lookup itself, since it's used in the process of
     environment symbol lookup.
     */
    lisp_object_t parent_plist = lisp_plist_get(environment, lisp_SI_PARENT_ENVIRONMENT);
    if (parent_plist == lisp_NIL) {
        return lisp_NIL;
    } else {
        return lisp_plist_get(parent_plist, lisp_APVAL);
    }
}


lisp_object_t lisp_environment_find_symbol(lisp_object_t environment,
                                           lisp_object_t symbol,
                                           lisp_object_t recursive)
{
    /*
     An environment is a plist, so look through it using our standard plist
     traversal function for the requested symbol's entry.
     */
    lisp_object_t entry;
    int found = lisp_plist_find_entry(environment, symbol, &entry);
    if (found) {
        /* An entry was found, so return the symbol's entire entry. */
        return entry;
    } else {
        /* An entry was not found, so either recurse or return NIL. */
        if (recursive == lisp_NIL) {
            return lisp_NIL;
        } else {
            lisp_object_t parent_environment = lisp_environment_parent(environment);
            if (parent_environment == lisp_NIL) {
                return lisp_NIL;
            } else {
                return lisp_environment_find_symbol(parent_environment, symbol, recursive);
            }
        }
    }
}

lisp_object_t lisp_environment_get_symbol_value(lisp_object_t environment,
                                                lisp_object_t symbol,
                                                lisp_object_t type,
                                                lisp_object_t recursive)
{
    lisp_object_t found_symbol = lisp_environment_find_symbol(environment, symbol, recursive);
    if (found_symbol == lisp_NIL) {
        return lisp_NIL;
    } else {
        lisp_object_t plist = lisp_cell_cdr(found_symbol);
        return lisp_plist_get(plist, type);
    }
}

lisp_object_t lisp_environment_set_symbol_value(lisp_object_t environment,
                                                lisp_object_t symbol,
                                                lisp_object_t type,
                                                lisp_object_t value,
                                                lisp_object_t recursive)
{
    lisp_object_t found_symbol = lisp_environment_find_symbol(environment, symbol, recursive);
    lisp_object_t plist = lisp_cell_cdr(found_symbol);
    if (plist == lisp_NIL) {
        /* There was no plist, create it. */
        lisp_object_t symbol_type_value_cell = lisp_cell_cons(type, value);
        lisp_object_t symbol_plist = lisp_plist_create(symbol_type_value_cell, NULL);
        lisp_plist_set(environment, symbol, symbol_plist);
    } else {
        /* There was a plist, update it. */
        lisp_plist_set(plist, type, value);
    }
    return value;
}

/**
 "Intern" a symbol for the given atom in the environment, using `NIL` as
 its `APVAL` since being interned doesn't necessarily mean being bound.
 */
lisp_object_t lisp_environment_intern_symbol(lisp_object_t environment,
                                             lisp_object_t atom)
{
    lisp_environment_set_symbol_value(environment,
                                      atom,
                                      lisp_APVAL, lisp_NIL,
                                      lisp_NIL);
    return atom;
}


lisp_object_t lisp_environment_create_root(void)
{
    /*
     Create the "raw" root environment property list, where each symbol is
     its own APVAL.

         ((T . ((PNAME . "T")
                (APVAL . T)))
          (NIL . ((PNAME . "NIL")
                  (APVAL . NIL)))
          (PNAME . ((PNAME . "PNAME")
                    (APVAL . PNAME)))
          (APVAL . ((PNAME . "APVAL")
                    (APVAL . APVAL)))
          (EXPR . ((PNAME . "EXPR")
                   (APVAL . EXPR)))
          (SUBR . ((PNAME . "SUBR")
                   (APVAL . SUBR)))
          (%SI:*PARENT-ENVIRONMENT* . ((PNAME . "%SI:*PARENT-ENVIRONMENT*")
                                       (APVAL . NIL))))

     It must be created "manually" in order to bootstrap everything else,
     because some of the things in it are used as keys in an environment
     property list itself.

     Note that the symbol for %SI:*PARENT-ENVIRONMENT* is registered with
     an APVAL of NIL to indicate that this environment has no parent; a
     non-root environment has its parent environment as its APVAL.
     */

    lisp_object_t lisp_T_name = lisp_string_create_c("T");
    lisp_object_t lisp_NIL_name = lisp_string_create_c("NIL");
    lisp_object_t lisp_PNAME_name = lisp_string_create_c("PNAME");
    lisp_object_t lisp_APVAL_name = lisp_string_create_c("APVAL");
    lisp_object_t lisp_EXPR_name = lisp_string_create_c("EXPR");
    lisp_object_t lisp_SUBR_name = lisp_string_create_c("SUBR");
    lisp_object_t lisp_parent_name = lisp_string_create_c("%SI:PARENT-ENVIRONMENT");

    lisp_T = lisp_atom_create(lisp_T_name);
    lisp_NIL = lisp_atom_create(lisp_NIL_name);
    lisp_PNAME = lisp_atom_create(lisp_PNAME_name);
    lisp_APVAL = lisp_atom_create(lisp_APVAL_name);
    lisp_EXPR = lisp_atom_create(lisp_EXPR_name);
    lisp_SUBR = lisp_atom_create(lisp_SUBR_name);
    lisp_SI_PARENT_ENVIRONMENT = lisp_atom_create(lisp_parent_name);

    lisp_object_t lisp_T_plist = lisp_plist_create(lisp_cell_cons(lisp_PNAME, lisp_T_name),
                                                   lisp_cell_cons(lisp_APVAL, lisp_T),
                                                   NULL);
    lisp_object_t lisp_NIL_plist = lisp_plist_create(lisp_cell_cons(lisp_PNAME, lisp_NIL_name),
                                                     lisp_cell_cons(lisp_APVAL, lisp_NIL),
                                                     NULL);
    lisp_object_t lisp_PNAME_plist = lisp_plist_create(lisp_cell_cons(lisp_PNAME, lisp_PNAME_name),
                                                       lisp_cell_cons(lisp_APVAL, lisp_PNAME),
                                                       NULL);
    lisp_object_t lisp_APVAL_plist = lisp_plist_create(lisp_cell_cons(lisp_PNAME, lisp_APVAL_name),
                                                       lisp_cell_cons(lisp_APVAL, lisp_APVAL),
                                                       NULL);
    lisp_object_t lisp_EXPR_plist = lisp_plist_create(lisp_cell_cons(lisp_PNAME, lisp_EXPR_name),
                                                      lisp_cell_cons(lisp_APVAL, lisp_EXPR),
                                                      NULL);
    lisp_object_t lisp_SUBR_plist = lisp_plist_create(lisp_cell_cons(lisp_PNAME, lisp_SUBR_name),
                                                      lisp_cell_cons(lisp_APVAL, lisp_SUBR),
                                                      NULL);
    lisp_object_t lisp_PARENT_plist = lisp_plist_create(lisp_cell_cons(lisp_PNAME, lisp_parent_name),
                                                        lisp_cell_cons(lisp_APVAL, lisp_NIL),
                                                        NULL);

    lisp_object_t environment = lisp_cell_list(lisp_cell_cons(lisp_T, lisp_T_plist),
                                               lisp_cell_cons(lisp_NIL, lisp_NIL_plist),
                                               lisp_cell_cons(lisp_PNAME, lisp_PNAME_plist),
                                               lisp_cell_cons(lisp_APVAL, lisp_APVAL_plist),
                                               lisp_cell_cons(lisp_EXPR, lisp_EXPR_plist),
                                               lisp_cell_cons(lisp_SUBR, lisp_SUBR_plist),
                                               lisp_cell_cons(lisp_SI_PARENT_ENVIRONMENT, lisp_PARENT_plist),
                                               lisp_NIL);

    /*
     Now that the rawest portion of the root environment has been
     established, register the built-in special forms and SUBRs.
     */

    lisp_environment_add_built_in_special_forms(environment);
    lisp_environment_add_built_in_SUBRs(environment);

    /*
     The root environment is preserved from modification by the creation
     of a child environment which is actually what gets returned. That way
     every caller doesn't need to do this itself, and we can alsop set up
     well-known mutable symbols in it instead of hte root.
     */

    lisp_object_t mutable_environment = lisp_environment_create(environment);

    /*
     Set up the built-in streams for our current environment.
     */
    lisp_environment_add_built_in_streams(mutable_environment);

    return mutable_environment;
}

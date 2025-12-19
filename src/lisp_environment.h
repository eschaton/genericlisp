/*
    File:       lisp_environment.h

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#ifndef __lisp_environment__
#define __lisp_environment__ 1


#include "lisp_types.h"


/**
 Create a Lisp environment that descends from a specified parent.

 An _environment_ is a context in which symbol

 - Note: An environment created with this function **must** have a
         `parent` environment. A root environment **must** be created
         using `lisp_environment_create_root` instead.
 */
LISP_EXTERN lisp_object_t lisp_environment_create(lisp_object_t parent);

/**
 Create a Lisp environment that descends from no parent, and contains
 all of the baseline definitions for the system.

 It should be the only environment that, when asked for its parent,
 returns `NIL`.
 */
LISP_EXTERN lisp_object_t lisp_environment_create_root(void);

/** Dispose of a Lisp environment. */
LISP_EXTERN void lisp_environment_dispose(lisp_object_t environment);

/** Get the parent environment of a Lisp environment. */
LISP_EXTERN lisp_object_t lisp_environment_parent(lisp_object_t environment);


/**
 Look up a symbol in the given environment and return its plist.

 - Parameters:
   - environment: The environment in which to look up the symbol.
   - symbol: The atom representing the symbol to look up.
   - recursive: Whether to look up the symbol in parent environments.

 - Returns: The symbol's environment entry, or `NIL` if the symbol was
            not found.
 */
LISP_EXTERN lisp_object_t lisp_environment_find_symbol(lisp_object_t environment,
                                                       lisp_object_t symbol,
                                                       lisp_object_t recursive);

/**
 Get the requested type of value for a symbol in the given environment,
 going up the parent environment chain as necessary to find the symbol.

 - Parameters:
   - environment: The environment in which to look up the symbol.
   - symbol: The atom representing the symbol to look up.
   - type: The specific type of value to lookup up, such as `PNAME`,
           `APVAL`, `SUBR`, etc.
   - recursive: Whether to search parent environments for the symbol.
 - Returns: The requested type of value for the symbol, or `NIL`. (It is
            not possible to distinguish set-to-`NIL` from unset.)
 */
LISP_EXTERN lisp_object_t lisp_environment_get_symbol_value(lisp_object_t environment,
                                                            lisp_object_t symbol,
                                                            lisp_object_t type,
                                                            lisp_object_t recursive);

/**
 Set the specified type of value for a symbol in the given environment,
 or (if requestsed) in whatever parent environment contains it.

 - Parameters:
   - environment: The environment in which to look up the symbol.
   - symbol: The atom representing the symbol to look up.
   - type: The specific type of value to lookup up, such as APVAL, SUBR,
           and so on.
   - value: The value to set.
   - recursive: Whether to search parent environments for the symbol.
 - Returns: The value that was set.
 */
LISP_EXTERN lisp_object_t lisp_environment_set_symbol_value(lisp_object_t environment,
                                                            lisp_object_t symbol,
                                                            lisp_object_t type,
                                                            lisp_object_t value,
                                                            lisp_object_t recursive);

/**
 "Intern" a symbol for the given atom in the environment, using it as
 its own `APVAL`.
 */
LISP_EXTERN lisp_object_t lisp_environment_intern_symbol(lisp_object_t environment,
                                                         lisp_object_t atom);


/**
 The well-known `T` symbol, always in the root environment.

 It is recursively defined; it is its own `APVAL`.
 */
LISP_EXTERN lisp_object_t lisp_T;

/**
 The well-known `NIL` symbol, always in the root environment.

 It is recursively defined; it is its own `APVAL`.
 */
LISP_EXTERN lisp_object_t lisp_NIL;


/**
 The well-known `PNAME` symbol.

 This is the plist key for the "print name" a symbol represents.
 */
LISP_EXTERN lisp_object_t lisp_PNAME;

/**
 The well-known `EXPR` symbol.

 This is the plist key for the interpretable function (in the form of
 S-expressions) that a symbol represents.
 */
LISP_EXTERN lisp_object_t lisp_EXPR;

/**
 The well-known `SUBR` symbol.

 This is the plist key for the compiled function a symbol represents.
 */
LISP_EXTERN lisp_object_t lisp_SUBR;

/**
 The well-known `APVAL` symbol.

 This is the plist key for the variable a symbol represents.
 */
LISP_EXTERN lisp_object_t lisp_APVAL;


/**
 The well-known `*TERMINAL-IO*` symbol.

 Its `APVAL` is the stream from which input comes by default when
 passing `T` in place of a stream in `read`, and to which output goes by
 default when passing `T` in place of a stream in `write`.

 - Warning: This is a symbol, not a stream! It must be looked up in
            the current environment!
 */
LISP_EXTERN lisp_object_t lisp_TERMINAL_IO;

/**
 The well-known `*STANDARD-INPUT*` symbol.

 Its `APVAL` is the stream from which input comes by default when
 passing `NIL` in place of a stream in `read`.

 - Warning: This is a symbol, not a stream! It must be looked up in
            the current environment!
 */
LISP_EXTERN lisp_object_t lisp_STANDARD_INPUT;

/**
 The well-known `*STANDARD-OUTPUT*` symbol.

 Its `APVAL` is the stream to which output goes by default when
 passing `NIL` in place of a stream in `write`.

 - Warning: This is a symbol, not a stream! It must be looked up in
            the current environment!
 */
LISP_EXTERN lisp_object_t lisp_STANDARD_OUTPUT;


#endif  /* __lisp_environment__ */

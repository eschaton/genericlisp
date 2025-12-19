/*
    File:       lisp_reading.h

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#ifndef __lisp_reading__
#define __lisp_reading__ 1


#include "lisp_types.h"


/**
 Read a Lisp object from the given input stream, or `*STANDARD-INPUT*`
 if `NIL`.

 The Lisp reader can read the following types of objects:

 - Atoms, introduced by a non-numeric, non-syntactic printing character;
   this includes keywords introduced by a colon.
 - Fixnums, optionally introduced by a plus or minus;
 - Lists, delimited by parentheses, with a period introducing a
   non-`NIL` `CDR`;
 - Strings, delimited by double-quotes with backslash escaping of
   a small number of special characters (`b`, `e`, `n`, `t`);
 - Vectors, introduced by an octothorpe and delimited by parentheses;
 - Characters, introduced by an octothorpe followed by a backslash.

 Whitespace and end-of-line comments are also handled, but not returned
 in any way to the caller.

 - Parameters:
   - environment: The environment in which the read takes place.
   - stream: The stream to read from, or `T` for `*STANDARD-INPUT*`.
   - recursivep: `T` if `lisp_read` is being invoked recursively, `NIL`
                 otherwise.

 - Returns: The form that was read.
 */
LISP_EXTERN lisp_object_t lisp_read(lisp_object_t environment,
                                    lisp_object_t stream,
                                    lisp_object_t recursivep);


#endif  /* __lisp_reading__ */

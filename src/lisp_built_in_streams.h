/*
    File:       lisp_built_in_streams.h

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#ifndef __lisp_built_in_streams__
#define __lisp_built_in_streams__ 1


#include "lisp_stream.h"


#if LISP_USE_STDLIB

#include <stdio.h>


/**
 Gets stream functions for C `stdio` on the given `FILE`.
 - Warning: If you use `stdin` or `stdout`, you must still open the
            stream for reading or writing as appropriate, and **must**
            not open it for unsupported operations.
 */
LISP_EXTERN lisp_object_t lisp_stream_functions_stdio(FILE *file);


/**
 Gets stream functions for C `stdio` on the given `FILE` pair.
 - Warning: If you use `stdin` or `stdout`, you must still open the
            stream for reading or writing as appropriate, and **must**
            not open it for unsupported operations.
 */
LISP_EXTERN lisp_object_t lisp_stream_functions_stdio_pair(FILE *input, FILE *output);


#endif /* LISP_USE_STDLIB */


/**
 Create the `*TERMINAL-IO*`, `*STANDARD-INPUT*` and `*STANDARD-OUTPUT*` streams in the environment, connected to the appropriate underlying constructs for our system.
 */
LISP_EXTERN void lisp_environment_add_built_in_streams(lisp_object_t environment);


#endif  /* __lisp_built_in_streams__ */

/*
    File:       lisp_printing.h

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#ifndef __lisp_printing__
#define __lisp_printing__ 1


#include "lisp_types.h"


/**
 Print a Lisp object to the given output stream.

 - Parameters:
   - environment: The environment providing context for printing, such
                  as radix settings and `*STANDARD-OUTPUT*`
   - stream: The stream to print on, or `T` for `*STANDARD-OUTPUT*`.
   - object: The object to print.
 - Returns: `T` upon success, `NIL` upon failure.
 */
LISP_EXTERN lisp_object_t lisp_print(lisp_object_t environment, lisp_object_t stream, lisp_object_t object);


#endif  /* __lisp_printing__ */

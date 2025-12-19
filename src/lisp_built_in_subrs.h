/*
    File:       lisp_built_in_subrs.h

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#ifndef __lisp_built_in_subrs__
#define __lisp_built_in_subrs__ 1


#include "lisp_types.h"


/**
 Add the built-in `SUBR` instances to the given environment, which
 should be a root environment or one level above it.
 */
LISP_EXTERN void lisp_environment_add_built_in_SUBRs(lisp_object_t environment);


#endif  /* __lisp_built_in_subrs__ */

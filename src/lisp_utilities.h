/*
    File:       lisp_utilities.h

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#ifndef __lisp_utilities__
#define __lisp_utilities__ 1


#include "lisp_types.h"


/**
 Round \a value up to the next multiple of \a multiple.
 */
#define lisp_round_to_next_multiple(value, multiple) \
    (((value) <= (multiple)) ? (multiple) \
                             : (((value / multiple) + 1) * multiple))


#endif  /* __lisp_utilities__ */

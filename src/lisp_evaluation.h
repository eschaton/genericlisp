/*
    File:       lisp_evaluation.h

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#ifndef __lisp_evaluation__
#define __lisp_evaluation__ 1


#include "lisp_types.h"


/**
 Initialize an evaluation environment.

 This is necessary only when establishing a root environment. It ensures
 that everything necessary for evaluation is set up, so it doesn't need
 to be set up on the fly.
 */
LISP_EXTERN lisp_object_t lisp_eval_initialize(lisp_object_t environment);

/**
 Evaluate a Lisp object, returning a Lisp object as the result.

 - Parameters:
   - environment: The environment in which to perform the evaluation, as
                  well as the environment in which any side-effects take
                  place.
   - form: The Lisp form to evaluate, which can be any Lisp object.
 - Returns: The result of evaluating _form_ or `NIL` upon failure.
 */
LISP_EXTERN lisp_object_t lisp_eval(lisp_object_t environment,
                                    lisp_object_t form);


/**
 Applies a function to a list of arguments, returning a Lisp object as the result.
 - Parameters:
   - environment: The environment in which to perform the evaluation, as
                  well as the environment in which any side-effects take
                  place.
   - function: The Lisp function to apply, which must be either a cell
               containing a lambda list or a compiled function.
   - arguments: The list of arguments to which the function will be applied.
 - Returns: The result of applying _form_ to _arguments_ or `NIL` upon
            failure.
 */
LISP_EXTERN lisp_object_t lisp_apply(lisp_object_t environment,
                                     lisp_object_t function,
                                     lisp_object_t arguments);


#endif  /* __lisp_evaluation__ */

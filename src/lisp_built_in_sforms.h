/*
    File:       lisp_built_in_sforms.h

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#ifndef __lisp_built_in_sforms__
#define __lisp_built_in_sforms__ 1


#include "lisp_types.h"


/**
 Indicate whether the given atom represents one of the built-in special
 forms, e.g. function-like invocations with special evaluation rules.
 */
LISP_EXTERN int lisp_eval_is_special_form(lisp_object_t special_form);

/**
 Evaluate one of the built-in special forms.
 */
LISP_EXTERN lisp_object_t lisp_eval_special_form(lisp_object_t environment,
                                                 lisp_object_t special_form,
                                                 lisp_object_t cell);

/**
 Add bindings for the built-in special forms to the given environment.
 */
LISP_EXTERN void lisp_environment_add_built_in_special_forms(lisp_object_t environment);


/* Well-known symbols represnting special forms. */
LISP_EXTERN lisp_object_t lisp_symbol_AND;
LISP_EXTERN lisp_object_t lisp_symbol_COND;
LISP_EXTERN lisp_object_t lisp_symbol_DEFINE;
LISP_EXTERN lisp_object_t lisp_symbol_DEFUN;
LISP_EXTERN lisp_object_t lisp_symbol_IF;
LISP_EXTERN lisp_object_t lisp_symbol_LAMBDA;
LISP_EXTERN lisp_object_t lisp_symbol_OR;
LISP_EXTERN lisp_object_t lisp_symbol_QUOTE;
LISP_EXTERN lisp_object_t lisp_symbol_SET;
LISP_EXTERN lisp_object_t lisp_symbol_SETQ;

LISP_EXTERN lisp_object_t lisp_symbol_BLOCK;
LISP_EXTERN lisp_object_t lisp_symbol_RETURN_FROM;
LISP_EXTERN lisp_object_t lisp_symbol_RETURN;

LISP_EXTERN lisp_object_t lisp_symbol_TAGBODY;
LISP_EXTERN lisp_object_t lisp_symbol_GO;


#endif  /* __lisp_built_in_sforms__ */

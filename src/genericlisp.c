/*
    File:       genericlisp.c

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#include "genericlisp.h"

#if LISP_USE_STDLIB
#include <stdlib.h>
#include <stdio.h>
#include <sysexits.h>
#endif


#if LISP_BUILD_AS_LIBRARY
#define main genericlisp_main
LISP_EXTERN int genericlisp_main(int argc, char **argv);
#endif


static int genericlisp_done = 0;

static void lisp_print_banner(lisp_object_t environment);
static void lisp_print_prompt(lisp_object_t environment);
static void lisp_run_repl(lisp_object_t environment);

/* Cached string to represent a newline. */
static lisp_object_t lisp_string_newline = NULL;


int main(int argc, char **argv)
{
    /* Do not use any Lisp objects at all before this point. */

    lisp_heap_initialize(1024 * 1024);

    /*
     At this point there is a Lisp heap, but not a Lisp environment. Only
     certain things can be done in this state. In particular, nothing must
     *care about* references to T or NIL until the root environment is
     established.
     */

    lisp_object_t root_environment = lisp_environment_create_root();

    /* Create a child environment for non-root bindings to go into. */

    lisp_object_t environment = lisp_environment_create(root_environment);

    /* Cache a string to represent a newline since that's extremely common. */
    lisp_string_newline = lisp_string_create_c("\n");

    /*
     At this point, the root Lisp environment has been established and it
     is safe to reference all Lisp objects.
     */

    lisp_print_banner(environment);

    /*
     Run a READ-EVAL-PRINT loop!
     */

    do {
        lisp_run_repl(environment);
    } while (genericlisp_done == 0);

    /*
     Clean up.
     */

    lisp_environment_dispose(environment);

    /*
     No more Lisp references once again after this point, as the root
     environment is no longer available.
     */

    return EX_OK;
}


void lisp_print_banner(lisp_object_t environment)
{
    lisp_print(environment, lisp_T, lisp_string_create_c("genericlisp 0.1"));
    lisp_print(environment, lisp_T, lisp_string_newline);
    lisp_print(environment, lisp_T, lisp_string_create_c("Copyright (c) 2025 Christopher M. Hanson. All rights reserved."));
    lisp_print(environment, lisp_T, lisp_string_newline);
    lisp_print(environment, lisp_T, lisp_string_newline);
#if DEBUG
    lisp_print(environment, lisp_T, lisp_string_create_c("Initial environment:\n"));
    lisp_print(environment, lisp_T, environment);
    lisp_print(environment, lisp_T, lisp_string_newline);
    lisp_print(environment, lisp_T, lisp_string_newline);
#endif
}



void lisp_print_prompt(lisp_object_t environment)
{
    lisp_print(environment, lisp_T, lisp_string_newline);
    lisp_print(environment, lisp_T, lisp_string_create_c("> "));
    fflush(stdout);
}


void lisp_run_repl(lisp_object_t environment)
{
    /* Print a prompt. */
    lisp_print_prompt(environment);

    /* Read an input form. */
    lisp_object_t read_obj = lisp_read(environment, lisp_T, lisp_NIL);

    /* Separate the input form from the output. */
    lisp_print(environment, lisp_T, lisp_string_newline);

#if DEBUG
    /* If debugging, print the form that was read pre-evaluation. */
    lisp_print(environment, lisp_T, lisp_string_create_c("Read: "));
    lisp_print(environment, lisp_T, read_obj);
    lisp_print(environment, lisp_T, lisp_string_newline);
#endif

    /* Evaluate the input form. */
    lisp_object_t eval_obj = lisp_eval(environment, read_obj);

    lisp_print(environment, lisp_T, eval_obj);
}

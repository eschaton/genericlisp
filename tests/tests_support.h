/*
    File:       tests_support.h

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#ifndef __tests_support__
#define __tests_support__ 1

#include <stdbool.h>

#include <check.h>

#include "genericlisp.h"


LISP_EXTERN lisp_object_t tests_root_environment;


LISP_EXTERN lisp_object_t tests_read_stream;
LISP_EXTERN char *tests_read_buffer;

LISP_EXTERN void tests_set_read_buffer(char *value);
LISP_EXTERN void tests_clear_read_buffer(void);
LISP_EXTERN bool tests_eofp_read_buffer(void);


LISP_EXTERN lisp_object_t tests_write_stream;
LISP_EXTERN char *tests_write_buffer;

LISP_EXTERN void tests_clear_write_buffer(void);


LISP_EXTERN void tests_shared_setup(void);
LISP_EXTERN void tests_shared_teardown(void);


/* Test Suites */

LISP_EXTERN Suite *atom_suite(void);
LISP_EXTERN Suite *cell_suite(void);
LISP_EXTERN Suite *char_suite(void);
LISP_EXTERN Suite *environment_suite(void);
LISP_EXTERN Suite *evaluation_suite(void);
LISP_EXTERN Suite *fixnum_suite(void);
LISP_EXTERN Suite *plist_suite(void);
LISP_EXTERN Suite *stream_suite(void);
LISP_EXTERN Suite *string_suite(void);


#endif /* __tests_support__ */

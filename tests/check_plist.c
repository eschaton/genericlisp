/*
    File:       check_plist.c

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#include <check.h>

#include "genericlisp.h"

#include "tests_support.h"


lisp_object_t _A, _B, _C, _D;
lisp_object_t _ab, _cd;
lisp_object_t _plist;


void plist_setup(void)
{
    tests_shared_setup();

    /*
     Create a plist of the form:

         ((A . B) . ((C . D))

     Each CAR in the plist is a (key . value) pair.
     Each CDR in the plist is a plist or NIL to terminate it.
     */

    _A = lisp_atom_create_c("A");
    _B = lisp_atom_create_c("B");
    _C = lisp_atom_create_c("C");
    _D = lisp_atom_create_c("D");

    _ab = lisp_cell_cons(_A, _B);
    _cd = lisp_cell_cons(_C, _D);
    _plist = lisp_plist_create(_ab, _cd, NULL);
}

void plist_teardown(void)
{
    /* It's all in the Lisp heap so nothing to do. */

    tests_shared_teardown();
}


/* MARK: - Plists */

START_TEST(test_creation)
{
    ck_assert_int_eq(lisp_tag_cell, lisp_object_get_tag(_plist));
}
END_TEST

START_TEST(test_printing)
{
    lisp_print(tests_root_environment, tests_write_stream, _plist);
    ck_assert_str_eq("((A . B) (C . D))", tests_write_buffer);
    tests_clear_write_buffer();
}
END_TEST

START_TEST(test_simple_successful_retrieval)
{
    lisp_object_t a_value = lisp_plist_get(_plist, _A);
    ck_assert_ptr_eq(a_value, _B);

    lisp_object_t c_value = lisp_plist_get(_plist, _C);
    ck_assert_ptr_eq(c_value, _D);
}
END_TEST

START_TEST(test_simple_failed_retrieval)
{
    lisp_object_t b_value = lisp_plist_get(_plist, _B);
    ck_assert_ptr_eq(b_value, lisp_NIL);
}
END_TEST


/* MARK: - Test Infrastructure */

Suite *plist_suite(void)
{
    Suite *s = suite_create("Plist");

    TCase *tc_plists = tcase_create("Plists");
    tcase_add_checked_fixture(tc_plists, plist_setup, plist_teardown);
    tcase_add_test(tc_plists, test_creation);
    tcase_add_test(tc_plists, test_printing);
    tcase_add_test(tc_plists, test_simple_successful_retrieval);
    tcase_add_test(tc_plists, test_simple_failed_retrieval);
    suite_add_tcase(s, tc_plists);

    return s;
}

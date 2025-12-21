/*
    File:       check_environment.c

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#include <check.h>

#include "genericlisp.h"

#include "tests_support.h"


/* MARK: - Root */

START_TEST(test_root_environment_creation)
{
    lisp_object_t environment = lisp_environment_parent(tests_root_environment);

    ck_assert_ptr_ne(lisp_NIL, environment);
    ck_assert_int_eq(lisp_tag_cell, lisp_object_get_tag(environment));
}
END_TEST

START_TEST(test_root_environment_has_t)
{
    lisp_object_t environment = lisp_environment_parent(tests_root_environment);

    lisp_object_t t_plist = lisp_plist_get(environment, lisp_T);
    ck_assert_ptr_ne(NULL, t_plist);
    ck_assert_int_eq(lisp_tag_cell, lisp_object_get_tag(t_plist));

    lisp_object_t t_pname = lisp_plist_get(t_plist, lisp_PNAME);
    ck_assert_ptr_ne(NULL, t_pname);
    ck_assert_int_eq(lisp_tag_string, lisp_object_get_tag(t_pname));

    lisp_string_t t_pname_string = lisp_string_get_value(t_pname);
    lisp_string_print_quoted(tests_write_stream, t_pname_string, lisp_NIL);
    ck_assert_str_eq("T", tests_write_buffer);
    tests_clear_write_buffer();

    lisp_object_t t_apval = lisp_plist_get(t_plist, lisp_APVAL);
    ck_assert_ptr_ne(NULL, t_apval);
    ck_assert_int_eq(lisp_tag_atom, lisp_object_get_tag(t_apval));
    ck_assert(lisp_equal(lisp_T, t_apval) != lisp_NIL);
    lisp_print(environment, tests_write_stream, t_apval);
    ck_assert_str_eq("T", tests_write_buffer);
    tests_clear_write_buffer();
}
END_TEST


/* MARK: - Nested */

START_TEST(test_nested_environment_gets_t_from_root)
{
    lisp_object_t environment = lisp_environment_create(tests_root_environment);

    lisp_object_t t_symbol = lisp_environment_find_symbol(environment, lisp_T, lisp_T);
    ck_assert_ptr_ne(NULL, t_symbol);
    ck_assert_int_eq(lisp_tag_cell, lisp_object_get_tag(t_symbol));

    lisp_object_t t_plist = lisp_cell_cdr(t_symbol);
    ck_assert_ptr_ne(NULL, t_plist);
    ck_assert_int_eq(lisp_tag_cell, lisp_object_get_tag(t_plist));

    lisp_object_t t_pname = lisp_plist_get(t_symbol, lisp_PNAME);
    ck_assert_ptr_ne(NULL, t_pname);
    ck_assert_int_eq(lisp_tag_string, lisp_object_get_tag(t_pname));

    lisp_string_t t_pname_string = lisp_string_get_value(t_pname);
    lisp_string_print_quoted(tests_write_stream, t_pname_string, lisp_NIL);
    ck_assert_str_eq("T", tests_write_buffer);
    tests_clear_write_buffer();

    lisp_object_t t_apval = lisp_plist_get(t_symbol, lisp_APVAL);
    ck_assert_ptr_ne(NULL, t_apval);
    ck_assert_int_eq(lisp_tag_atom, lisp_object_get_tag(t_apval));
    ck_assert(lisp_equal(lisp_T, t_apval) != lisp_NIL);
    lisp_print(environment, tests_write_stream, t_apval);
    ck_assert_str_eq("T", tests_write_buffer);
    tests_clear_write_buffer();
}
END_TEST


/* MARK: - Test Infrastructure */

Suite *environment_suite(void)
{
    Suite *s = suite_create("Environment");

    TCase *tc_root_environment = tcase_create("Root");
    tcase_add_checked_fixture(tc_root_environment, tests_shared_setup, tests_shared_teardown);
    tcase_add_test(tc_root_environment, test_root_environment_creation);
    suite_add_tcase(s, tc_root_environment);

    TCase *tc_nested_environment = tcase_create("Nested");
    tcase_add_checked_fixture(tc_nested_environment, tests_shared_setup, tests_shared_teardown);
    tcase_add_test(tc_nested_environment, test_nested_environment_gets_t_from_root);
    suite_add_tcase(s, tc_nested_environment);

    return s;
}

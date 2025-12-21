/*
    File:       check_atom.c

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#include <check.h>

#include <stdlib.h>
#include <string.h>

#include "genericlisp.h"

#include "tests_support.h"


/* MARK: - Atoms */

START_TEST(test_atom_creation)
{
    lisp_object_t object = lisp_atom_create_c("ABC");

    lisp_tag_t tag = lisp_object_get_tag(object);
    ck_assert_int_eq(lisp_tag_atom, tag);

    const char *name = (const char *)lisp_object_get_raw_value(object);
    ck_assert(strcmp("ABC", name) == 0);
}
END_TEST

START_TEST(test_atom_printing)
{
    lisp_object_t X_char = lisp_char_create('X');
    lisp_char_t X_char_value = lisp_char_get_value(X_char);
    lisp_char_print(tests_write_stream, X_char_value);

    lisp_object_t Y_char = lisp_char_create('Y');
    lisp_char_t Y_char_value = lisp_char_get_value(Y_char);
    lisp_char_print_quoted(tests_write_stream, Y_char_value, lisp_NIL);

    ck_assert_str_eq("XY", tests_write_buffer);
}
END_TEST

START_TEST(test_atom_equality)
{
    lisp_object_t a = lisp_char_create('A');
    lisp_object_t b = lisp_char_create('B');

    ck_assert(lisp_char_equal(a, a) != lisp_NIL);
    ck_assert(lisp_char_equal(b, b) != lisp_NIL);
    ck_assert(lisp_char_equal(a, b) == lisp_NIL);
    ck_assert(lisp_char_equal(b, a) == lisp_NIL);
}
END_TEST

START_TEST(test_atom_reading)
{
    lisp_object_t environment = tests_root_environment;
    lisp_object_t stream = tests_read_stream;

    tests_set_read_buffer("DEF");
    lisp_object_t read_object = lisp_read(environment, stream, lisp_NIL);
    ck_assert_ptr_ne(read_object, lisp_NIL);
    ck_assert_int_eq(lisp_tag_atom, lisp_object_get_tag(read_object));
    ck_assert_ptr_ne(lisp_equal(lisp_atom_create_c("DEF"), read_object), lisp_NIL);
    ck_assert(tests_eofp_read_buffer());

    tests_set_read_buffer("GHI \n ; comment \n JKL \n ; comment\n");
    lisp_object_t read_ghi = lisp_read(environment, stream, lisp_NIL);
    ck_assert_ptr_ne(read_ghi, lisp_NIL);
    ck_assert_ptr_ne(lisp_equal(lisp_atom_create_c("GHI"), read_ghi), lisp_NIL);
    lisp_object_t read_jkl = lisp_read(environment, stream, lisp_NIL);
    ck_assert_ptr_ne(read_jkl, lisp_NIL);
    ck_assert_ptr_ne(lisp_equal(lisp_atom_create_c("JKL"), read_jkl), lisp_NIL);
}
END_TEST


/* MARK: - Test Infrastructure */

Suite *atom_suite(void)
{
    Suite *s = suite_create("Atom");

    TCase *tc_atoms = tcase_create("Atoms");
    tcase_add_checked_fixture(tc_atoms, tests_shared_setup, tests_shared_teardown);
    tcase_add_test(tc_atoms, test_atom_creation);
    tcase_add_test(tc_atoms, test_atom_printing);
    tcase_add_test(tc_atoms, test_atom_equality);
    tcase_add_test(tc_atoms, test_atom_reading);
    suite_add_tcase(s, tc_atoms);

    return s;
}

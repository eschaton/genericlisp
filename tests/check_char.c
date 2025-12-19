/*
    File:       check_char.c

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#include <check.h>

#include <stdlib.h>
#include <string.h>

#include "genericlisp.h"

#include "tests_support.h"


/* MARK: - Chars */

START_TEST(test_char_creation)
{
    lisp_object_t object = lisp_char_create('A');

    lisp_tag_t tag = lisp_object_get_tag(object);
    ck_assert_int_eq(lisp_tag_char, tag);

    uintptr_t expectedValue = ((uintptr_t) 'A') << 4;
    uintptr_t value = lisp_object_get_raw_value(object);
    /* Use pointer equality to handle I32/LP64 platforms. */
    ck_assert_ptr_eq((void *)expectedValue, (void *)value);
}
END_TEST

START_TEST(test_char_printing)
{
    lisp_object_t environment = tests_root_environment;
    lisp_object_t stream = tests_write_stream;

    lisp_object_t X_char = lisp_char_create('X');
    lisp_char_t X_char_value = lisp_char_get_value(X_char);
    lisp_char_print(stream, X_char_value);

    lisp_object_t Y_char = lisp_char_create('Y');
    lisp_char_t Y_char_value = lisp_char_get_value(Y_char);
    lisp_char_print_quoted(stream, Y_char_value, lisp_NIL);

    ck_assert_str_eq("XY", tests_write_buffer);
}
END_TEST

START_TEST(test_char_equality)
{
    lisp_object_t a = lisp_char_create('A');
    lisp_object_t b = lisp_char_create('B');

    ck_assert(lisp_char_equal(a, a) != lisp_NIL);
    ck_assert(lisp_char_equal(b, b) != lisp_NIL);
    ck_assert(lisp_char_equal(a, b) == lisp_NIL);
    ck_assert(lisp_char_equal(b, a) == lisp_NIL);
}
END_TEST

START_TEST(test_char_reading)
{
    lisp_object_t environment = tests_root_environment;
    lisp_object_t stream = tests_read_stream;

    tests_set_read_buffer("#\\A");
    lisp_object_t read_object = lisp_read(environment, stream, lisp_NIL);
    ck_assert_ptr_ne(read_object, lisp_NIL);
    ck_assert_int_eq(lisp_tag_char, lisp_object_get_tag(read_object));
    ck_assert_ptr_eq(lisp_char_create('A'), read_object);
    ck_assert(tests_eofp_read_buffer());
}
END_TEST


/* MARK: - Test Infrastructure */

Suite *char_suite(void)
{
    Suite *s = suite_create("Char");

    TCase *tc_chars = tcase_create("Chars");
    tcase_add_checked_fixture(tc_chars, tests_shared_setup, tests_shared_teardown);
    tcase_add_test(tc_chars, test_char_creation);
    tcase_add_test(tc_chars, test_char_printing);
    tcase_add_test(tc_chars, test_char_equality);
    tcase_add_test(tc_chars, test_char_reading);
    suite_add_tcase(s, tc_chars);

    return s;
}


int main(int argc, char **argv)
{
    Suite *s = char_suite();

    return tests_main(argc, argv, s);
}

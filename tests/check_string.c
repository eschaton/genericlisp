/*
    File:       check_string.c

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#include <check.h>

#include "genericlisp.h"

#include "tests_support.h"


/* MARK: - Strings */

START_TEST(test_creation)
{
    lisp_object_t object = lisp_string_create_c("ABC");
    lisp_tag_t tag = lisp_object_get_tag(object);
    ck_assert_int_eq(lisp_tag_string, tag);

    lisp_string_t string = lisp_string_get_value(object);
    ck_assert_ptr_nonnull(string);

    ck_assert_ptr_nonnull(string->chars);
    ck_assert_int_eq(16, string->capacity);
    ck_assert_int_eq(3, string->length);

    ck_assert_int_eq(lisp_tag_interior, lisp_object_get_tag(string->chars));

    lisp_object_t *chars = lisp_interior_get_value(string->chars);
    ck_assert_ptr_eq(lisp_char_create('A'), chars[0]);
    ck_assert_ptr_eq(lisp_char_create('B'), chars[1]);
    ck_assert_ptr_eq(lisp_char_create('C'), chars[2]);
}
END_TEST

START_TEST(test_printing)
{
    lisp_object_t object = lisp_string_create_c("XYZ");

    lisp_string_t string_value = lisp_string_get_value(object);
    lisp_string_print(tests_write_stream, string_value);

    ck_assert_str_eq("XYZ", tests_write_buffer);
}
END_TEST

START_TEST(test_equality)
{
    lisp_object_t abc = lisp_string_create_c("ABC");
    lisp_object_t xyz = lisp_string_create_c("XYZ");

    ck_assert_ptr_ne(lisp_string_equal(abc, abc), lisp_NIL);
    ck_assert_ptr_ne(lisp_string_equal(xyz, xyz), lisp_NIL);
    ck_assert_ptr_eq(lisp_string_equal(abc, xyz), lisp_NIL);
    ck_assert_ptr_eq(lisp_string_equal(xyz, abc), lisp_NIL);

    lisp_object_t abc2 = lisp_string_create_c("ABC");
    lisp_object_t xyz2 = lisp_string_create_c("XYZ");

    ck_assert_ptr_ne(abc, abc2);
    ck_assert_ptr_ne(xyz, xyz2);

    ck_assert_ptr_ne(lisp_string_equal(abc, abc2), lisp_NIL);
    ck_assert_ptr_ne(lisp_string_equal(xyz, xyz2), lisp_NIL);

    lisp_object_t ab = lisp_string_create_c("AB");

    ck_assert_ptr_eq(lisp_string_equal(abc, ab), lisp_NIL);
}
END_TEST

START_TEST(test_reading)
{
    tests_set_read_buffer("; comment\n   \"ABC\"");
    lisp_object_t read_object = lisp_read(tests_root_environment, tests_read_stream, lisp_NIL);
    ck_assert_ptr_ne(read_object, lisp_NIL);
    ck_assert_int_eq(lisp_tag_string, lisp_object_get_tag(read_object));
    ck_assert_ptr_ne(lisp_equal(lisp_string_create_c("ABC"), read_object), lisp_NIL);
    ck_assert(tests_eofp_read_buffer());
}
END_TEST

START_TEST(test_reallocation)
{
    lisp_object_t *chars_buffer;
    lisp_object_t chars_interior = lisp_interior_create(sizeof(lisp_object_t) * 1, (void **)&chars_buffer);
    chars_buffer[0] = lisp_char_create('A');
    lisp_object_t string = lisp_string_create(chars_interior, 1, 1);
    lisp_string_t string_value = lisp_string_get_value(string);
    ck_assert_int_eq(1, string_value->capacity);
    ck_assert_int_eq(1, string_value->length);
    ck_assert_ptr_eq(chars_interior, string_value->chars);
    lisp_string_append_char(string, lisp_char_create('B'));
    ck_assert_int_eq(17, string_value->capacity);
    ck_assert_int_eq(2, string_value->length);
    ck_assert_ptr_ne(chars_interior, string_value->chars);
}
END_TEST


/* MARK: - Test Infrastructure */

Suite *string_suite(void)
{
    Suite *s = suite_create("String");

    TCase *tc_strings = tcase_create("Strings");
    tcase_add_checked_fixture(tc_strings, tests_shared_setup, tests_shared_teardown);
    tcase_add_test(tc_strings, test_creation);
    tcase_add_test(tc_strings, test_printing);
    tcase_add_test(tc_strings, test_equality);
    tcase_add_test(tc_strings, test_reading);
    tcase_add_test(tc_strings, test_reallocation);
    suite_add_tcase(s, tc_strings);

    return s;
}


int main(int argc, char **argv)
{
    Suite *s = string_suite();

    return tests_main(argc, argv, s);
}

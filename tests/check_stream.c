/*
    File:       check_stream.c

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#include <check.h>

#include <stdio.h>

#include "genericlisp.h"

#include "tests_support.h"


/* MARK: - Streams */

START_TEST(test_creation)
{
    lisp_object_t stream = tests_write_stream;
    ck_assert_ptr_nonnull(stream);

    lisp_tag_t tag = lisp_object_get_tag(stream);
    ck_assert_int_eq(lisp_tag_stream, tag);

    uintptr_t value = lisp_object_get_raw_value(stream);
    ck_assert_ptr_nonnull((void *)value);
}
END_TEST

START_TEST(test_writing_characters)
{
    lisp_object_t stream = tests_write_stream;
    ck_assert_ptr_nonnull(stream);

    lisp_stream_write_char(stream, lisp_char_create('A'));
    lisp_stream_write_char(stream, lisp_char_create('B'));
    lisp_stream_write_char(stream, lisp_char_create('C'));

    ck_assert_str_eq("ABC", tests_write_buffer);
}
END_TEST

START_TEST(test_writing_string)
{
    lisp_object_t stream = tests_write_stream;
    ck_assert_ptr_nonnull(stream);

    lisp_stream_write_string(stream, lisp_string_create_c("ABC"));

    ck_assert_str_eq("ABC", tests_write_buffer);
}
END_TEST

START_TEST(test_printing_interior)
{
    lisp_object_t environment = tests_root_environment;
    lisp_object_t output_stream = tests_write_stream;

    lisp_interior_t interior_value = NULL;
    lisp_object_t interior = lisp_interior_create(16, (void **)&interior_value);
    lisp_print(environment, output_stream, interior);

    char expected[256];
    snprintf(expected, 256, "#<INTERIOR 0x%llX>", (unsigned long long)interior_value);
    ck_assert_str_eq(expected, tests_write_buffer);
}
END_TEST

START_TEST(test_printing_structure)
{
    lisp_object_t environment = tests_root_environment;
    lisp_object_t output_stream = tests_write_stream;

    lisp_object_t structure = lisp_struct_create(stdin, sizeof(FILE *), 0);
    lisp_struct_t structure_value = lisp_struct_get_value(structure);
    lisp_print(environment, output_stream, structure);

    char expected[256];
    snprintf(expected, 256, "#<STRUCT 0x%llX>", (unsigned long long)structure_value);
    ck_assert_str_eq(expected, tests_write_buffer);
}
END_TEST


/* MARK: - Test Infrastructure */

Suite *stream_suite(void)
{
    Suite *s = suite_create("Stream");

    TCase *tc_streams = tcase_create("Streams");
    tcase_add_checked_fixture(tc_streams, tests_shared_setup, tests_shared_teardown);
    tcase_add_test(tc_streams, test_creation);
    tcase_add_test(tc_streams, test_writing_characters);
    tcase_add_test(tc_streams, test_writing_string);
    tcase_add_test(tc_streams, test_printing_interior);
    tcase_add_test(tc_streams, test_printing_structure);
    suite_add_tcase(s, tc_streams);

    return s;
}


int main(int argc, char **argv)
{
    Suite *s = stream_suite();

    return tests_main(argc, argv, s);
}

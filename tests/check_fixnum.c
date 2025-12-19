/*
    File:       check_fixnum.c

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#include <check.h>

#include "genericlisp.h"

#include "tests_support.h"


#if __L6P4__
char *_min_fixnum_str    = "-576460752303423488";
lisp_fixnum_t _min_fixnum_value =  -576460752303423488

char *_max_fixnum_str    = "+576460752303423487";
lisp_fixnum_t _max_fixnum_value =  +576460752303423487;
#else
char *_min_fixnum_str    = "-134217728";
lisp_fixnum_t _min_fixnum_value =  -134217728;

char *_max_fixnum_str    = "+134217727";
lisp_fixnum_t _max_fixnum_value =  +134217727;
#endif


/* MARK: - Fixnums */

START_TEST(test_creation_min_fixnum)
{
    lisp_object_t min_fixnum = lisp_fixnum_create(_min_fixnum_value);
    ck_assert_ptr_ne(lisp_NIL, min_fixnum);

    ck_assert_int_eq(lisp_tag_fixnum, lisp_object_get_tag(min_fixnum));

    lisp_fixnum_t extracted_min_fixnum = lisp_fixnum_get_value(min_fixnum);
    ck_assert_int_eq(_min_fixnum_value, extracted_min_fixnum);
}
END_TEST

START_TEST(test_creation_max_fixnum)
{
    lisp_object_t max_fixnum = lisp_fixnum_create(_max_fixnum_value);
    ck_assert_ptr_ne(lisp_NIL, max_fixnum);

    ck_assert_int_eq(lisp_tag_fixnum, lisp_object_get_tag(max_fixnum));

    lisp_fixnum_t extracted_max_fixnum = lisp_fixnum_get_value(max_fixnum);
    ck_assert_int_eq(_max_fixnum_value, extracted_max_fixnum);
}
END_TEST

START_TEST(test_printing)
{
    lisp_object_t object = lisp_fixnum_create(1234567890);

    lisp_fixnum_t fixnum_value = lisp_fixnum_get_value(object);
    lisp_fixnum_print(tests_write_stream, fixnum_value);

    ck_assert_str_eq("1234567890", tests_write_buffer);
}
END_TEST

START_TEST(test_printing_min_fixnum)
{
    lisp_object_t min_fixnum = lisp_fixnum_create(_min_fixnum_value);

    lisp_print(tests_root_environment, tests_write_stream, min_fixnum);

    ck_assert_str_eq(_min_fixnum_str, tests_write_buffer);
}
END_TEST

START_TEST(test_printing_max_fixnum)
{
    lisp_object_t max_fixnum = lisp_fixnum_create(_max_fixnum_value);

    lisp_print(tests_root_environment, tests_write_stream, max_fixnum);

    // Printing elides the + prefix that the reader supports (but does not require).
    const char * const max_fixnum_str = &_max_fixnum_str[1];

    ck_assert_str_eq(max_fixnum_str, tests_write_buffer);
}
END_TEST

START_TEST(test_equality)
{
#if __LP64__
    lisp_object_t a = lisp_fixnum_create(0x07665544332211AA);
    lisp_object_t b = lisp_fixnum_create(0x0AA1122334455667);
#else
    lisp_object_t a = lisp_fixnum_create(0x07665544);
    lisp_object_t b = lisp_fixnum_create(0x04455667);
#endif

    ck_assert(lisp_fixnum_equal(a, a) != lisp_NIL);
    ck_assert(lisp_fixnum_equal(b, b) != lisp_NIL);
    ck_assert(lisp_fixnum_equal(a, b) == lisp_NIL);
    ck_assert(lisp_fixnum_equal(b, a) == lisp_NIL);
}
END_TEST

START_TEST(test_reading_min_fixnum)
{
    lisp_object_t environment = tests_root_environment;

    tests_set_read_buffer(_min_fixnum_str);
    lisp_object_t read_object = lisp_read(environment, tests_read_stream, lisp_NIL);
    ck_assert_ptr_ne(read_object, lisp_NIL);
    ck_assert_int_eq(lisp_tag_fixnum, lisp_object_get_tag(read_object));
    ck_assert_int_eq(_min_fixnum_value, lisp_fixnum_get_value(read_object));
}
END_TEST

START_TEST(test_reading_max_fixnum)
{
    lisp_object_t environment = tests_root_environment;

    tests_set_read_buffer(_max_fixnum_str);
    lisp_object_t read_object = lisp_read(environment, tests_read_stream, lisp_NIL);
    ck_assert_ptr_ne(read_object, lisp_NIL);
    ck_assert_int_eq(lisp_tag_fixnum, lisp_object_get_tag(read_object));
    ck_assert_int_eq(_max_fixnum_value, lisp_fixnum_get_value(read_object));
}
END_TEST


/* MARK: - Test Infrastructure */

Suite *fixnum_suite(void)
{
    Suite *s = suite_create("Fixnum");

    TCase *tc_fixnums = tcase_create("Fixnums");
    tcase_add_checked_fixture(tc_fixnums, tests_shared_setup, tests_shared_teardown);
    tcase_add_test(tc_fixnums, test_creation_min_fixnum);
    tcase_add_test(tc_fixnums, test_creation_max_fixnum);
    tcase_add_test(tc_fixnums, test_printing);
    tcase_add_test(tc_fixnums, test_printing_min_fixnum);
    tcase_add_test(tc_fixnums, test_printing_max_fixnum);
    tcase_add_test(tc_fixnums, test_equality);
    tcase_add_test(tc_fixnums, test_reading_min_fixnum);
    tcase_add_test(tc_fixnums, test_reading_max_fixnum);
    suite_add_tcase(s, tc_fixnums);

    return s;
}


int main(int argc, char **argv)
{
    Suite *s = fixnum_suite();

    return tests_main(argc, argv, s);
}

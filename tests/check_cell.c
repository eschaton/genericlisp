/*
    File:       check_cell.c

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#include <check.h>

#include <stdlib.h>

#include "genericlisp.h"
#include "lisp_built_in_sforms.h"

#include "tests_support.h"


/* MARK: - Cells */

START_TEST(test_cell_creation)
{
    lisp_object_t foo = lisp_string_create_c("foo");
    lisp_object_t bar = lisp_string_create_c("bar");
    lisp_object_t object = lisp_cell_cons(foo, bar);
    ck_assert_ptr_nonnull(object);

    lisp_tag_t tag = lisp_object_get_tag(object);
    ck_assert_int_eq(lisp_tag_cell, tag);

    uintptr_t value = lisp_object_get_raw_value(object);
    ck_assert_ptr_ne(NULL, (void *)value);

    ck_assert_ptr_eq(foo, lisp_cell_car(object));
    ck_assert_ptr_eq(bar, lisp_cell_cdr(object));
}
END_TEST


START_TEST(test_cell_replacement)
{
    lisp_object_t a = lisp_string_create_c("a");
    lisp_object_t b = lisp_string_create_c("b");
    lisp_object_t object = lisp_cell_cons(a, b);
    ck_assert_ptr_nonnull(object);

    lisp_object_t x = lisp_string_create_c("x");
    lisp_object_t xobject = lisp_cell_rplaca(object, x);
    ck_assert_ptr_eq(object, xobject);
    ck_assert_ptr_eq(lisp_cell_car(xobject), x);
    ck_assert_ptr_eq(lisp_cell_cdr(xobject), b);

    lisp_object_t y = lisp_string_create_c("y");
    lisp_object_t yobject = lisp_cell_rplacd(object, y);
    ck_assert_ptr_eq(object, yobject);
    ck_assert_ptr_eq(lisp_cell_car(yobject), x);
    ck_assert_ptr_eq(lisp_cell_cdr(yobject), y);
}
END_TEST


START_TEST(test_cell_printing)
{
    lisp_object_t a = lisp_string_create_c("a");
    lisp_object_t b = lisp_string_create_c("b");
    lisp_object_t object = lisp_cell_cons(a, b);
    ck_assert_ptr_nonnull(object);

    lisp_print(tests_root_environment, tests_write_stream, object);

    ck_assert_str_eq("(\"a\" . \"b\")", tests_write_buffer);
}
END_TEST


/* MARK: - Lists */

START_TEST(test_list_creation)
{
    lisp_object_t A = lisp_char_create('A');
    lisp_object_t B = lisp_char_create('B');
    lisp_object_t C = lisp_char_create('C');

    lisp_object_t list = lisp_cell_list(A, B, C, lisp_NIL);
    ck_assert_ptr_ne((lisp_object_t) NULL, list);

    lisp_object_t list_by_cons = lisp_cell_cons(A, lisp_cell_cons(B, lisp_cell_cons(C, lisp_NIL)));
    ck_assert_ptr_ne((lisp_object_t) NULL, list_by_cons);

    lisp_object_t first = lisp_cell_car(list);
    lisp_object_t first_rest = lisp_cell_cdr(list);
    ck_assert_ptr_eq(A, first);

    lisp_object_t second = lisp_cell_car(first_rest);
    lisp_object_t second_rest = lisp_cell_cdr(first_rest);
    ck_assert_ptr_eq(B, second);

    lisp_object_t third = lisp_cell_car(second_rest);
    lisp_object_t third_rest = lisp_cell_cdr(second_rest);
    ck_assert_ptr_eq(C, third);

    lisp_object_t fourth = lisp_cell_car(third_rest);
    ck_assert_ptr_eq(lisp_NIL, fourth);
}
END_TEST

START_TEST(test_list_printing)
{
    lisp_object_t list = lisp_cell_list(lisp_atom_create_c("X"), lisp_atom_create_c("Y"), lisp_atom_create_c("Z"), lisp_NIL);

    lisp_print(tests_root_environment, tests_write_stream, list);

    ck_assert_str_eq("(X Y Z)", tests_write_buffer);
}
END_TEST

START_TEST(test_list_printing_structural)
{
    lisp_object_t list = lisp_cell_list(lisp_atom_create_c("X"), lisp_atom_create_c("Y"), lisp_atom_create_c("Z"), lisp_NIL);

    lisp_cell_t cell_value = lisp_cell_get_value(list);
    lisp_cell_print_dotted(tests_root_environment, tests_write_stream, cell_value, lisp_NIL);

    ck_assert_str_eq("(X . (Y . (Z . NIL)))", tests_write_buffer);
}
END_TEST

START_TEST(test_list_reading)
{
    lisp_object_t environment = tests_root_environment;
    lisp_object_t stream = tests_read_stream;
    tests_set_read_buffer("(A B)");

    lisp_object_t read_object = lisp_read(environment, stream, lisp_NIL);
    ck_assert_ptr_ne(lisp_NIL, read_object);
    ck_assert_int_eq(lisp_tag_cell, lisp_object_get_tag(read_object));

    ck_assert_ptr_ne(lisp_NIL, lisp_equal(lisp_atom_create_c("A"), lisp_cell_car(read_object)));
    ck_assert_ptr_ne(lisp_NIL, lisp_equal(lisp_atom_create_c("B"), lisp_cell_car(lisp_cell_cdr(read_object))));
    ck_assert_ptr_eq(lisp_NIL, lisp_cell_car(lisp_cell_cdr(lisp_cell_cdr(read_object))));
}
END_TEST

START_TEST(test_list_reading_nested)
{
    lisp_object_t environment = tests_root_environment;
    lisp_object_t stream = tests_read_stream;
    tests_set_read_buffer("(A (B C) D)");

    lisp_object_t read_object = lisp_read(environment, stream, lisp_NIL);
    ck_assert_ptr_ne(lisp_NIL, read_object);
    ck_assert_int_eq(lisp_tag_cell, lisp_object_get_tag(read_object));

    lisp_object_t A = lisp_atom_create_c("A");
    lisp_object_t B = lisp_atom_create_c("B");
    lisp_object_t C = lisp_atom_create_c("C");
    lisp_object_t D = lisp_atom_create_c("D");

    lisp_object_t matching_list = lisp_cell_list(A, lisp_cell_list(B, C, lisp_NIL), D, lisp_NIL);
    ck_assert_ptr_ne(lisp_NIL, lisp_equal(read_object, matching_list));

    lisp_object_t matching_cells = lisp_cell_cons(A, lisp_cell_cons(lisp_cell_cons(B, lisp_cell_cons(C, lisp_NIL)), lisp_cell_cons(D, lisp_NIL)));
    ck_assert_ptr_ne(lisp_NIL, lisp_equal(read_object, matching_cells));
}
END_TEST

START_TEST(test_list_reading_atom_interning)
{
    lisp_object_t environment = lisp_environment_create(tests_root_environment);
    lisp_object_t stream = tests_read_stream;
    tests_set_read_buffer("(A A))");

    lisp_object_t read_list = lisp_read(environment, stream, lisp_NIL);
    ck_assert_ptr_ne(lisp_NIL, read_list);
    ck_assert_int_eq(lisp_tag_cell, lisp_object_get_tag(read_list));

    lisp_object_t first_A = lisp_cell_car(read_list);
    lisp_object_t second_A = lisp_cell_car(lisp_cell_cdr(read_list));

    /* The A atoms should be EQ, not just EQUAL. */
    ck_assert_ptr_eq(first_A, second_A);
}
END_TEST

START_TEST(test_list_reading_quoted_atom)
{
    lisp_object_t environment = tests_root_environment;
    lisp_object_t stream = tests_read_stream;
    tests_set_read_buffer("'A");

    lisp_object_t read_object = lisp_read(environment, stream, lisp_NIL);
    ck_assert_ptr_ne(read_object, lisp_NIL);
    ck_assert_int_eq(lisp_tag_cell, lisp_object_get_tag(read_object));

    lisp_object_t A = lisp_atom_create_c("A");

    lisp_object_t quote = lisp_cell_car(read_object);
    lisp_object_t atom = lisp_cell_car(lisp_cell_cdr(read_object));
    ck_assert_ptr_eq(lisp_T, lisp_equal(quote, lisp_symbol_QUOTE));
    ck_assert_ptr_eq(lisp_T, lisp_equal(atom, A));
}
END_TEST

START_TEST(test_list_reading_quoted_list)
{
    lisp_object_t environment = tests_root_environment;

    tests_set_read_buffer("'(A B)");
    lisp_object_t read_object = lisp_read(environment, tests_read_stream, lisp_NIL);
    ck_assert_ptr_ne(read_object, lisp_NIL);
    ck_assert_int_eq(lisp_tag_cell, lisp_object_get_tag(read_object));

    lisp_object_t A = lisp_atom_create_c("A");
    lisp_object_t B = lisp_atom_create_c("B");

    lisp_object_t quote = lisp_cell_car(read_object);
    lisp_object_t cell_A = lisp_cell_car(lisp_cell_cdr(read_object));
    lisp_object_t atom_A = lisp_cell_car(cell_A);
    lisp_object_t atom_B = lisp_cell_car(lisp_cell_cdr(cell_A));

    ck_assert_ptr_eq(lisp_T, lisp_equal(quote, lisp_symbol_QUOTE));
    ck_assert_ptr_eq(lisp_T, lisp_equal(atom_A, A));
    ck_assert_ptr_eq(lisp_T, lisp_equal(atom_B, B));
}
END_TEST


/* MARK: - Test Infrastructure */

Suite *cell_suite(void)
{
    Suite *s = suite_create("Cell");

    TCase *tc_cells = tcase_create("Cells");
    tcase_add_checked_fixture(tc_cells, tests_shared_setup, tests_shared_teardown);
    tcase_add_test(tc_cells, test_cell_creation);
    tcase_add_test(tc_cells, test_cell_replacement);
    tcase_add_test(tc_cells, test_cell_printing);
    suite_add_tcase(s, tc_cells);

    TCase *tc_lists = tcase_create("Lists");
    tcase_add_checked_fixture(tc_lists, tests_shared_setup, tests_shared_teardown);
    tcase_add_test(tc_lists, test_list_creation);
    tcase_add_test(tc_lists, test_list_printing);
    tcase_add_test(tc_lists, test_list_printing_structural);
    tcase_add_test(tc_lists, test_list_reading);
    tcase_add_test(tc_lists, test_list_reading_nested);
    tcase_add_test(tc_lists, test_list_reading_atom_interning);
    tcase_add_test(tc_lists, test_list_reading_quoted_atom);
    tcase_add_test(tc_lists, test_list_reading_quoted_list);
    suite_add_tcase(s, tc_lists);

    return s;
}

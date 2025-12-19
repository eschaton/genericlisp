/*
    File:       check_environment.c

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#include <check.h>

#include "genericlisp.h"
#include "lisp_built_in_sforms.h"

#include "tests_support.h"


/* MARK: - Atoms */

START_TEST(test_evaluating_unknown_atom)
{
    lisp_object_t environment = tests_root_environment;

    // Atoms representing no known symbol should evaluate to NIL.

    lisp_object_t A = lisp_atom_create_c("A");
    lisp_object_t evaluated_A = lisp_eval(environment, A);
    ck_assert_ptr_eq(lisp_NIL, evaluated_A);
}
END_TEST

START_TEST(test_evaluating_known_atom)
{
    lisp_object_t environment = tests_root_environment;

    // Atoms representing well-known symbols should evaluate to their APVAL.

    lisp_object_t T = lisp_atom_create_c("T");
    lisp_object_t evaluated_T = lisp_eval(environment, T);
    ck_assert_ptr_eq(lisp_T, evaluated_T);
}
END_TEST


/* MARK: - Special Forms */

START_TEST(test_evaluating_QUOTE)
{
    lisp_object_t environment = tests_root_environment;

    // The `QUOTE` special form should return its argument unevaluated.

    lisp_object_t QUOTE = lisp_symbol_QUOTE;
    lisp_object_t X = lisp_atom_create_c("X");
    lisp_object_t NIL = lisp_NIL;

    lisp_object_t form = lisp_cell_list(QUOTE, X, NIL);
    lisp_object_t evaluated = lisp_eval(environment, form);
    ck_assert_ptr_eq(X, evaluated);
}
END_TEST

START_TEST(test_evaluating_SET)
{
    // Create a child environment in order to isolate effects.

    lisp_object_t environment = lisp_environment_create(tests_root_environment);

    lisp_object_t SET = lisp_symbol_SET;
    lisp_object_t QUOTE = lisp_symbol_QUOTE;
    lisp_object_t X = lisp_atom_create_c("X");
    lisp_object_t Y = lisp_atom_create_c("Y");
    lisp_object_t NIL = lisp_NIL;

    /*
     Construct a simple binding using SET:

         (SET 'X 'Y)

     While SET is special form, it uses normal evaluation rules and thus
     requires quoting. This is so it's possible to programmatically decide
     both what should be bound and what it should be bound to.
     */
    lisp_object_t form = lisp_cell_list(SET, lisp_cell_list(QUOTE, X, NIL), lisp_cell_list(QUOTE, Y, NIL), NIL);

    // Evaluate it. It should evaluate to the atom Y since that's the value SET.
    lisp_object_t evaluated_SET = lisp_eval(environment, form);
    ck_assert_ptr_eq(Y, evaluated_SET);

    // Now evaluate X, it should also evaluate to Y.
    lisp_object_t evaluated_X = lisp_eval(environment, X);
    ck_assert_ptr_eq(Y, evaluated_X);
}
END_TEST

START_TEST(test_evaluating_DEFINE)
{
    // Create a child environment in order to isolate effects.

    lisp_object_t environment = lisp_environment_create(tests_root_environment);

    lisp_object_t DEFINE = lisp_symbol_DEFINE;
    lisp_object_t LAMBDA = lisp_symbol_LAMBDA;
    lisp_object_t MULTIPLY = lisp_atom_create_c("*");
    lisp_object_t SQUARE = lisp_atom_create_c("SQUARE");
    lisp_object_t X = lisp_atom_create_c("X");
    lisp_object_t NIL = lisp_NIL;

    /*
     Construct a simple function using DEFINE:

         (DEFINE SQUARE
            (LAMBDA (X)
               (* X X)))

     Since DEFINE is a special form, we don't need to do extra quoting.
     */
    lisp_object_t SQUARE_LAMBDA_form = lisp_cell_list(LAMBDA, lisp_cell_list(X, NIL),
                                                      lisp_cell_list(MULTIPLY, X, X, NIL),
                                                      NIL);

    lisp_object_t SQUARE_form = lisp_cell_list(DEFINE, SQUARE, SQUARE_LAMBDA_form, NIL);

    // Evaluate it. It should evaluate to the atom SQUARE since that's the function DEFINE'd.
    lisp_object_t evaluated_SQUARE_form = lisp_eval(environment, SQUARE_form);
    ck_assert_ptr_eq(SQUARE, evaluated_SQUARE_form);

    // Now evaluate SQUARE itself. It should evaluate to the LAMBDA expression.
    lisp_object_t evaluated_SQUARE_value = lisp_eval(environment, SQUARE);
    ck_assert_ptr_eq(SQUARE_LAMBDA_form, evaluated_SQUARE_value);
}
END_TEST

START_TEST(test_evaluating_IF)
{
    // Create a child environment in order to isolate effects.

    lisp_object_t environment = lisp_environment_create(tests_root_environment);

    lisp_object_t IF = lisp_symbol_IF;
    lisp_object_t QUOTE = lisp_symbol_QUOTE;
    lisp_object_t UNSET = lisp_atom_create_c("UNSET");
    lisp_object_t X = lisp_atom_create_c("X");
    lisp_object_t Y = lisp_atom_create_c("Y");
    lisp_object_t T = lisp_T;
    lisp_object_t NIL = lisp_NIL;
    lisp_object_t SET = lisp_symbol_SET;

    /*
     Construct a simple expression using IF:

         (IF A
           (QUOTE X)
           (QUOTE Y))

     The use of QUOTE will enable us to both test evaluation of the
     expressions and choice between the expressions, because examining the
     environment after an evaluation should not show an interned symbol for
     the branch not taken.
     */
    lisp_object_t IF_form = lisp_cell_list(IF, UNSET,
                                           lisp_cell_list(QUOTE, X, NIL),
                                           lisp_cell_list(QUOTE, Y, NIL),
                                           NIL);

    /*
     Evaluate it. It should evaluate to the atom Y since UNSET is unbound
     and therefore NIL.
     */
    lisp_object_t evaluated_IF_form_NIL = lisp_eval(environment, IF_form);
    ck_assert_ptr_eq(Y, evaluated_IF_form_NIL);

    /*
     Evaluate (SET 'UNSET T) to change the behavior of the IF form.
     */
    lisp_object_t SET_form = lisp_cell_list(SET,
                                            lisp_cell_list(QUOTE, UNSET, NIL),
                                            T,
                                            NIL);
    lisp_object_t evaluated_SET_form = lisp_eval(environment, SET_form);
    ck_assert_ptr_eq(lisp_T, evaluated_SET_form);

    /*
     Evaluate the IF form again. It should evaluate to the atom X since A
     is now in the environment with the value T.
     */
    lisp_object_t evaluated_IF_form_T = lisp_eval(environment, IF_form);
    ck_assert_ptr_eq(X, evaluated_IF_form_T);
}
END_TEST

START_TEST(test_evaluating_LAMBDA)
{
    // Create a child environment in order to isolate effects.

    lisp_object_t environment = lisp_environment_create(tests_root_environment);

    lisp_object_t LAMBDA = lisp_symbol_LAMBDA;
    lisp_object_t IF = lisp_symbol_IF;
    lisp_object_t QUOTE = lisp_symbol_QUOTE;
    lisp_object_t V = lisp_atom_create_c("V");
    lisp_object_t X = lisp_atom_create_c("X");
    lisp_object_t Y = lisp_atom_create_c("Y");
    lisp_object_t NIL = lisp_NIL;

    /*
     Construct a simple expression using LAMBDA:

         (LAMBDA (V)
           (IF V
             (QUOTE X)
             (QUOTE Y)))

     The expression should evaluate to itself, because LAMBDA represents a
     _LAMBDA expression_, which must be _applied_ to a list of arguments in
     order to produce a result.
     */
    lisp_object_t IF_form = lisp_cell_list(IF, V,
                                           lisp_cell_list(QUOTE, X, NIL),
                                           lisp_cell_list(QUOTE, Y, NIL),
                                           NIL);
    lisp_object_t LAMBDA_form = lisp_cell_list(LAMBDA,
                                               lisp_cell_list(V, NIL),
                                               IF_form,
                                               NIL);

    /* Evaluate the LAMBDA form. */
    lisp_object_t evaluated_LAMBDA_form = lisp_eval(environment, LAMBDA_form);

    /* The evaluated LAMBDA form should be identical to the LAMBDA form. */
    ck_assert_ptr_eq(lisp_T, lisp_equal(LAMBDA_form, evaluated_LAMBDA_form));
}
END_TEST

START_TEST(test_applying_LAMBDA)
{
    // Create a child environment in order to isolate effects.

    lisp_object_t environment = lisp_environment_create(tests_root_environment);

    lisp_object_t DEFINE = lisp_symbol_DEFINE;
    lisp_object_t X_OR_Y = lisp_atom_create_c("X-OR-Y");
    lisp_object_t LAMBDA = lisp_symbol_LAMBDA;
    lisp_object_t IF = lisp_symbol_IF;
    lisp_object_t QUOTE = lisp_symbol_QUOTE;
    lisp_object_t V = lisp_atom_create_c("V");
    lisp_object_t X = lisp_atom_create_c("X");
    lisp_object_t Y = lisp_atom_create_c("Y");
    lisp_object_t NIL = lisp_NIL;
    lisp_object_t T = lisp_T;

    /*
     Construct a definition using LAMBDA:

         (DEFINE X-OR-Y
           (LAMBDA (V)
             (IF V
                 (QUOTE X)
                 (QUOTE Y))))

     Evaluating it should set X-OR-Y to an EXPR that matches the LAMBDA.
     */
    lisp_object_t IF_form = lisp_cell_list(IF, V,
                                           lisp_cell_list(QUOTE, X, NIL),
                                           lisp_cell_list(QUOTE, Y, NIL),
                                           NIL);
    lisp_object_t LAMBDA_form = lisp_cell_list(LAMBDA,
                                               lisp_cell_list(V, NIL),
                                               IF_form,
                                               NIL);
    lisp_object_t DEFINE_form = lisp_cell_list(DEFINE,
                                               X_OR_Y,
                                               LAMBDA_form,
                                               NIL);

    /* Evaluate the DEFINE form. */
    lisp_object_t evaluated_DEFINE_form = lisp_eval(environment, DEFINE_form);
    ck_assert_ptr_ne(lisp_NIL, evaluated_DEFINE_form);

    /*
     Now construct the expression (X-OR-Y NIL) and evaluate it to ensure it
     returns Y.
     */
    lisp_object_t X_OR_Y_use_NIL = lisp_cell_list(X_OR_Y, NIL, NIL);

    lisp_object_t evaluated_X_OR_Y_use_NIL = lisp_eval(environment,
                                                       X_OR_Y_use_NIL);
    ck_assert_ptr_eq(Y, evaluated_X_OR_Y_use_NIL);

    /* Finally, try (X-OR-Y T) to ensure it returns X. */
    lisp_object_t X_OR_Y_use_T = lisp_cell_list(X_OR_Y, T, NIL);

    lisp_object_t evaluated_X_OR_Y_use_T = lisp_eval(environment,
                                                     X_OR_Y_use_T);
    ck_assert_ptr_eq(X, evaluated_X_OR_Y_use_T);
}
END_TEST

START_TEST(test_evaluating_LAMBDA_in_function_position)
{
    // Create a child environment in order to isolate effects.

    lisp_object_t environment = lisp_environment_create(tests_root_environment);

    lisp_object_t LAMBDA = lisp_symbol_LAMBDA;
    lisp_object_t IF = lisp_symbol_IF;
    lisp_object_t QUOTE = lisp_symbol_QUOTE;
    lisp_object_t V = lisp_atom_create_c("V");
    lisp_object_t X = lisp_atom_create_c("X");
    lisp_object_t Y = lisp_atom_create_c("Y");
    lisp_object_t NIL = lisp_NIL;
    lisp_object_t T = lisp_T;

    /*
     Construct a LAMBDA:

         (LAMBDA (V)
           (IF V
               (QUOTE X)
               (QUOTE Y)))

     Then evaluate it in the function position with an appropriate
     argument and ensure it evaluates correctly.
     */
    lisp_object_t IF_form = lisp_cell_list(IF, V,
                                           lisp_cell_list(QUOTE, X, NIL),
                                           lisp_cell_list(QUOTE, Y, NIL),
                                           NIL);
    lisp_object_t LAMBDA_form = lisp_cell_list(LAMBDA,
                                               lisp_cell_list(V, NIL),
                                               IF_form,
                                               NIL);

    lisp_object_t LAMBDA_against_NIL = lisp_cell_cons(LAMBDA_form,
                                                      lisp_cell_cons(NIL, NIL));
    lisp_object_t evaluated_LAMBDA_against_NIL = lisp_eval(environment,
                                                           LAMBDA_against_NIL);
    ck_assert_ptr_eq(Y, evaluated_LAMBDA_against_NIL);


    lisp_object_t LAMBDA_against_T = lisp_cell_cons(LAMBDA_form,
                                                    lisp_cell_cons(T, NIL));
    lisp_object_t evaluated_LAMBDA_against_T = lisp_eval(environment,
                                                         LAMBDA_against_T);
    ck_assert_ptr_eq(X, evaluated_LAMBDA_against_T);
}
END_TEST

START_TEST(test_evaluating_BLOCK_without_return)
{
    lisp_object_t environment = lisp_environment_create(tests_root_environment);

    tests_set_read_buffer(
     "(block my-block\n"
     "  (setq a 1)\n"
     "  (setq b 2))\n");
    lisp_object_t my_block_form = lisp_read(environment, tests_read_stream, lisp_NIL);

    lisp_eval(environment, my_block_form);

    lisp_object_t A = lisp_atom_create_c("A");
    lisp_object_t B = lisp_atom_create_c("B");

    lisp_object_t A_value = lisp_environment_get_symbol_value(environment, A,
                                                              lisp_APVAL,
                                                              lisp_NIL);
    lisp_object_t B_value = lisp_environment_get_symbol_value(environment, B,
                                                              lisp_APVAL,
                                                              lisp_NIL);
    ck_assert_int_eq(lisp_tag_fixnum, lisp_object_get_tag(A_value));
    ck_assert_int_eq(lisp_tag_fixnum, lisp_object_get_tag(B_value));

    ck_assert_int_eq(1, lisp_fixnum_get_value(A_value));
    ck_assert_int_eq(2, lisp_fixnum_get_value(B_value));
}
END_TEST

START_TEST(test_evaluating_COND)
{
    lisp_object_t environment = lisp_environment_create(tests_root_environment);

    tests_set_read_buffer(
     "(define named-number\n"
     "  (lambda (n)\n"
     "    (cond ((= n 0) 'zero)\n"
     "          ((= n 1) 'one)\n"
     "          ((= n 2) 'two))))\n");
    lisp_object_t named_number = lisp_read(environment, tests_read_stream, lisp_NIL);

    lisp_eval(environment, named_number);

    tests_clear_read_buffer();
    tests_set_read_buffer("(named-number 1)\n");
    lisp_object_t named_number_1 = lisp_read(environment, tests_read_stream, lisp_NIL);

    lisp_object_t evaluated_named_number_1 = lisp_eval(environment, named_number_1);

    ck_assert_ptr_eq(lisp_T, lisp_equal(lisp_atom_create_c("one"), evaluated_named_number_1));
}
END_TEST

START_TEST(test_evaluating_DEFUN)
{
    lisp_object_t environment = lisp_environment_create(tests_root_environment);

    tests_set_read_buffer(
     "(defun x-or-y (v)\n"
     "  (if v\n"
     "      'x\n"
     "      'y\n))");
    lisp_object_t my_defun_form = lisp_read(environment, tests_read_stream, lisp_NIL);

    lisp_eval(environment, my_defun_form);
    lisp_object_t x_or_y = lisp_atom_create_c("X-OR-Y");

    lisp_print(environment, tests_write_stream, lisp_eval(environment, x_or_y));

    ck_assert_str_eq("(LAMBDA (V) (BLOCK X-OR-Y (IF V (QUOTE X) (QUOTE Y))))", tests_write_buffer);
}
END_TEST


/* MARK: - Built-in SUBRs */

START_TEST(test_evaluating_AND)
{
    lisp_object_t environment = lisp_environment_create(tests_root_environment);

    tests_set_read_buffer(
     "(if (and (= a 1) (= b 2))\n"
     "    'both-set\n"
     "    'else-clause)\n");
    lisp_object_t if_form = lisp_read(environment, tests_read_stream, lisp_NIL);
    lisp_object_t both_set = lisp_atom_create_c("BOTH-SET");
    lisp_object_t else_clause = lisp_atom_create_c("ELSE-CLAUSE");

    lisp_object_t A = lisp_atom_create_c("A");
    lisp_object_t B = lisp_atom_create_c("B");
    lisp_environment_set_symbol_value(environment, A, lisp_APVAL, lisp_fixnum_create(0), lisp_NIL);
    lisp_environment_set_symbol_value(environment, B, lisp_APVAL, lisp_fixnum_create(0), lisp_NIL);

    lisp_object_t result_0_0 = lisp_eval(environment, if_form);
    ck_assert_ptr_eq(lisp_T, lisp_equal(else_clause, result_0_0));

    lisp_environment_set_symbol_value(environment, A, lisp_APVAL, lisp_fixnum_create(1), lisp_NIL);
    lisp_environment_set_symbol_value(environment, B, lisp_APVAL, lisp_fixnum_create(1), lisp_NIL);

    lisp_object_t result_1_1 = lisp_eval(environment, if_form);
    ck_assert_ptr_eq(lisp_T, lisp_equal(else_clause, result_1_1));

    lisp_environment_set_symbol_value(environment, A, lisp_APVAL, lisp_fixnum_create(1), lisp_NIL);
    lisp_environment_set_symbol_value(environment, B, lisp_APVAL, lisp_fixnum_create(2), lisp_NIL);

    lisp_object_t result_1_2 = lisp_eval(environment, if_form);
    ck_assert_ptr_eq(lisp_T, lisp_equal(both_set, result_1_2));
}
END_TEST

START_TEST(test_evaluating_AND_with_zero_arguments)
{
    lisp_object_t environment = lisp_environment_create(tests_root_environment);

    lisp_object_t zero_arguments_and = lisp_cell_list(lisp_symbol_AND, lisp_NIL);
    lisp_object_t evaluated_z_a_and = lisp_eval(environment, zero_arguments_and);
    ck_assert_ptr_eq(lisp_T, evaluated_z_a_and);
}
END_TEST

START_TEST(test_evaluating_OR)
{
    lisp_object_t environment = lisp_environment_create(tests_root_environment);

    tests_set_read_buffer(
     "(if (or (= a 1) (= b 2))\n"
     "    'at-least-one-set\n"
     "    'neither-set)\n");
    lisp_object_t if_form = lisp_read(environment, tests_read_stream, lisp_NIL);
    lisp_object_t at_least_one_set = lisp_atom_create_c("AT-LEAST-ONE-SET");
    lisp_object_t neither_set = lisp_atom_create_c("NEITHER-SET");

    lisp_object_t A = lisp_atom_create_c("A");
    lisp_object_t B = lisp_atom_create_c("B");
    lisp_environment_set_symbol_value(environment, A, lisp_APVAL, lisp_fixnum_create(0), lisp_NIL);
    lisp_environment_set_symbol_value(environment, B, lisp_APVAL, lisp_fixnum_create(0), lisp_NIL);

    lisp_object_t result_0_0 = lisp_eval(environment, if_form);
    ck_assert_ptr_eq(lisp_T, lisp_equal(neither_set, result_0_0));

    lisp_environment_set_symbol_value(environment, A, lisp_APVAL, lisp_fixnum_create(0), lisp_NIL);
    lisp_environment_set_symbol_value(environment, B, lisp_APVAL, lisp_fixnum_create(2), lisp_NIL);

    lisp_object_t result_0_2 = lisp_eval(environment, if_form);
    ck_assert_ptr_eq(lisp_T, lisp_equal(at_least_one_set, result_0_2));

    lisp_environment_set_symbol_value(environment, A, lisp_APVAL, lisp_fixnum_create(1), lisp_NIL);
    lisp_environment_set_symbol_value(environment, B, lisp_APVAL, lisp_fixnum_create(1), lisp_NIL);

    lisp_object_t result_1_1 = lisp_eval(environment, if_form);
    ck_assert_ptr_eq(lisp_T, lisp_equal(at_least_one_set, result_1_1));
}
END_TEST

START_TEST(test_evaluating_OR_with_zero_arguments)
{
    lisp_object_t environment = lisp_environment_create(tests_root_environment);

    lisp_object_t zero_arguments_or = lisp_cell_list(lisp_symbol_OR, lisp_NIL);
    lisp_object_t evaluated_z_a_or = lisp_eval(environment, zero_arguments_or);
    ck_assert_ptr_eq(lisp_NIL, evaluated_z_a_or);
}
END_TEST

START_TEST(test_evaluating_CAR)
{
    lisp_object_t environment = lisp_environment_create(tests_root_environment);

    tests_set_read_buffer("(CAR (LIST 1 2 3 4))");
    lisp_object_t read_structure = lisp_read(environment, tests_read_stream, lisp_NIL);
    lisp_object_t evaluated = lisp_eval(environment, read_structure);

    ck_assert_ptr_eq(lisp_T, lisp_fixnump(evaluated));
    ck_assert_int_eq(1, lisp_fixnum_get_value(evaluated));
}
END_TEST

START_TEST(test_evaluating_CDR)
{
    lisp_object_t environment = lisp_environment_create(tests_root_environment);

    tests_set_read_buffer("(CDR (LIST 1 2 3 4))");
    lisp_object_t read_structure = lisp_read(environment, tests_read_stream, lisp_NIL);
    lisp_object_t evaluated = lisp_eval(environment, read_structure);

    ck_assert_ptr_eq(lisp_T, lisp_cellp(evaluated));
    lisp_object_t expected = lisp_cell_list(lisp_fixnum_create(2),
                                            lisp_fixnum_create(3),
                                            lisp_fixnum_create(4),
                                            lisp_NIL);
    ck_assert_ptr_eq(lisp_T, lisp_equal(expected, evaluated));
}
END_TEST

START_TEST(test_evaluating_PLUS_with_two_arguments)
{
    lisp_object_t environment = lisp_environment_create(tests_root_environment);

    tests_set_read_buffer("(+ 1 2)");
    lisp_object_t read_structure = lisp_read(environment, tests_read_stream, lisp_NIL);
    lisp_object_t evaluated = lisp_eval(environment, read_structure);

    ck_assert_ptr_eq(lisp_T, lisp_fixnump(evaluated));
    ck_assert_ptr_eq(lisp_T, lisp_equal(lisp_fixnum_create(3), evaluated));
}
END_TEST

START_TEST(test_evaluating_PLUS_with_n_arguments)
{
    lisp_object_t environment = lisp_environment_create(tests_root_environment);

    tests_set_read_buffer("(+ 1 2 -3 4)");
    lisp_object_t read_structure = lisp_read(environment, tests_read_stream, lisp_NIL);
    lisp_object_t evaluated = lisp_eval(environment, read_structure);

    ck_assert_ptr_eq(lisp_T, lisp_fixnump(evaluated));
    ck_assert_ptr_eq(lisp_T, lisp_equal(lisp_fixnum_create(4), evaluated));
}
END_TEST

START_TEST(test_evaluating_MINUS_with_one_positive_argument)
{
    lisp_object_t environment = lisp_environment_create(tests_root_environment);

    tests_set_read_buffer("(- 1)");
    lisp_object_t read_structure = lisp_read(environment, tests_read_stream, lisp_NIL);
    lisp_object_t evaluated = lisp_eval(environment, read_structure);

    ck_assert_ptr_eq(lisp_T, lisp_fixnump(evaluated));
    ck_assert_ptr_eq(lisp_T, lisp_equal(lisp_fixnum_create(-1), evaluated));
}
END_TEST

START_TEST(test_evaluating_MINUS_with_one_negative_argument)
{
    lisp_object_t environment = lisp_environment_create(tests_root_environment);

    tests_set_read_buffer("(- -2)");
    lisp_object_t read_structure = lisp_read(environment, tests_read_stream, lisp_NIL);
    lisp_object_t evaluated = lisp_eval(environment, read_structure);

    ck_assert_ptr_eq(lisp_T, lisp_fixnump(evaluated));
    ck_assert_ptr_eq(lisp_T, lisp_equal(lisp_fixnum_create(2), evaluated));
}
END_TEST

START_TEST(test_evaluating_MINUS_with_two_arguments)
{
    lisp_object_t environment = lisp_environment_create(tests_root_environment);

    tests_set_read_buffer("(- 3 2)");
    lisp_object_t read_structure = lisp_read(environment, tests_read_stream, lisp_NIL);
    lisp_object_t evaluated = lisp_eval(environment, read_structure);

    ck_assert_ptr_eq(lisp_T, lisp_fixnump(evaluated));
    ck_assert_ptr_eq(lisp_T, lisp_equal(lisp_fixnum_create(1), evaluated));
}
END_TEST

START_TEST(test_evaluating_MINUS_with_n_arguments)
{
    lisp_object_t environment = lisp_environment_create(tests_root_environment);

    tests_set_read_buffer("(- 1 -2 -3 4)");
    lisp_object_t read_structure = lisp_read(environment, tests_read_stream, lisp_NIL);
    lisp_object_t evaluated = lisp_eval(environment, read_structure);

    ck_assert_ptr_eq(lisp_T, lisp_fixnump(evaluated));
    ck_assert_ptr_eq(lisp_T, lisp_equal(lisp_fixnum_create(2), evaluated));
}
END_TEST


/* MARK: - Test Infrastructure */

Suite *evaluation_suite(void)
{
    Suite *s = suite_create("Evaluation");

    TCase *tc_atoms = tcase_create("Atoms");
    tcase_add_checked_fixture(tc_atoms, tests_shared_setup, tests_shared_teardown);
    tcase_add_test(tc_atoms, test_evaluating_unknown_atom);
    tcase_add_test(tc_atoms, test_evaluating_known_atom);
    suite_add_tcase(s, tc_atoms);

    TCase *tc_special_forms = tcase_create("Special Forms");
    tcase_add_checked_fixture(tc_special_forms, tests_shared_setup, tests_shared_teardown);
    tcase_add_test(tc_special_forms, test_evaluating_QUOTE);
    tcase_add_test(tc_special_forms, test_evaluating_SET);
    tcase_add_test(tc_special_forms, test_evaluating_DEFINE);
    tcase_add_test(tc_special_forms, test_evaluating_IF);
    tcase_add_test(tc_special_forms, test_evaluating_LAMBDA);
    tcase_add_test(tc_special_forms, test_applying_LAMBDA);
    tcase_add_test(tc_special_forms, test_evaluating_LAMBDA_in_function_position);
    tcase_add_test(tc_special_forms, test_evaluating_BLOCK_without_return);
    tcase_add_test(tc_special_forms, test_evaluating_COND);
    tcase_add_test(tc_special_forms, test_evaluating_DEFUN);
    suite_add_tcase(s, tc_special_forms);

    TCase *tc_built_in_SUBRs = tcase_create("Built-in SUBRs");
    tcase_add_checked_fixture(tc_built_in_SUBRs, tests_shared_setup, tests_shared_teardown);
    tcase_add_test(tc_built_in_SUBRs, test_evaluating_AND);
    tcase_add_test(tc_built_in_SUBRs, test_evaluating_AND_with_zero_arguments);
    tcase_add_test(tc_built_in_SUBRs, test_evaluating_OR);
    tcase_add_test(tc_built_in_SUBRs, test_evaluating_OR_with_zero_arguments);
    tcase_add_test(tc_built_in_SUBRs, test_evaluating_CAR);
    tcase_add_test(tc_built_in_SUBRs, test_evaluating_CDR);
    tcase_add_test(tc_built_in_SUBRs, test_evaluating_PLUS_with_two_arguments);
    tcase_add_test(tc_built_in_SUBRs, test_evaluating_PLUS_with_n_arguments);
    tcase_add_test(tc_built_in_SUBRs, test_evaluating_MINUS_with_one_positive_argument);
    tcase_add_test(tc_built_in_SUBRs, test_evaluating_MINUS_with_one_negative_argument);
    tcase_add_test(tc_built_in_SUBRs, test_evaluating_MINUS_with_two_arguments);
    tcase_add_test(tc_built_in_SUBRs, test_evaluating_MINUS_with_n_arguments);
    suite_add_tcase(s, tc_built_in_SUBRs);

    return s;
}


int main(int argc, char **argv)
{
    Suite *s = evaluation_suite();

    return tests_main(argc, argv, s);
}

//
//  TestGenericLispEvaluation.m
//  GenericLispTests
//
//  Copyright © 2025 Christopher M. Hanson. All rights reserved.
//  See file COPYING for details.
//

#import "GenericLispTestCase.h"

#import "lisp_built_in_sforms.h"


NS_ASSUME_NONNULL_BEGIN


@interface TestGenericLispEvaluation : GenericLispTestCase
@end


@implementation TestGenericLispEvaluation

- (void)testEvaluatingUnknownAtom
{
    lisp_object_t environment = self.rootEnvironment;

    // Atoms representing no known symbol should evaluate to NIL.

    lisp_object_t A = lisp_atom_create_c("A");
    lisp_object_t evaluated_A = lisp_eval(environment, A);
    XCTAssertEqual(lisp_NIL, evaluated_A);
}

- (void)testEvaluatingKnownAtom
{
    lisp_object_t environment = self.rootEnvironment;

    // Atoms representing well-known symbols should evaluate to their APVAL.

    lisp_object_t T = lisp_atom_create_c("T");
    lisp_object_t evaluated_T = lisp_eval(environment, T);
    XCTAssertEqual(lisp_T, evaluated_T);
}

- (void)testEvaluatingQUOTE
{
    lisp_object_t environment = self.rootEnvironment;

    // The `QUOTE` special form should return its argument unevaluated.

    lisp_object_t QUOTE = lisp_symbol_QUOTE;
    lisp_object_t X = lisp_atom_create_c("X");
    lisp_object_t NIL = lisp_NIL;

    lisp_object_t form = lisp_cell_list(QUOTE, X, NIL);
    lisp_object_t evaluated = lisp_eval(environment, form);
    XCTAssertEqual(X, evaluated);
}

- (void)testEvaluatingSET
{
    // Create a child environment in order to isolate effects.

    lisp_object_t environment = lisp_environment_create(self.rootEnvironment);

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
    XCTAssertEqual(Y, evaluated_SET);

    // Now evaluate X, it should also evaluate to Y.
    lisp_object_t evaluated_X = lisp_eval(environment, X);
    XCTAssertEqual(Y, evaluated_X);
}

- (void)testEvaluatingDEFINE
{
    // Create a child environment in order to isolate effects.

    lisp_object_t environment = lisp_environment_create(self.rootEnvironment);

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
    XCTAssertEqual(SQUARE, evaluated_SQUARE_form);

    // Now evaluate SQUARE itself. It should evaluate to the LAMBDA expression.
    lisp_object_t evaluated_SQUARE_value = lisp_eval(environment, SQUARE);
    XCTAssertEqual(SQUARE_LAMBDA_form, evaluated_SQUARE_value);
}

- (void)testEvaluatingIF
{
    // Create a child environment in order to isolate effects.

    lisp_object_t environment = lisp_environment_create(self.rootEnvironment);

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
    XCTAssertEqual(Y, evaluated_IF_form_NIL);

    /*
     Evaluate (SET 'UNSET T) to change the behavior of the IF form.
     */
    lisp_object_t SET_form = lisp_cell_list(SET,
                                            lisp_cell_list(QUOTE, UNSET, NIL),
                                            T,
                                            NIL);
    lisp_object_t evaluated_SET_form = lisp_eval(environment, SET_form);
    XCTAssertEqual(lisp_T, evaluated_SET_form);

    /*
     Evaluate the IF form again. It should evaluate to the atom X since A
     is now in the environment with the value T.
     */
    lisp_object_t evaluated_IF_form_T = lisp_eval(environment, IF_form);
    XCTAssertEqual(X, evaluated_IF_form_T);
}

- (void)testEvaluatingLAMBDA
{
    // Create a child environment in order to isolate effects.

    lisp_object_t environment = lisp_environment_create(self.rootEnvironment);

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
    XCTAssertEqual(lisp_T, lisp_equal(LAMBDA_form, evaluated_LAMBDA_form));
}

- (void)testApplyingLAMBDA
{
    // Create a child environment in order to isolate effects.

    lisp_object_t environment = lisp_environment_create(self.rootEnvironment);

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
    XCTAssertNotEqual(lisp_NIL, evaluated_DEFINE_form);

    /*
     Now construct the expression (X-OR-Y NIL) and evaluate it to ensure it
     returns Y.
     */
    lisp_object_t X_OR_Y_use_NIL = lisp_cell_list(X_OR_Y, NIL, NIL);

    lisp_object_t evaluated_X_OR_Y_use_NIL = lisp_eval(environment,
                                                       X_OR_Y_use_NIL);
    XCTAssertEqual(Y, evaluated_X_OR_Y_use_NIL);

    /* Finally, try (X-OR-Y T) to ensure it returns X. */
    lisp_object_t X_OR_Y_use_T = lisp_cell_list(X_OR_Y, T, NIL);

    lisp_object_t evaluated_X_OR_Y_use_T = lisp_eval(environment,
                                                     X_OR_Y_use_T);
    XCTAssertEqual(X, evaluated_X_OR_Y_use_T);
}

- (void)testEvaluatingLAMBDAInFunctionPosition
{
    // Create a child environment in order to isolate effects.

    lisp_object_t environment = lisp_environment_create(self.rootEnvironment);

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
    XCTAssertEqual(Y, evaluated_LAMBDA_against_NIL);


    lisp_object_t LAMBDA_against_T = lisp_cell_cons(LAMBDA_form,
                                                    lisp_cell_cons(T, NIL));
    lisp_object_t evaluated_LAMBDA_against_T = lisp_eval(environment,
                                                         LAMBDA_against_T);
    XCTAssertEqual(X, evaluated_LAMBDA_against_T);
}

- (void)testEvaluatingBLOCKWithoutReturn
{
    lisp_object_t environment = lisp_environment_create(self.rootEnvironment);

    [self.readBuffer setString:
     @"(block my-block\n"
     @"  (setq a 1)\n"
     @"  (setq b 2))\n"];
    lisp_object_t my_block_form = lisp_read(environment, self.readStream, lisp_NIL);

    lisp_eval(environment, my_block_form);

    lisp_object_t A = lisp_atom_create_c("A");
    lisp_object_t B = lisp_atom_create_c("B");

    lisp_object_t A_value = lisp_environment_get_symbol_value(environment, A,
                                                              lisp_APVAL,
                                                              lisp_NIL);
    lisp_object_t B_value = lisp_environment_get_symbol_value(environment, B,
                                                              lisp_APVAL,
                                                              lisp_NIL);
    XCTAssertEqual(lisp_tag_fixnum, lisp_object_get_tag(A_value));
    XCTAssertEqual(lisp_tag_fixnum, lisp_object_get_tag(B_value));

    XCTAssertEqual(1, lisp_fixnum_get_value(A_value));
    XCTAssertEqual(2, lisp_fixnum_get_value(B_value));
}

- (void)testEvaluatingCOND
{
    lisp_object_t environment = lisp_environment_create(self.rootEnvironment);

    [self.readBuffer setString:
     @"(define named-number\n"
     @"  (lambda (n)\n"
     @"    (cond ((= n 0) 'zero)\n"
     @"          ((= n 1) 'one)\n"
     @"          ((= n 2) 'two))))\n"];
    lisp_object_t named_number = lisp_read(environment, self.readStream, lisp_NIL);

    lisp_eval(environment, named_number);

    [self clearReadBuffer];
    [self.readBuffer setString:@"(named-number 1)\n"];
    lisp_object_t named_number_1 = lisp_read(environment, self.readStream, lisp_NIL);

    lisp_object_t evaluated_named_number_1 = lisp_eval(environment, named_number_1);

    XCTAssertEqual(lisp_T, lisp_equal(lisp_atom_create_c("one"), evaluated_named_number_1));
}

- (void)testEvaluatingAND
{
    lisp_object_t environment = lisp_environment_create(self.rootEnvironment);

    [self.readBuffer setString:
     @"(if (and (= a 1) (= b 2))\n"
     @"    'both-set\n"
     @"    'else-clause)\n"];
    lisp_object_t if_form = lisp_read(environment, self.readStream, lisp_NIL);
    lisp_object_t both_set = lisp_atom_create_c("BOTH-SET");
    lisp_object_t else_clause = lisp_atom_create_c("ELSE-CLAUSE");

    lisp_object_t A = lisp_atom_create_c("A");
    lisp_object_t B = lisp_atom_create_c("B");
    lisp_environment_set_symbol_value(environment, A, lisp_APVAL, lisp_fixnum_create(0), lisp_NIL);
    lisp_environment_set_symbol_value(environment, B, lisp_APVAL, lisp_fixnum_create(0), lisp_NIL);

    lisp_object_t result_0_0 = lisp_eval(environment, if_form);
    XCTAssertEqual(lisp_T, lisp_equal(else_clause, result_0_0));

    lisp_environment_set_symbol_value(environment, A, lisp_APVAL, lisp_fixnum_create(1), lisp_NIL);
    lisp_environment_set_symbol_value(environment, B, lisp_APVAL, lisp_fixnum_create(1), lisp_NIL);

    lisp_object_t result_1_1 = lisp_eval(environment, if_form);
    XCTAssertEqual(lisp_T, lisp_equal(else_clause, result_1_1));

    lisp_environment_set_symbol_value(environment, A, lisp_APVAL, lisp_fixnum_create(1), lisp_NIL);
    lisp_environment_set_symbol_value(environment, B, lisp_APVAL, lisp_fixnum_create(2), lisp_NIL);

    lisp_object_t result_1_2 = lisp_eval(environment, if_form);
    XCTAssertEqual(lisp_T, lisp_equal(both_set, result_1_2));
}

- (void)testEvaluatingANDWithZeroArguments
{
    lisp_object_t environment = lisp_environment_create(self.rootEnvironment);

    lisp_object_t zero_arguments_and = lisp_cell_list(lisp_symbol_AND, lisp_NIL);
    lisp_object_t evaluated_z_a_and = lisp_eval(environment, zero_arguments_and);
    XCTAssertEqual(lisp_T, evaluated_z_a_and);
}

- (void)testEvaluatingOR
{
    lisp_object_t environment = lisp_environment_create(self.rootEnvironment);

    [self.readBuffer setString:
     @"(if (or (= a 1) (= b 2))\n"
     @"    'at-least-one-set\n"
     @"    'neither-set)\n"];
    lisp_object_t if_form = lisp_read(environment, self.readStream, lisp_NIL);
    lisp_object_t at_least_one_set = lisp_atom_create_c("AT-LEAST-ONE-SET");
    lisp_object_t neither_set = lisp_atom_create_c("NEITHER-SET");

    lisp_object_t A = lisp_atom_create_c("A");
    lisp_object_t B = lisp_atom_create_c("B");
    lisp_environment_set_symbol_value(environment, A, lisp_APVAL, lisp_fixnum_create(0), lisp_NIL);
    lisp_environment_set_symbol_value(environment, B, lisp_APVAL, lisp_fixnum_create(0), lisp_NIL);

    lisp_object_t result_0_0 = lisp_eval(environment, if_form);
    XCTAssertEqual(lisp_T, lisp_equal(neither_set, result_0_0));

    lisp_environment_set_symbol_value(environment, A, lisp_APVAL, lisp_fixnum_create(0), lisp_NIL);
    lisp_environment_set_symbol_value(environment, B, lisp_APVAL, lisp_fixnum_create(2), lisp_NIL);

    lisp_object_t result_0_2 = lisp_eval(environment, if_form);
    XCTAssertEqual(lisp_T, lisp_equal(at_least_one_set, result_0_2));

    lisp_environment_set_symbol_value(environment, A, lisp_APVAL, lisp_fixnum_create(1), lisp_NIL);
    lisp_environment_set_symbol_value(environment, B, lisp_APVAL, lisp_fixnum_create(1), lisp_NIL);

    lisp_object_t result_1_1 = lisp_eval(environment, if_form);
    XCTAssertEqual(lisp_T, lisp_equal(at_least_one_set, result_1_1));
}

- (void)testEvaluatingORWithZeroArguments
{
    lisp_object_t environment = lisp_environment_create(self.rootEnvironment);

    lisp_object_t zero_arguments_or = lisp_cell_list(lisp_symbol_OR, lisp_NIL);
    lisp_object_t evaluated_z_a_or = lisp_eval(environment, zero_arguments_or);
    XCTAssertEqual(lisp_NIL, evaluated_z_a_or);
}

- (void)testEvaluatingDEFUN
{
    lisp_object_t environment = lisp_environment_create(self.rootEnvironment);

    [self.readBuffer setString:
     @"(defun x-or-y (v)\n"
     @"  (if v\n"
     @"      'x\n"
     @"      'y\n))"];
    lisp_object_t my_defun_form = lisp_read(environment, self.readStream, lisp_NIL);

    lisp_eval(environment, my_defun_form);
    lisp_object_t x_or_y = lisp_atom_create_c("X-OR-Y");

    lisp_print(environment, self.writeStream, lisp_eval(environment, x_or_y));

    XCTAssertEqualObjects(@"(LAMBDA (V) (BLOCK X-OR-Y (IF V (QUOTE X) (QUOTE Y))))", self.writeBuffer);
}

- (void)testEvaluatingCAR
{
    lisp_object_t environment = lisp_environment_create(self.rootEnvironment);

    [self.readBuffer setString:@"(CAR (LIST 1 2 3 4))"];
    lisp_object_t read_structure = lisp_read(environment, self.readStream, lisp_NIL);
    lisp_object_t evaluated = lisp_eval(environment, read_structure);

    XCTAssertEqual(lisp_T, lisp_fixnump(evaluated));
    XCTAssertEqual(1, lisp_fixnum_get_value(evaluated));
}

- (void)testEvaluatingCDR
{
    lisp_object_t environment = lisp_environment_create(self.rootEnvironment);

    [self.readBuffer setString:@"(CDR (LIST 1 2 3 4))"];
    lisp_object_t read_structure = lisp_read(environment, self.readStream, lisp_NIL);
    lisp_object_t evaluated = lisp_eval(environment, read_structure);

    XCTAssertEqual(lisp_T, lisp_cellp(evaluated));
    lisp_object_t expected = lisp_cell_list(lisp_fixnum_create(2),
                                            lisp_fixnum_create(3),
                                            lisp_fixnum_create(4),
                                            lisp_NIL);
    XCTAssertEqual(lisp_T, lisp_equal(expected, evaluated));
}

- (void)testEvaluatingPLUSWithTwoArguments
{
    lisp_object_t environment = lisp_environment_create(self.rootEnvironment);

    [self.readBuffer setString:@"(+ 1 2)"];
    lisp_object_t read_structure = lisp_read(environment, self.readStream, lisp_NIL);
    lisp_object_t evaluated = lisp_eval(environment, read_structure);

    XCTAssertEqual(lisp_T, lisp_fixnump(evaluated));
    XCTAssertEqual(lisp_T, lisp_equal(lisp_fixnum_create(3), evaluated));
}

- (void)testEvaluatingPLUSWithNArguments
{
    lisp_object_t environment = lisp_environment_create(self.rootEnvironment);

    [self.readBuffer setString:@"(+ 1 2 -3 4)"];
    lisp_object_t read_structure = lisp_read(environment, self.readStream, lisp_NIL);
    lisp_object_t evaluated = lisp_eval(environment, read_structure);

    XCTAssertEqual(lisp_T, lisp_fixnump(evaluated));
    XCTAssertEqual(lisp_T, lisp_equal(lisp_fixnum_create(4), evaluated));
}

- (void)testEvaluatingMINUSWithOnePositiveArgument
{
    lisp_object_t environment = lisp_environment_create(self.rootEnvironment);

    [self.readBuffer setString:@"(- 1)"];
    lisp_object_t read_structure = lisp_read(environment, self.readStream, lisp_NIL);
    lisp_object_t evaluated = lisp_eval(environment, read_structure);

    XCTAssertEqual(lisp_T, lisp_fixnump(evaluated));
    XCTAssertEqual(lisp_T, lisp_equal(lisp_fixnum_create(-1), evaluated));
}

- (void)testEvaluatingMINUSWithOneNegativeArgument
{
    lisp_object_t environment = lisp_environment_create(self.rootEnvironment);

    [self.readBuffer setString:@"(- -2)"];
    lisp_object_t read_structure = lisp_read(environment, self.readStream, lisp_NIL);
    lisp_object_t evaluated = lisp_eval(environment, read_structure);

    XCTAssertEqual(lisp_T, lisp_fixnump(evaluated));
    XCTAssertEqual(lisp_T, lisp_equal(lisp_fixnum_create(2), evaluated));
}

- (void)testEvaluatingMINUSWithTwoArguments
{
    lisp_object_t environment = lisp_environment_create(self.rootEnvironment);

    [self.readBuffer setString:@"(- 3 2)"];
    lisp_object_t read_structure = lisp_read(environment, self.readStream, lisp_NIL);
    lisp_object_t evaluated = lisp_eval(environment, read_structure);

    XCTAssertEqual(lisp_T, lisp_fixnump(evaluated));
    XCTAssertEqual(lisp_T, lisp_equal(lisp_fixnum_create(1), evaluated));
}

- (void)testEvaluatingMINUSWithNArguments
{
    lisp_object_t environment = lisp_environment_create(self.rootEnvironment);

    [self.readBuffer setString:@"(- 1 -2 -3 4)"];
    lisp_object_t read_structure = lisp_read(environment, self.readStream, lisp_NIL);
    lisp_object_t evaluated = lisp_eval(environment, read_structure);

    XCTAssertEqual(lisp_T, lisp_fixnump(evaluated));
    XCTAssertEqual(lisp_T, lisp_equal(lisp_fixnum_create(2), evaluated));
}

@end


NS_ASSUME_NONNULL_END

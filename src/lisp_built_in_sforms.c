/*
    File:       lisp_built_in_sforms.c

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#include "lisp_built_in_sforms.h"


#include "lisp_atom.h"
#include "lisp_cell.h"
#include "lisp_environment.h"
#include "lisp_evaluation.h"
#include "lisp_memory.h"
#include "lisp_plist.h"
#include "lisp_subr.h"


#if LISP_USE_STDLIB
#include <stdlib.h>
#endif


/* MARK: - Special Forms */
static void lisp_eval_special_forms_initialize(lisp_object_t environment);

/* The symbols representing the built-in special forms. */
lisp_object_t lisp_symbol_AND = NULL;
lisp_object_t lisp_symbol_COND = NULL;
lisp_object_t lisp_symbol_DEFINE = NULL;
lisp_object_t lisp_symbol_DEFUN = NULL;
lisp_object_t lisp_symbol_IF = NULL;
lisp_object_t lisp_symbol_LAMBDA = NULL;
lisp_object_t lisp_symbol_OR = NULL;
lisp_object_t lisp_symbol_QUOTE = NULL;
lisp_object_t lisp_symbol_SET = NULL;
lisp_object_t lisp_symbol_SETQ = NULL;
lisp_object_t lisp_symbol_BLOCK = NULL;
lisp_object_t lisp_symbol_RETURN_FROM = NULL;
lisp_object_t lisp_symbol_RETURN = NULL;
lisp_object_t lisp_symbol_TAGBODY = NULL;
lisp_object_t lisp_symbol_GO = NULL;

/**
 Add symbols for the built-in special forms to the environment, and do
 any other intiialization required in order to use special forms.
 */
void lisp_environment_add_built_in_special_forms(lisp_object_t environment)
{
    /* Set up all of the symbols representing special forms. */
    lisp_symbol_AND = lisp_environment_intern_symbol(environment, lisp_atom_create_c("AND"));
    lisp_symbol_COND = lisp_environment_intern_symbol(environment, lisp_atom_create_c("COND"));
    lisp_symbol_DEFINE = lisp_environment_intern_symbol(environment, lisp_atom_create_c("DEFINE"));
    lisp_symbol_DEFUN = lisp_environment_intern_symbol(environment, lisp_atom_create_c("DEFUN"));
    lisp_symbol_IF = lisp_environment_intern_symbol(environment, lisp_atom_create_c("IF"));
    lisp_symbol_LAMBDA = lisp_environment_intern_symbol(environment, lisp_atom_create_c("LAMBDA"));
    lisp_symbol_OR = lisp_environment_intern_symbol(environment, lisp_atom_create_c("OR"));
    lisp_symbol_QUOTE = lisp_environment_intern_symbol(environment, lisp_atom_create_c("QUOTE"));
    lisp_symbol_SET = lisp_environment_intern_symbol(environment, lisp_atom_create_c("SET"));
    lisp_symbol_SETQ = lisp_environment_intern_symbol(environment, lisp_atom_create_c("SETQ"));

    lisp_symbol_BLOCK = lisp_environment_intern_symbol(environment, lisp_atom_create_c("BLOCK"));
    lisp_symbol_RETURN_FROM = lisp_environment_intern_symbol(environment, lisp_atom_create_c("RETURN-FROM"));
    lisp_symbol_RETURN = lisp_environment_intern_symbol(environment, lisp_atom_create_c("RETURN"));

    lisp_symbol_TAGBODY = lisp_environment_intern_symbol(environment, lisp_atom_create_c("TAGBODY"));
    lisp_symbol_GO = lisp_environment_intern_symbol(environment, lisp_atom_create_c("GO"));

    /* Initialize everything else needed by special forms. */
    lisp_eval_special_forms_initialize(environment);
}


/** A mapping between symbols and special forms. */
struct lisp_special_form_mapping {
    lisp_object_t symbol;
    lisp_object_t (*function)(lisp_object_t environment, lisp_object_t cell);
} *lisp_special_form_mappings = NULL;

/** The number of mappings between symbols and special forms. */
uintptr_t lisp_special_form_mappings_count = 0;

int lisp_eval_is_special_form(lisp_object_t special_form)
{
    for (uintptr_t i = 0; i < lisp_special_form_mappings_count; i++) {
        if (lisp_eq(special_form, lisp_special_form_mappings[i].symbol) != lisp_NIL) {
            return 1;
        }
    }

    return 0;
}

lisp_object_t lisp_eval_special_form(lisp_object_t environment,
                                     lisp_object_t special_form,
                                     lisp_object_t cell)
{
    lisp_object_t (*function)(lisp_object_t environment, lisp_object_t cell) = NULL;

    for (uintptr_t i = 0; i < lisp_special_form_mappings_count; i++) {
        if (lisp_eq(special_form, lisp_special_form_mappings[i].symbol) != lisp_NIL) {
            function = lisp_special_form_mappings[i].function;
            break;
        }
    }

    if (function != NULL) {
        return (*function)(environment, cell);
    }

    return lisp_NIL;
}


/* MARK: - Primary Special Forms */

/**
 Evaluate the `AND` special form.

 The `AND` special form evaluates each of its arguments in turn until
 one evaluates to `NIL` and returns that. If none evaluate to `NIL`, it
 returns the result of the final form evaluated. If no arguments are
 passed, returns `T`.
 */
lisp_object_t lisp_eval_AND(lisp_object_t environment, lisp_object_t cell)
{
    /* The first item is the AND itself. */
    lisp_object_t arguments = lisp_cell_cdr(cell);

    /* If we were passed no arguments, return T. */
    if (arguments == lisp_NIL) return lisp_T;

    lisp_object_t result;
    lisp_object_t current = arguments;
    do {
        lisp_object_t argument = lisp_cell_car(current);
        result = lisp_eval(environment, argument);
        if (result == lisp_NIL) {
            return lisp_NIL;
        }
        current = lisp_cell_cdr(current);
    } while (current != lisp_NIL);

    return result;
}

/**
 Evaluate the `COND` special form.

 The `COND` special form takes a series of `(CONDITION FORMS)` lists for
 which it checks in sequence. The first time a `CONDITION` evaluates to a
 non-`NIL` value, its `FORMS` if any are evaluated and their final value
 returned from `COND` with no further evaluation. If there is no
 successful `CONDITION`, returns `NIL`.

 If there was a successful `CONDITION` with no `FORMS`, the value of the
 successful `CONDITION` is itself returned.
 */
lisp_object_t lisp_eval_COND(lisp_object_t environment, lisp_object_t cell)
{
    lisp_object_t result = lisp_NIL;

    /*
     The condition list starts with the cell's `CDR`. Iterate through every
     condition-and-forms construct one at a time.
     */
    lisp_object_t condition_list = lisp_cell_cdr(cell);
    do {
        /* Get the condition-and-forms construct to check. */
        lisp_object_t condition_and_forms = lisp_cell_car(condition_list);

        /* Get the condition itself from the construct. */
        lisp_object_t condition = lisp_cell_car(condition_and_forms);

        /*
         Evaluate the condition. If it's successful, iterate over the
         associated forms, returning the value of the last one.
         */
        result = lisp_eval(environment, condition);

        if (result != lisp_NIL) {
            lisp_object_t form_list = lisp_cell_cdr(condition_and_forms);
            do {
                lisp_object_t form = lisp_cell_car(form_list);
                result = lisp_eval(environment, form);
                form_list = lisp_cell_cdr(form_list);
            } while (form_list != lisp_NIL);

            return result;
        }

        /* Go to the next condition-and-forms construct in the list. */
        condition_list = lisp_cell_cdr(condition_list);
    } while (condition_list != lisp_NIL);

    return result;
}

/**
 Evaluate the `DEFINE` special form.

 The `DEFINE` special form establishes a symbol in the environment as a
 function. It does so by evaluating its second parameter (which should
 evaluate to an atom) to determine the name (and `PNAME`) of the symbol
 to establish, then evaluating its third parameter to get the `EXPR` of the
 symbol to etsablish. (Thus the third parameter must be a `LAMBDA` in
 order to produce an `EXPR` that can be _applied_ to a list arguments.)

 The result of the `DEFINE` special form is the bound atom..
 */
lisp_object_t lisp_eval_DEFINE(lisp_object_t environment, lisp_object_t cell)
{
    lisp_object_t result;

    lisp_object_t cell_rest = lisp_cell_cdr(cell);
    lisp_object_t second = lisp_cell_car(cell_rest);
    lisp_object_t second_rest = lisp_cell_cdr(cell_rest);

    if (second != lisp_NIL) {
        /* The second argument is the symbol to define. */
        lisp_object_t symbol_atom = second;

        /* The third argument is the symbol's `EXPR` value. */
        lisp_object_t symbol_expr = lisp_cell_car(second_rest);;

        /* Set it in the current environment without looking in parent(s). */
        lisp_environment_set_symbol_value(environment, symbol_atom, lisp_EXPR, symbol_expr, lisp_NIL);

        result = symbol_atom;
    } else {
        /* Return `NIL` for now to indicate an error. */
        result = lisp_NIL;
    }

    return result;
}

/**
 Evaluate the `DEFUN` special form.

 The `DEFUN` special form is a special version of `DEFINE` that provides
 significantly more implicit structure. It does not require quoting the
 atom used as the function's name, nor supplying a `LAMBDA` to be the
 function's body. Instead it takes a lambda-list representing the arguments,
 and constructs a `LAMBDA` containing a `BLOCK` to represent the function.

 In essence, `DEFUN` could be implemented as a macro that transforms the
 form

     (DEFUN NAME (ARGUMENTS)
        BODY-FORMS)

 into the form

     (DEFINE NAME
        (LAMBDA (ARGUMENTS)
           (BLOCK NAME
              BODY-FORMS))

 if a macro facility were available. Instead we do the transformation
 "manually" and evaluate that in order to provide equivalent behavior.
 */
lisp_object_t lisp_eval_DEFUN(lisp_object_t environment, lisp_object_t cell)
{
    /* The first item is the AND itself. */
    lisp_object_t arglist = lisp_cell_cdr(cell);

    /* Get the name, argument lambda-list, and body forms. */
    lisp_object_t name = lisp_cell_car(arglist);
    lisp_object_t arguments = lisp_cell_car(lisp_cell_cdr(arglist));
    lisp_object_t body_forms = lisp_cell_cdr(lisp_cell_cdr(arglist));

    /* Construct the DEFINE equivalent. */
    lisp_object_t block_form = lisp_cell_cons(lisp_symbol_BLOCK, lisp_cell_cons(name, body_forms));
    lisp_object_t lambda_form = lisp_cell_list(lisp_symbol_LAMBDA, arguments, block_form, lisp_NIL);
    lisp_object_t define_form = lisp_cell_list(lisp_symbol_DEFINE, name, lambda_form, lisp_NIL);

    /* Return the result of evaluating the DEFINE equivalent. */
    return lisp_eval(environment, define_form);
}

/**
 Evaluate the `IF` special form.

 The `IF` special form is used to perform a conditional action. The
 second item in the list is itself evaluated, and if it evaluates to a
 non-`NIL` value, then the third item is evaluated and the result
 returned. Otherwise the fourth item, if one exists, is returned; if
 none exists, then `NIL` is returned.
 */
lisp_object_t lisp_eval_IF(lisp_object_t environment, lisp_object_t cell)
{
    lisp_object_t result;

    lisp_object_t cell_rest = lisp_cell_cdr(cell);
    lisp_object_t second = lisp_cell_car(cell_rest);
    lisp_object_t second_rest = lisp_cell_cdr(cell_rest);
    lisp_object_t third = lisp_cell_car(second_rest);
    lisp_object_t third_rest = lisp_cell_cdr(second_rest);
    lisp_object_t fourth = lisp_cell_car(third_rest);

    /* Evaluate the expression (second argument). */
    lisp_object_t evaluated_second = lisp_eval(environment, second);

    if (evaluated_second != lisp_NIL) {
        /*
         If the expression was non-NIL, return evalution of the third item.
         */
        lisp_object_t evaluated_third = lisp_eval(environment, third);
        result = evaluated_third;
    } else {
        /*
         If the expression was non-NIL, return evaluation of the fourth item.
         (Just return NIL if it's NIL.)
         */
        if (fourth != lisp_NIL) {
            lisp_object_t evaluated_fourth = lisp_eval(environment, fourth);
            result = evaluated_fourth;
        } else {
            result = lisp_NIL;
        }
    }

    return result;
}

/**
 Evaluate the `LAMBDA` special form.

 The `LAMBDA` special form is used to represent a function that can be
 _applied_ to some number of _arguments_.

 The first argument to the special form is a _lambda list_ naming the
 arguments to the function. The second and subsequent arguments are the
 expressions that make up the function and are unevaluated by `LAMBDA`—
 thus the entirety of the `LAMBDA` itself is actually unevaluated. Only
 when a `LAMBDA` special form is _applied_ to one or more _arguments_ is
 there is any effect; this happens in `lisp_eval_cell` when the `CAR` of
 the form is a cell.
 */
lisp_object_t lisp_eval_LAMBDA(lisp_object_t environment, lisp_object_t cell)
{
    /*
     Nothing else we need to do here right now.

     In the future we could, say, validate the structure.

     The reason for this is that a lambda expression isn't evaluated, it's
     applied.
     */

    return cell;
}

/**
 Evaluate the `OR` special form.

 The `OR` special form evaluates each of its arguments in turn until one
 one evaluates to a non-`NIL` value returns that. If all evaluate to `NIL`, it
 returns `NIL`. If no arguments are passed, returns `NIL`.
 */
lisp_object_t lisp_eval_OR(lisp_object_t environment, lisp_object_t cell)
{
    /* The first item is the OR itself. */
    lisp_object_t arguments = lisp_cell_cdr(cell);

    /* If we were passed no arguments, return NIL. */
    if (arguments == lisp_NIL) return lisp_NIL;

    /* Evaluate each argument until one returns non-NIL. */
    lisp_object_t result;
    lisp_object_t current = arguments;
    do {
        lisp_object_t argument = lisp_cell_car(current);
        result = lisp_eval(environment, argument);
        if (result != lisp_NIL) {
            return result;
        }
        current = lisp_cell_cdr(current);
    } while (current != lisp_NIL);

    /* None returned non-NIL, return NIL. */

    return lisp_NIL;
}

/**
 Evaluate the `QUOTE` special form.

 The `QUOTE` special form just returns the rest of the list entirely
 unevaluated.
 */
lisp_object_t lisp_eval_QUOTE(lisp_object_t environment, lisp_object_t cell)
{
    return lisp_cell_car(lisp_cell_cdr(cell));
}

/**
 Evaluate the `SET` special form.

 The `SET` special form establishes a symbol in the environment as a
 variable. It does so by evaluating its second parameter (which should
 evaluate to an atom) to determine the name (and `PNAME`) of the symbol
 to establish, then evaluating its third parameter to get the `APVAL` of
 the symbol to etsablish.

 The result of the `SET` special form is the value that was used for the
 `APVAL` of the symbol, allowing `SET` to be usefully used in
 expressions.
 */
lisp_object_t lisp_eval_SET(lisp_object_t environment, lisp_object_t cell)
{
    lisp_object_t result;

    lisp_object_t cell_rest = lisp_cell_cdr(cell);
    lisp_object_t second = lisp_cell_car(cell_rest);
    lisp_object_t second_rest = lisp_cell_cdr(cell_rest);
    lisp_object_t evaluated_second = lisp_eval(environment, second);

    if (evaluated_second != lisp_NIL) {
        /* The second argument is the symbol to define. */
        lisp_object_t symbol_atom = evaluated_second;

        /* Evaluate the third argument. */
        lisp_object_t third = lisp_cell_car(second_rest);
        lisp_object_t evaluated_third = lisp_eval(environment, third);

        /* The third argument is the symbol's `APVAL` value. */
        lisp_object_t symbol_expr = evaluated_third;

        /* Set it in the current environment without looking in parent(s). */
        result = lisp_environment_set_symbol_value(environment, symbol_atom,
                                                   lisp_APVAL, symbol_expr,
                                                   lisp_NIL);
    } else {
        /* Return `NIL` for now to indicate an error. */
        result = lisp_NIL;
    }

    return result;
}

/**
 Evaluate the `SETQ` special form.

 The `SETQ` special form is essentially identical to `SET`, without the
 need to quote its first argument.
 */
lisp_object_t lisp_eval_SETQ(lisp_object_t environment, lisp_object_t cell)
{
    lisp_object_t result;

    lisp_object_t cell_rest = lisp_cell_cdr(cell);
    lisp_object_t second = lisp_cell_car(cell_rest);
    lisp_object_t second_rest = lisp_cell_cdr(cell_rest);

    /* The second argument is the symbol to define. */
    lisp_object_t symbol_atom = second;

    /* Evaluate the third argument. */
    lisp_object_t third = lisp_cell_car(second_rest);
    lisp_object_t evaluated_third = lisp_eval(environment, third);

    /* The third argument is the symbol's `APVAL` value. */
    lisp_object_t symbol_expr = evaluated_third;

    /* Set it in the current environment without looking in parent(s). */
    result = lisp_environment_set_symbol_value(environment, symbol_atom,
                                               lisp_APVAL, symbol_expr,
                                               lisp_NIL);

    return result;
}


/* MARK: BLOCK/RETURN-FROM/RETURN */

/**
 Evaluate the `BLOCK` special form.

 The `BLOCK` special form takes an atom to use as a "tag" and a series of forms. Each form is evaluated in turn until there are no forms or  a `RETURN-FROM` is executed that is passed the block's "tag," at which point execution transfers to the evaluater of the `BLOCK` expression with the result value passed as the second argument to the `RETURN-FROM`. If all forms are executed, the result is the value of the last form executed. If there are no forms, the result is `NIL`.
 */
lisp_object_t lisp_eval_BLOCK(lisp_object_t environment, lisp_object_t cell)
{
    lisp_object_t result = lisp_NIL;

    /* The first item is the BLOCK itself. */
    lisp_object_t arguments = lisp_cell_cdr(cell);

    /* The first argument is the tag. */
#warning lisp_eval_BLOCK: Implement RETURN-FROM support.
#if 0
    lisp_object_t tag = lisp_cell_car(arguments);
#endif

    /* The second and subsequent arguments are the body. */
    lisp_object_t remaining_body_forms = lisp_cell_cdr(arguments);
    do {
        lisp_object_t body_form = lisp_cell_car(remaining_body_forms);
        result = lisp_eval(environment, body_form);
        remaining_body_forms = lisp_cell_cdr(remaining_body_forms);
    } while (remaining_body_forms != lisp_NIL);

    return result;
}

/**
 Evaluate the `RETURN-FROM` special form.

 The `RETURN-FROM` special form takes an atom "tag" naming the `BLOCK`
 from which to return and an optional result to use as the result of
 that `BLOCK`, and then performs an exit from the `BLOCK` with the given
 result or `NIL` if no result value was supplied.
 */
lisp_object_t lisp_eval_RETURN_FROM(lisp_object_t environment, lisp_object_t cell)
{
#warning lisp_eval_RETURN_FROM: Implement.
    return lisp_NIL;
}

/**
 Evaluate the `RETURN` special form.

 The `RETURN` special form is identical to a `RETURN-FROM` special form
 supplied with a `NIL` "tag."
 */
lisp_object_t lisp_eval_RETURN(lisp_object_t environment, lisp_object_t cell)
{
#warning lisp_eval_RETURN_FROM: Implement.
    return lisp_NIL;
}


/* MARK: TAGBODY/GO */

static lisp_object_t lisp_SI_TAGBODY_STACK = NULL;
static lisp_object_t lisp_SI_TAGBODY_CURRENT = NULL;
static lisp_object_t lisp_SI_TAGBODY_SEQEUENCE = NULL;
static lisp_object_t lisp_SI_TAGBODY_MAPPING = NULL;
static lisp_object_t lisp_SI_TAGBODY_NEXT = NULL;
static lisp_object_t lisp_SI_TAGBODY_START = NULL;
static lisp_object_t lisp_SI_TAGBODY_END = NULL;

static void lisp_tagbody_initialize(lisp_object_t environment)
{
    /* Create and intern the atoms needed by the TAGBODY/GO mechanism. */

    lisp_SI_TAGBODY_STACK = lisp_atom_create_c("%SI:*TAGBODY-STACK*");
    lisp_SI_TAGBODY_CURRENT = lisp_atom_create_c("%SI:*TAGBODY-CURRENT*");
    lisp_SI_TAGBODY_SEQEUENCE = lisp_atom_create_c("%SI:TAGBODY-SEQUENCE");
    lisp_SI_TAGBODY_MAPPING = lisp_atom_create_c("%SI:TAGBODY-MAPPING");
    lisp_SI_TAGBODY_NEXT = lisp_atom_create_c("%SI:TAGBODY-NEXT");
    lisp_SI_TAGBODY_START = lisp_atom_create_c("%SI:TAGBODY-START");
    lisp_SI_TAGBODY_END = lisp_atom_create_c("%SI:TAGBODY-END");

    lisp_environment_intern_symbol(environment, lisp_SI_TAGBODY_STACK);
    lisp_environment_intern_symbol(environment, lisp_SI_TAGBODY_CURRENT);
    lisp_environment_intern_symbol(environment, lisp_SI_TAGBODY_SEQEUENCE);
    lisp_environment_intern_symbol(environment, lisp_SI_TAGBODY_MAPPING);
    lisp_environment_intern_symbol(environment, lisp_SI_TAGBODY_NEXT);
    lisp_environment_intern_symbol(environment, lisp_SI_TAGBODY_START);
    lisp_environment_intern_symbol(environment, lisp_SI_TAGBODY_END);

    /*
     The other atoms are only ever used as plist keys, but both
     %SI:*TAGBODY-STACK* and %SI:*TAGBODY-CURRENT* are global variables,
     so their APVAL need to be reset to NIL instead of pointing to
     themselves.
     */
    lisp_environment_set_symbol_value(environment,
                                      lisp_SI_TAGBODY_STACK,
                                      lisp_APVAL, lisp_NIL,
                                      lisp_NIL);
    lisp_environment_set_symbol_value(environment,
                                      lisp_SI_TAGBODY_CURRENT,
                                      lisp_APVAL, lisp_NIL,
                                      lisp_NIL);
}

/**
 Create the property list representation of the state machine for a
 given `TAGBODY` sequence.

 The implementation of `TAGBODY` relies on a pair of special symbols in
 the environment: `%SI:*TAGBODY-STACK*`, which is a list of all
 currently-active `TAGBODY` invocations, and `%SI:*TAGBODY-CURRENT*`
 which is a reference to the currently-executing `TAGBODY` on the stack.
 They must be tracked separately because the `GO` special form can be
 used to transfer control out of `TAGBODY` *B* and into `TAGBODY` *A*
 (as long as *B* is lexically within *A*).

 Each `TAGBODY` invocation contains the following information,
 represented as a property list under the following keys:

 - `%SI:TAGBODY-SEQUENCE`: A list containing the atoms referenced by the
 `TAGBODY` in the order in which they're referenced.

 - `%SI:TAGBODY-MAPPING`: A property list mapping the atoms in the
 `TAGBODY` to the lists of forms they represent, using the special atoms
 `%SI:TAGBODY-START` and `%SI:TAGBODY-END` to represent the very start
 and very end of the `TAGBODY`.

 - `%SI:TAGBODY-NEXT`: The atom representing the next form to execute,
 in case control is transferred during execution.

 Overall,  the property list created by a `TAGBODY` form can be thought
 of as a state machine: `%SI:TAGBODY-SEQUENCE` represents the set of
 states and the order their transitions, `%SI:TAGBODY-MAPPING`
 represents the forms to evaluate for each state, and `%SI:TAGBODY-NEXT`
 represents the current state.

 When a particular `TAGBODY` returns, its invocation is removed from
 `%SI:*TAGBODY-STACK*`, along with any currently-active invocations that
 may be contained within it.

 - Returns: A plist representing the `TAGBODY` state machine, or `NIL`
            on error.
 */
static lisp_object_t lisp_tagbody_create_plist(lisp_object_t environment,
                                               lisp_object_t cell)
{
    /* An accumulation list of the tags to execute, in order. */
    lisp_object_t tags = lisp_NIL;
    lisp_object_t tags_tail = lisp_NIL;

    /* The tag for forms we're currently accumulating. */
    lisp_object_t tag_current = lisp_SI_TAGBODY_START;

    /*
     An accumulation list for the forms to execute for the current tag.
     */
    lisp_object_t forms = lisp_NIL;
    lisp_object_t forms_tail = lisp_NIL;

    /* Create the plist of forms to execute. */
    lisp_object_t execute
    = lisp_plist_create(lisp_cell_cons(lisp_SI_TAGBODY_START,
                                       lisp_NIL));

    /*
     Iterate over the arguments to the TAGBODY, deciding what to do
     for each based on whether it's an atoms or a cell. For an atom,
     add the current atom and forms to the plist mapping atoms to forms,
     and set up new ones. For a cell, add it to the forms to execute for
     the current atom.
     */
    lisp_object_t current_cell = lisp_cell_cdr(cell);
    while (current_cell != lisp_NIL) {
        lisp_object_t atom_or_form = lisp_cell_car(current_cell);
        if (lisp_atomp(atom_or_form) != lisp_NIL) {
            /*
             It was an atom, so take the current atom and the current
             list of forms and create an entry in the forms plist for
             them.
             */
            lisp_plist_set(execute, tag_current, forms);

            /*
             Put the atom at the end of the list of atoms to execute.
             */
            if (tags == lisp_NIL) {
                tags = lisp_cell_cons(tag_current, lisp_NIL);
                tags_tail = tags;
            } else {
                lisp_object_t new_atoms_tail = lisp_cell_cons(tag_current,
                                                              lisp_NIL);
                lisp_cell_rplacd(tags_tail, new_atoms_tail);
                tags_tail = new_atoms_tail;
            }

            /*
             Now establish a new current atom and the list of forms for
             it.
             */
            tag_current = atom_or_form;
            forms = lisp_NIL;
            forms_tail = lisp_NIL;
        } else if (lisp_cellp(atom_or_form) != lisp_NIL) {
            /*
             It was a form, append it to the current list of forms to
             execute for the current atom.
             */
            if (forms == lisp_NIL) {
                forms = lisp_cell_cons(atom_or_form, lisp_NIL);
                forms_tail = forms;
            } else {
                lisp_object_t new_forms_tail = lisp_cell_cons(atom_or_form,
                                                              lisp_NIL);
                lisp_cell_rplacd(forms_tail, new_forms_tail);
                forms_tail = new_forms_tail;
            }
        } else {
#warning lisp_tagbody_create_plist: Handle errors.
            return lisp_NIL;
        }
        current_cell = lisp_cell_cdr(current_cell);
    }

    /*
     When we get here, there will still be a current tag and list of
     forms (though the latter may be an empty list), so we need to
     ensure they're added to the plist as well.
     */
    lisp_plist_set(execute, tag_current, forms);

    /* Append the current tag to the list of tags to execute. */

    if (tags == lisp_NIL) {
        tags = lisp_cell_cons(tag_current, lisp_NIL);
        tags_tail = tags;
    } else {
        lisp_object_t new_atoms_tail = lisp_cell_cons(tag_current,
                                                      lisp_NIL);
        lisp_cell_rplacd(tags_tail, new_atoms_tail);
        tags_tail = new_atoms_tail;
    }

    /*
     Now do that once more for the special tag indicating end-of-body,
     which has an empty list of forms. This represents the end state of
     the state machine we've created out of the TAGBODY.
     */

    lisp_plist_set(execute, lisp_SI_TAGBODY_END, lisp_NIL);
    lisp_object_t end_tag_cell = lisp_cell_cons(lisp_SI_TAGBODY_END,
                                                lisp_NIL);
    lisp_cell_rplacd(tags_tail, end_tag_cell);

    /*
     Create and return the structure representing the completely
     encapsulated TAGBODY state machine.
     */

    return lisp_plist_create(lisp_cell_cons(lisp_SI_TAGBODY_MAPPING,
                                            execute),
                             lisp_cell_cons(lisp_SI_TAGBODY_SEQEUENCE,
                                            tags),
                             lisp_cell_cons(lisp_SI_TAGBODY_NEXT,
                                            lisp_SI_TAGBODY_START),
                             NULL);
}

static void lisp_tagbody_push(lisp_object_t environment,
                              lisp_object_t tagbody_plist)
{
    /* Put the new TAGBODY plist at the top of the stack. */

    lisp_object_t stack = lisp_environment_get_symbol_value(environment,
                                                            lisp_SI_TAGBODY_STACK,
                                                            lisp_APVAL,
                                                            lisp_T);

    lisp_object_t new_stack = lisp_cell_cons(tagbody_plist, stack);

    lisp_environment_set_symbol_value(environment,
                                      lisp_SI_TAGBODY_STACK,
                                      lisp_APVAL, new_stack,
                                      lisp_T);
}

static void lisp_tagbody_pop(lisp_object_t environment,
                             lisp_object_t tagbody_plist)
{
    /*
     Find the cell containing the given TAGBODY plist on the stack, then
     pop both it and everything above it in the stack.
     */

    lisp_object_t stack = lisp_environment_get_symbol_value(environment,
                                                            lisp_SI_TAGBODY_STACK,
                                                            lisp_APVAL,
                                                            lisp_T);

    lisp_object_t next_cell = stack;
    do {
        lisp_object_t plist = lisp_cell_car(next_cell);
        next_cell = lisp_cell_cdr(next_cell);
        if (plist == tagbody_plist) {
            lisp_environment_set_symbol_value(environment,
                                              lisp_SI_TAGBODY_STACK,
                                              lisp_APVAL, next_cell,
                                              lisp_T);
            break;
        }
    } while (next_cell != lisp_NIL);
}

/**
 Execute the given `TAGBODY`, which must be on (but not necessarilly at
 the top of) the `TAGBODY` stack. Do so by getting the next forms to
 execute from the `TAGBODY` and then evaluating them one at a time until
 the last one returns.
 */
static void lisp_tagbody_execute(lisp_object_t environment,
                                 lisp_object_t tagbody_plist)
{
    /* Loop over all the tags to execute until there are no more. */
    lisp_object_t sequence = lisp_plist_get(tagbody_plist,
                                            lisp_SI_TAGBODY_SEQEUENCE);

    while (1) {
        /* Get the next state for the state machine. */
        lisp_object_t cur_tag = lisp_plist_get(tagbody_plist,
                                               lisp_SI_TAGBODY_NEXT);

        /*
         Run the sequence forward to that tag, just in case we saw a
         non-local jump.
         */
        lisp_object_t updated_sequence = sequence;
        while (updated_sequence != lisp_NIL) {
            lisp_object_t seq_tag = lisp_cell_car(updated_sequence);
            if (seq_tag == cur_tag) {
                /* Found the tag to use, adjust iteration sequence. */
                sequence = updated_sequence;
                break;
            } else {
                /* Not found yet, keep iterating. */
                updated_sequence = lisp_cell_cdr(updated_sequence);
            }
        }

        /*
         If we're at the end, pop the TAGBODY from the stack and exit.
         */
        if (cur_tag == lisp_SI_TAGBODY_END) {
            lisp_tagbody_pop(environment, tagbody_plist);
            return;
        }

        /* Get the forms to execute for the tag. */
        lisp_object_t mapping = lisp_plist_get(tagbody_plist,
                                               lisp_SI_TAGBODY_MAPPING);
        lisp_object_t forms = lisp_plist_get(mapping, cur_tag);

        /* Evaluate each of the forms. */
        while (forms != lisp_NIL) {
            lisp_object_t form = lisp_cell_car(forms);
            lisp_eval(environment, form);
            forms = lisp_cell_cdr(forms);
        }

        /*
         Send the state machine to the next state, updating both the
         sequence pointer and the next tag entry in the state machine.

         It's important to keep the state machine itself up to date in
         order to handle jumps and such. This is also why the next tag
         whose forms should be executed has to come from the state
         machine's next tag slot above, rather from iterating sequence:
         One of the forms may have changed it.
         */
        sequence = lisp_cell_cdr(sequence);
        lisp_object_t next_tag = lisp_cell_car(sequence);
        lisp_plist_set(tagbody_plist, lisp_SI_TAGBODY_NEXT, next_tag);
    }
}

/**
 Set the next tag to execute in the current stack of `TAGBODY` state
 machines. This can cause a jump from an inner to an outer `TAGBODY`
 (but not the opposite) which means we need to search the stack from the
 currently-executing `TAGBODY` to any further out in order to find the
 tag from which to resume execution.
 */
static lisp_object_t lisp_tagbody_set_next(lisp_object_t environment,
                                           lisp_object_t tag)
{
    return lisp_NIL;//xxx
}


/**
 Evaluate the `TAGBODY` special form, which takes a sequence of atoms
 and forms, where the atoms denote "points of resumption" for execution
 of the forms that can be switched to when the `GO` special form is
 encountered. Unlike a `BLOCK`, a `TAGBODY` always returns `NIL`.

 A `TAGBODY` evaluates all forms in sequence until either it completes
 or until a `GO` is encountered that changes where it is in executing
 its forms.

 - Returns: Always returns `NIL`.
 */
lisp_object_t lisp_eval_TAGBODY(lisp_object_t environment, lisp_object_t cell)
{
    /* Build the property list representing this TAGBODY. */
    lisp_object_t tagbody_plist = lisp_tagbody_create_plist(environment, cell);

    /* Push this TAGBODY on %SI:*TAGBODY-STACK* so it can be run. */
    lisp_tagbody_push(environment, tagbody_plist);

    /* Start executing the TAGBODY state machine. */
    lisp_tagbody_execute(environment, tagbody_plist);

    /* Popping the TAGBODY happens when execution completes. */

    return lisp_NIL;
}

/**
 Evaluate the `GO` special form, which transfers control within a
 `TAGBODY` to another point in that `TAGBODY` or in some containing
 `TAGBODY`.

 - Returns; Always returns `NIL` since it always results in a transfer
 of control within a currently-executing `TAGBODY` invocation.
 */
lisp_object_t lisp_eval_GO(lisp_object_t environment, lisp_object_t cell)
{
    /* Transfer control to the specified point via the TAGBODY stack. */

    lisp_object_t atom = lisp_cell_car(lisp_cell_cdr(cell));

    lisp_tagbody_set_next(environment, atom);

    return lisp_NIL;
}


/* MARK: - Initialization */

void lisp_eval_special_forms_initialize(lisp_object_t environment)
{
    struct lisp_special_form_mapping mappings[] = {
        { lisp_symbol_AND, lisp_eval_AND },
        { lisp_symbol_COND, lisp_eval_COND },
        { lisp_symbol_DEFINE, lisp_eval_DEFINE },
        { lisp_symbol_DEFUN, lisp_eval_DEFUN },
        { lisp_symbol_IF, lisp_eval_IF },
        { lisp_symbol_LAMBDA, lisp_eval_LAMBDA },
        { lisp_symbol_OR, lisp_eval_OR },
        { lisp_symbol_QUOTE, lisp_eval_QUOTE },
        { lisp_symbol_BLOCK, lisp_eval_BLOCK },
        { lisp_symbol_RETURN_FROM, lisp_eval_RETURN_FROM },
        { lisp_symbol_RETURN, lisp_eval_RETURN },
        { lisp_symbol_SET, lisp_eval_SET },
        { lisp_symbol_SETQ, lisp_eval_SETQ },
        { lisp_symbol_TAGBODY, lisp_eval_TAGBODY },
        { lisp_symbol_GO, lisp_eval_GO },
    };

    lisp_special_form_mappings_count = sizeof(mappings) / sizeof(struct lisp_special_form_mapping);

#if LISP_USE_STDLIB
    /* TODO: Put this in the Lisp heap as an interior? */
    lisp_special_form_mappings = calloc(lisp_special_form_mappings_count,
                                        sizeof(struct lisp_special_form_mapping));
#endif

    for (size_t i = 0; i < lisp_special_form_mappings_count; i++) {
        lisp_special_form_mappings[i].symbol = mappings[i].symbol;
        lisp_special_form_mappings[i].function = mappings[i].function;
    }

    lisp_tagbody_initialize(environment);
}

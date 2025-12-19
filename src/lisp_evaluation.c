/*
    File:       lisp_evaluation.c

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#include "lisp_evaluation.h"

#include "lisp_cell.h"
#include "lisp_environment.h"
#include "lisp_plist.h"
#include "lisp_subr.h"

#include "lisp_built_in_sforms.h"


static lisp_object_t lisp_eval_atom(lisp_object_t environment, lisp_object_t atom);
static lisp_object_t lisp_eval_cell(lisp_object_t environment, lisp_object_t cell);

static lisp_object_t lisp_eval_argument_list(lisp_object_t environment, lisp_object_t list);

static lisp_object_t lisp_apply_expr(lisp_object_t environment, lisp_object_t function, lisp_object_t arguments);
static lisp_object_t lisp_apply_subr(lisp_object_t environment, lisp_object_t function, lisp_object_t arguments);


/* MARK: - Evaluation */

lisp_object_t lisp_eval(lisp_object_t environment,
                        lisp_object_t form)
{
    lisp_object_t result;
    lisp_tag_t tag = lisp_object_get_tag(form);

    switch (tag) {
        case lisp_tag_atom: {
            /* Atoms look up a symbol in the environment. */
            result = lisp_eval_atom(environment, form);
        } break;

        case lisp_tag_cell: {
            /* Lists are complicated. */
            result = lisp_eval_cell(environment, form);
        } break;

        default: {
            /* All other types are value types that evaluate to themselves. */
            result = form;
        } break;
    }

    return result;
}

/**
 Evaluate an atom in the given environment.

 Evaluating an atom treats it as a symbol to look up in the given
 \a environment (and any parent environments), and returns one of the
 following values, checked in this order:

 1. Its `SUBR` since this represents a compiled function.

 2. Its `EXPR` since this represents an uncompiled function.

 3. Its `APVAL` since this repreesents a variable binding.

 If no entry exists for the atom in the environment, the atom evaluates
 to `NIL`.
 */
lisp_object_t lisp_eval_atom(lisp_object_t environment, lisp_object_t atom)
{
    /* Check whether the atom has an entry in the environment at all. */
    lisp_object_t symbol = lisp_environment_find_symbol(environment, atom, lisp_T);
    lisp_object_t plist = lisp_cell_cdr(symbol);

    if ((symbol == lisp_NIL) || (plist == lisp_NIL)) {
        /* No plist in the environment or its parents, just the atom itself. */
        return lisp_NIL;
    }

    /* Return the atom's SUBR if it has one. */
    lisp_object_t subr = lisp_plist_get(plist, lisp_SUBR);
    if (subr != lisp_NIL) {
        return subr;
    }

    /* Return the atom's EXPR if it has one. */
    lisp_object_t expr = lisp_plist_get(plist, lisp_EXPR);
    if (expr != lisp_NIL) {
        return expr;
    }

    /* Return the atom's APVAL if it has one. */
    lisp_object_t apval = lisp_plist_get(plist, lisp_APVAL);
    if (apval != lisp_NIL) {
        return apval;
    }

    /* The atom doesn't have a SUBR, EXPR, or APVAL so just return NIL. */

    return lisp_NIL;
}

/**
 Evaluate a cell in the given environment.

 Cell (list) evaluation is where we implement "real" evaluation and the
 basic special forms. What happens when a list is evaluated depends
 entirely on its `CAR`:

 - If it's an atom representing a special form, then handle the special
   form.
 - If it's another atom, look up its value and then apply that to the
   result of evaluating every item in the `CDR`.
 - If it's a cell, evaluate it and then apply that to the result of
   evaluating every item in the `CDR`.
 - If it's neither an atom nor a cell, return `NIL` since this isn't
   valid.

 The special forms are defined each in their own functions below.
 */
lisp_object_t lisp_eval_cell(lisp_object_t environment, lisp_object_t cell)
{
    lisp_object_t result;

    lisp_object_t car = lisp_cell_car(cell);
    if (lisp_atomp(car) != lisp_NIL) {
        if (lisp_eval_is_special_form(car)) {
            result = lisp_eval_special_form(environment, car, cell);
        } else {
            lisp_object_t function = lisp_eval_atom(environment, car);
            if (function != lisp_NIL) {
                lisp_object_t arguments = lisp_cell_cdr(cell);
                lisp_object_t evaluated_arguments = lisp_eval_argument_list(environment, arguments);
                result = lisp_apply(environment, function, evaluated_arguments);
            } else {
                result = lisp_NIL;
            }
        }
    } else if (lisp_cellp(car) != lisp_NIL) {
        lisp_object_t function = lisp_eval_cell(environment, car);
        lisp_object_t arguments = lisp_cell_cdr(cell);
        lisp_object_t evaluated_arguments = lisp_eval_argument_list(environment, arguments);
        result = lisp_apply(environment, function, evaluated_arguments);
    } else {
        result = lisp_NIL;
    }

    return result;
}

/**
 For each item in the given argument list, evaluate it and append it to
 a new list.

 - Returns: The list of arguments, evaluated.
 */
static lisp_object_t lisp_eval_argument_list(lisp_object_t environment, lisp_object_t list)
{
    lisp_object_t result = lisp_NIL;
    lisp_object_t result_tail = lisp_NIL;

    /* Return NIL if we're given NIL or a non-cell object for list. */
    if ((list == lisp_NIL) || (lisp_cellp(list) == lisp_NIL)) {
        return result;
    }

    /* Iterate over the list. */
    lisp_object_t lisp_iter = list;
    do {
        /* Evaluate the car and put the result in a cell. */
        lisp_object_t car = lisp_cell_car(lisp_iter);
        lisp_object_t car_evaluated = lisp_eval(environment, car);
        lisp_object_t result_cell = lisp_cell_cons(car_evaluated, lisp_NIL);

        /* Append the cell to the result, or create the result if none yet. */
        if (result != lisp_NIL) {
            lisp_cell_rplacd(result_tail, result_cell);
            result_tail = result_cell;
        } else {
            result = result_cell;
            result_tail = result_cell;
        }

        /* Go on to the next element in the list. */
        lisp_iter = lisp_cell_cdr(lisp_iter);
    } while (lisp_iter != lisp_NIL);

    return result;
}


/* MARK: - Application */

lisp_object_t lisp_apply(lisp_object_t environment,
                         lisp_object_t function,
                         lisp_object_t arguments)
{
    lisp_object_t result;

    if (lisp_cellp(function) != lisp_NIL) {
        result = lisp_apply_expr(environment, function, arguments);
    } else {
        result = lisp_apply_subr(environment, function, arguments);
    }

    return result;
}

/**
 Bind the given variables to the given values in the given environment.

 - Parameters:
   - environment: The environment in which to bind the variables.
                  Variables will be bound *exclusively* in this
                  environment, shadowing any previous bindings.
   - variables: A *lambda list* describing the variables to bind. At the
                moment this consists solely of atoms, there is no
                support for `&key` or `&rest` arguments.
   - values: A list of values to bind to the variables pairwise.
 - Returns: `NIL` if the binding fails for some reason (e.g. mismatched
            number of variables and arguments), `T` otherwise.
 */
lisp_object_t lisp_environment_bind_variables(lisp_object_t environment,
                                              lisp_object_t variables,
                                              lisp_object_t values)
{
    lisp_object_t variables_iter = variables;
    lisp_object_t values_iter = values;
    do {
        /* Get a variable and a value from the lists. */
        lisp_object_t variable = lisp_cell_car(variables_iter);
        lisp_object_t value = lisp_cell_car(values_iter);

        /*
         Bind the APVAL of the symbol the variable represents, without
         searching the parent environment chain.
         */
        lisp_environment_set_symbol_value(environment, variable,
                                          lisp_APVAL, value,
                                          lisp_NIL);

        /* Go to the next variable and value in the lists. */
        variables_iter = lisp_cell_cdr(variables_iter);
        values_iter = lisp_cell_cdr(values_iter);
    } while ((variables_iter != lisp_NIL) && (values_iter != lisp_NIL));

    /* If we exhausted one list but not the other, return NIL. */
    if (variables_iter != values_iter) {
        return lisp_NIL;
    }

    return lisp_T;
}

/**
 Apply an `EXPR` (which must be a `LAMBDA` expression) to a list of
 arguments in the context of an environment.
 */
lisp_object_t lisp_apply_expr(lisp_object_t environment, lisp_object_t function, lisp_object_t arguments)
{
    /* Create an environment in which the application takes place. */
    lisp_object_t application_environment = lisp_environment_create(environment);

    /* Get the variables to bind out of the LAMBDA expression. */
    lisp_object_t function_rest = lisp_cell_cdr(function);
    lisp_object_t variables = lisp_cell_car(function_rest);

    /* Bind the variables in the application environment. */
    lisp_object_t boundp = lisp_environment_bind_variables(application_environment,
                                                           variables, arguments);
    if (boundp == lisp_NIL) {
        return lisp_NIL;
    }

    /*
     Iterate over the third and beyond entries in the lambda, evaluating
     each one in the application environment, and returning the value of
     the last evaluation.
     */
    lisp_object_t result = lisp_NIL;
    for (lisp_object_t function_next = lisp_cell_cdr(function_rest);
         function_next != lisp_NIL;
         function_next = lisp_cell_cdr(function_next))
    {
        lisp_object_t form = lisp_cell_car(function_next);
        result = lisp_eval(application_environment, form);
    }

    return result;
}

/**
 Apply a `SUBR` to a list of arguments in the context of an environment.
 */
lisp_object_t lisp_apply_subr(lisp_object_t environment, lisp_object_t function, lisp_object_t arguments)
{
    /* We don't have to do anything but call the function. */

    return lisp_subr_call(function, environment, arguments);
}

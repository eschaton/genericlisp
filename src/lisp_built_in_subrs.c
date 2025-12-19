/*
    File:       lisp_built_in_subrs.c

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#include "lisp_built_in_subrs.h"

#include "lisp_atom.h"
#include "lisp_cell.h"
#include "lisp_environment.h"
#include "lisp_evaluation.h"
#include "lisp_fixnum.h"
#include "lisp_printing.h"
#include "lisp_reading.h"
#include "lisp_stream.h"
#include "lisp_string.h"
#include "lisp_subr.h"


/*
 The built-in SUBRs cover the rest of Lisp.
 */

lisp_object_t lisp_subr_CAR(lisp_object_t environment, lisp_object_t arguments)
{
    return lisp_cell_car(lisp_cell_car(arguments));
}

lisp_object_t lisp_subr_CDR(lisp_object_t environment, lisp_object_t arguments)
{
    return lisp_cell_cdr(lisp_cell_car(arguments));
}

lisp_object_t lisp_subr_CONS(lisp_object_t environment, lisp_object_t arguments)
{
    lisp_object_t first = lisp_cell_car(arguments);
    lisp_object_t second = lisp_cell_car(lisp_cell_cdr(arguments));
    return lisp_cell_cons(first, second);
}

lisp_object_t lisp_subr_ATOM(lisp_object_t environment, lisp_object_t arguments)
{
    lisp_object_t first = lisp_cell_car(arguments);
    return lisp_atomp(first);
}

lisp_object_t lisp_subr_EQ(lisp_object_t environment, lisp_object_t arguments)
{
    lisp_object_t first = lisp_cell_car(arguments);
    lisp_object_t second = lisp_cell_car(lisp_cell_cdr(arguments));
    return lisp_eq(first, second);
}

lisp_object_t lisp_subr_EQUAL(lisp_object_t environment, lisp_object_t arguments)
{
    lisp_object_t first = lisp_cell_car(arguments);
    lisp_object_t second = lisp_cell_car(lisp_cell_cdr(arguments));
    return lisp_equal(first, second);
}

lisp_object_t lisp_subr_LIST(lisp_object_t environment, lisp_object_t arguments)
{
    return arguments;
}

lisp_object_t lisp_subr_NULL(lisp_object_t environment, lisp_object_t arguments)
{
    lisp_object_t first = lisp_cell_car(arguments);
    return (first == lisp_NIL) ? lisp_T : lisp_NIL;
}

lisp_object_t lisp_subr_MEMBER(lisp_object_t environment, lisp_object_t arguments)
{
    lisp_object_t x = lisp_cell_car(arguments);
    lisp_object_t list = lisp_cell_car(lisp_cell_cdr(arguments));

    do {
        lisp_object_t list_car = lisp_cell_car(list);
        if (lisp_equal(x, list_car) != lisp_NIL) {
            return lisp_T;
        }
        list = lisp_cell_cdr(list);
    } while (list != lisp_NIL);

    return lisp_NIL;
}

lisp_object_t lisp_subr_LENGTH(lisp_object_t environment, lisp_object_t arguments)
{
    lisp_fixnum_t length = 0;
    lisp_object_t list = lisp_cell_car(arguments);

    while (list != lisp_NIL) {
        length = length + 1;
        list = lisp_cell_cdr(list);
    }

    return lisp_fixnum_create(length);
}

lisp_object_t lisp_subr_RPLACA(lisp_object_t environment, lisp_object_t arguments)
{
    lisp_object_t first = lisp_cell_car(arguments);
    lisp_object_t second = lisp_cell_car(lisp_cell_cdr(arguments));
    return lisp_cell_rplaca(first, second);
}

lisp_object_t lisp_subr_RPLACD(lisp_object_t environment, lisp_object_t arguments)
{
    lisp_object_t first = lisp_cell_car(arguments);
    lisp_object_t second = lisp_cell_car(lisp_cell_cdr(arguments));
    return lisp_cell_rplacd(first, second);
}

lisp_object_t lisp_subr_NOT(lisp_object_t environment, lisp_object_t arguments)
{
    lisp_object_t first = lisp_cell_car(arguments);
    return (first == lisp_NIL) ? lisp_T : lisp_NIL;
}

lisp_object_t lisp_subr_NUMBERP(lisp_object_t environment, lisp_object_t arguments)
{
    lisp_object_t first = lisp_cell_car(arguments);
    return lisp_fixnump(first);
}

lisp_object_t lisp_subr_ZEROP(lisp_object_t environment, lisp_object_t arguments)
{
    lisp_object_t first = lisp_cell_car(arguments);
    if (lisp_fixnump(first) == lisp_NIL) return lisp_NIL;
    lisp_fixnum_t fixnum = lisp_fixnum_get_value(first);
    return (fixnum == 0) ? lisp_T : lisp_NIL;
}

lisp_object_t lisp_subr_MINUSP(lisp_object_t environment, lisp_object_t arguments)
{
    lisp_object_t first = lisp_cell_car(arguments);
    if (lisp_fixnump(first) == lisp_NIL) return lisp_NIL;
    lisp_fixnum_t fixnum = lisp_fixnum_get_value(first);
    return (fixnum < 0) ? lisp_T : lisp_NIL;
}

lisp_object_t lisp_subr_sign_LESS_THAN(lisp_object_t environment, lisp_object_t arguments)
{
    lisp_object_t first = lisp_cell_car(arguments);
    if (lisp_fixnump(first) == lisp_NIL) return lisp_NIL;

    lisp_object_t second = lisp_cell_car(lisp_cell_cdr(arguments));
    if (lisp_fixnump(second) == lisp_NIL) return lisp_NIL;

    lisp_fixnum_t x = lisp_fixnum_get_value(first);
    lisp_fixnum_t y = lisp_fixnum_get_value(second);

    return (x < y) ? lisp_T : lisp_NIL;
}

lisp_object_t lisp_subr_sign_LESS_THAN_OR_EQUALS(lisp_object_t environment, lisp_object_t arguments)
{
    lisp_object_t first = lisp_cell_car(arguments);
    if (lisp_fixnump(first) == lisp_NIL) return lisp_NIL;

    lisp_object_t second = lisp_cell_car(lisp_cell_cdr(arguments));
    if (lisp_fixnump(second) == lisp_NIL) return lisp_NIL;

    lisp_fixnum_t x = lisp_fixnum_get_value(first);
    lisp_fixnum_t y = lisp_fixnum_get_value(second);

    return (x <= y) ? lisp_T : lisp_NIL;
}

lisp_object_t lisp_subr_sign_GREATER_THAN(lisp_object_t environment, lisp_object_t arguments)
{
    lisp_object_t first = lisp_cell_car(arguments);
    if (lisp_fixnump(first) == lisp_NIL) return lisp_NIL;

    lisp_object_t second = lisp_cell_car(lisp_cell_cdr(arguments));
    if (lisp_fixnump(second) == lisp_NIL) return lisp_NIL;

    lisp_fixnum_t x = lisp_fixnum_get_value(first);
    lisp_fixnum_t y = lisp_fixnum_get_value(second);

    return (x > y) ? lisp_T : lisp_NIL;
}

lisp_object_t lisp_subr_sign_GREATER_THAN_OR_EQUALS(lisp_object_t environment, lisp_object_t arguments)
{
    lisp_object_t first = lisp_cell_car(arguments);
    if (lisp_fixnump(first) == lisp_NIL) return lisp_NIL;

    lisp_object_t second = lisp_cell_car(lisp_cell_cdr(arguments));
    if (lisp_fixnump(second) == lisp_NIL) return lisp_NIL;

    lisp_fixnum_t x = lisp_fixnum_get_value(first);
    lisp_fixnum_t y = lisp_fixnum_get_value(second);

    return (x >= y) ? lisp_T : lisp_NIL;
}

lisp_object_t lisp_subr_sign_EQUALS(lisp_object_t environment, lisp_object_t arguments)
{
    lisp_object_t first = lisp_cell_car(arguments);
    if (lisp_fixnump(first) == lisp_NIL) return lisp_NIL;

    lisp_object_t second = lisp_cell_car(lisp_cell_cdr(arguments));
    if (lisp_fixnump(second) == lisp_NIL) return lisp_NIL;

    lisp_fixnum_t x = lisp_fixnum_get_value(first);
    lisp_fixnum_t y = lisp_fixnum_get_value(second);

    return (x == y) ? lisp_T : lisp_NIL;
}

lisp_object_t lisp_subr_sign_PLUS(lisp_object_t environment, lisp_object_t arguments)
{
    lisp_fixnum_t sum = 0;
    lisp_object_t args = arguments;
    while (args != lisp_NIL) {
        lisp_object_t arg = lisp_cell_car(args);
        if (lisp_fixnump(arg) == lisp_NIL) return lisp_NIL;
        sum = sum + lisp_fixnum_get_value(arg);
        args = lisp_cell_cdr(args);
    }
    return lisp_fixnum_create(sum);
}

lisp_object_t lisp_subr_sign_MINUS(lisp_object_t environment, lisp_object_t arguments)
{
    /*
     Get the first value separately, since MINUS implements both
     negation and subtraction.
     */
    lisp_object_t first = lisp_cell_car(arguments);
    if (lisp_fixnump(first) == lisp_NIL) return lisp_NIL;
    lisp_fixnum_t accumulator = lisp_fixnum_get_value(first);

    lisp_object_t subsequent = lisp_cell_cdr(arguments);
    if (subsequent == lisp_NIL) {
        /* If there was only one argument, this is negation. */
        return lisp_fixnum_create(- accumulator);
    } else {
        /* If there was more than one argument, this is subtraction. */
        while (subsequent != lisp_NIL) {
            lisp_object_t arg = lisp_cell_car(subsequent);
            if (lisp_fixnump(arg) == lisp_NIL) return lisp_NIL;
            accumulator = accumulator - lisp_fixnum_get_value(arg);
            subsequent = lisp_cell_cdr(subsequent);
        }
        return lisp_fixnum_create(accumulator);
    }
}

lisp_object_t lisp_subr_sign_TIMES(lisp_object_t environment, lisp_object_t arguments)
{
    lisp_fixnum_t product = 0;
    lisp_object_t args = arguments;
    while (args != lisp_NIL) {
        lisp_object_t arg = lisp_cell_car(args);
        if (lisp_fixnump(arg) == lisp_NIL) return lisp_NIL;
        product = product * lisp_fixnum_get_value(arg);
        args = lisp_cell_cdr(args);
    }
    return lisp_fixnum_create(product);
}

lisp_object_t lisp_subr_sign_DIVIDE(lisp_object_t environment, lisp_object_t arguments)
{
    lisp_object_t first = lisp_cell_car(arguments);
    if (lisp_fixnump(first) == lisp_NIL) return lisp_NIL;

    lisp_object_t second = lisp_cell_car(lisp_cell_cdr(arguments));
    if (lisp_fixnump(second) == lisp_NIL) return lisp_NIL;

    lisp_fixnum_t x = lisp_fixnum_get_value(first);
    lisp_fixnum_t y = lisp_fixnum_get_value(second);

    return lisp_fixnum_create(x / y);
}

lisp_object_t lisp_subr_sign_MODULO(lisp_object_t environment, lisp_object_t arguments)
{
    lisp_object_t first = lisp_cell_car(arguments);
    if (lisp_fixnump(first) == lisp_NIL) return lisp_NIL;

    lisp_object_t second = lisp_cell_car(lisp_cell_cdr(arguments));
    if (lisp_fixnump(second) == lisp_NIL) return lisp_NIL;

    lisp_fixnum_t x = lisp_fixnum_get_value(first);
    lisp_fixnum_t y = lisp_fixnum_get_value(second);

    return lisp_fixnum_create(x % y);
}

lisp_object_t lisp_subr_STRINGP(lisp_object_t environment, lisp_object_t arguments)
{
    lisp_object_t first = lisp_cell_car(arguments);
    return lisp_stringp(first);
}

lisp_object_t lisp_subr_STREAMP(lisp_object_t environment, lisp_object_t arguments)
{
    lisp_object_t first = lisp_cell_car(arguments);
    return lisp_streamp(first);
}

lisp_object_t lisp_subr_READ(lisp_object_t environment, lisp_object_t arguments)
{
    lisp_object_t stream = lisp_cell_car(arguments);
    if ((stream != lisp_T) && (lisp_streamp(stream) == lisp_NIL)) return lisp_NIL;
    return lisp_read(environment, stream, lisp_NIL);
}

lisp_object_t lisp_subr_PRIN1(lisp_object_t environment, lisp_object_t arguments)
{
    lisp_object_t object = lisp_cell_car(arguments);
    lisp_object_t stream = lisp_cell_car(lisp_cell_cdr(arguments));
    lisp_print(environment, stream, object);
    return object;
}

lisp_object_t lisp_subr_PRINC(lisp_object_t environment, lisp_object_t arguments)
{
    lisp_object_t object = lisp_cell_car(arguments);
    lisp_object_t stream = lisp_cell_car(lisp_cell_cdr(arguments));
    lisp_print(environment, stream, object);
    return object;
}

lisp_object_t lisp_subr_PRINT(lisp_object_t environment, lisp_object_t arguments)
{
    lisp_object_t object = lisp_cell_car(arguments);
    lisp_object_t stream = lisp_cell_car(lisp_cell_cdr(arguments));
    lisp_print(environment, stream, lisp_char_create(char_newline));
    lisp_print(environment, stream, object);
    lisp_print(environment, stream, lisp_char_create(char_space));
    return object;
}

lisp_object_t lisp_subr_TERPRI(lisp_object_t environment, lisp_object_t arguments)
{
    lisp_object_t stream = lisp_cell_car(arguments);
    lisp_print(environment, stream, lisp_char_create(char_newline));
    return lisp_NIL;
}

lisp_object_t lisp_subr_EVAL(lisp_object_t environment, lisp_object_t arguments)
{
    lisp_object_t form = lisp_cell_car(arguments);
    if (form == lisp_NIL) return lisp_NIL;
    return lisp_eval(environment, form);
}

lisp_object_t lisp_subr_APPLY(lisp_object_t environment, lisp_object_t arguments)
{
    lisp_object_t function = lisp_cell_car(arguments);
    if ((lisp_subrp(function) == lisp_NIL) && (lisp_cellp(function) == lisp_NIL)) return lisp_NIL;
    lisp_object_t function_arguments = lisp_cell_car(lisp_cell_cdr(arguments));
    return lisp_apply(environment, function, function_arguments);
}


void lisp_environment_add_built_in_SUBRs(lisp_object_t environment)
{
    struct proto_subr {
        lisp_callable callable;
        char *name;
    } lisp_built_in_SUBRs[] = {
        { lisp_subr_CAR, "CAR" },
        { lisp_subr_CDR, "CDR" },
        { lisp_subr_CONS, "CONS" },
        { lisp_subr_ATOM, "ATOM" },
        { lisp_subr_EQ, "EQ" },
        { lisp_subr_EQUAL, "EQUAL" },
        { lisp_subr_LIST, "LIST" },
        { lisp_subr_NULL, "NULL" },
        { lisp_subr_MEMBER, "MEMBER" },
        { lisp_subr_LENGTH, "LENGTH" },
        { lisp_subr_RPLACA, "RPLACA" },
        { lisp_subr_RPLACD, "RPLACD" },
        { lisp_subr_NOT, "NOT" },
        { lisp_subr_NUMBERP, "NUMBERP" },
        { lisp_subr_ZEROP, "ZEROP" },
        { lisp_subr_MINUSP, "MINUSP" },
        { lisp_subr_sign_LESS_THAN, "<" },
        { lisp_subr_sign_LESS_THAN_OR_EQUALS, "<=" },
        { lisp_subr_sign_GREATER_THAN, ">" },
        { lisp_subr_sign_GREATER_THAN_OR_EQUALS, ">=" },
        { lisp_subr_sign_EQUALS, "=" },
        { lisp_subr_sign_PLUS, "+" },
        { lisp_subr_sign_MINUS, "-" },
        { lisp_subr_sign_TIMES, "*" },
        { lisp_subr_sign_DIVIDE, "/" },
        { lisp_subr_sign_MODULO, "%" },
        { lisp_subr_STRINGP, "STRINGP" },
        { lisp_subr_STREAMP, "STREAMP" },
        { lisp_subr_READ, "READ" },
        { lisp_subr_PRIN1, "PRIN1" },
        { lisp_subr_PRIN1, "PRINC" },
        { lisp_subr_PRINT, "PRINT" },
        { lisp_subr_TERPRI, "TERPRI" },
        { lisp_subr_EVAL, "EVAL" },
        { lisp_subr_APPLY, "APPLY" },
        { 0, 0 },
    };

    for (struct proto_subr *item = lisp_built_in_SUBRs;
         item->callable != 0;
         item++)
    {
        lisp_object_t symbol = lisp_atom_create_c(item->name);
        lisp_object_t symbol_name = lisp_string_create_c(item->name);
        lisp_callable symbol_callable = item->callable;
        lisp_object_t symbol_subr = lisp_subr_create(symbol_callable, symbol_name);
        lisp_environment_set_symbol_value(environment, symbol, lisp_SUBR, symbol_subr, lisp_NIL);
        lisp_environment_set_symbol_value(environment, symbol, lisp_PNAME, symbol_name, lisp_NIL);
    }
}

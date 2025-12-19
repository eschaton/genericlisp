/*
    File:       lisp_reading.c

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#include "lisp_reading.h"

#include "lisp_atom.h"
#include "lisp_cell.h"
#include "lisp_environment.h"
#include "lisp_evaluation.h"
#include "lisp_fixnum.h"
#include "lisp_stream.h"
#include "lisp_string.h"

#include "lisp_built_in_sforms.h"

#if LISP_USE_STDLIB
#include <assert.h>
#include <stdlib.h>
#endif


/*
 The Lisp reader can read the following types of objects:

 - Atoms, introduced by a non-numeric, non-syntactic printing character;
   this includes keywords introduced by a colon.
 - Fixnums, optionally introduced by a plus or minus;
 - Lists, delimited by parentheses;
 - Strings, delimited by double-quotes with backslash escaping of
   a small number of special characters (`b`, `e`, `n`, `t`);
 - Vectors, introduced by an octothorpe and delimited by parentheses;
 - Characters, introduced by an octothorpe followed by a backslash.

 Whitespace and end-of-line comments are also handled, but not returned
 in any way to the caller.

 Thus the grammar can be described using the following tokens

     object = atom | fixnum | quote | list | string | vector | character.
     atom = atom-starting-character+ atom-character*.
     fixnum = ('+' | '-')? fixnum-character+.
     quote = '\'' atom | list.
     list = '(' object* ')'.
     string = '"' non-string-character* '"'.
     vector = '#'_'(' object* ')'.
     character = '#'_'\' character.

 where whitespace and end-of-line comments can be present between any
 tokens except those written a_b in the grammar.
 */

static lisp_object_t lisp_read_object(lisp_object_t environment, lisp_object_t stream, lisp_object_t recursivep);
static lisp_object_t lisp_read_atom(lisp_object_t environment, lisp_object_t stream, lisp_object_t recursivep);
static lisp_object_t lisp_read_fixnum(lisp_object_t environment, lisp_object_t stream, lisp_object_t recursivep);
static lisp_object_t lisp_read_list(lisp_object_t environment, lisp_object_t stream, lisp_object_t recursivep);
static lisp_object_t lisp_read_string(lisp_object_t environment, lisp_object_t stream, lisp_object_t recursivep);
static lisp_object_t lisp_read_vector(lisp_object_t environment, lisp_object_t stream, lisp_object_t recursivep);
static lisp_object_t lisp_read_character(lisp_object_t stream);
static lisp_object_t lisp_read_quote(lisp_object_t environment, lisp_object_t stream, lisp_object_t recursivep);
static void lisp_skip_whitespace_and_comments(lisp_object_t stream);
static void lisp_skip_comment(lisp_object_t stream);


/* MARK: - Reader */


lisp_object_t lisp_read(lisp_object_t environment,
                        lisp_object_t stream,
                        lisp_object_t recursivep)
{
    lisp_object_t input_stream = lisp_stream_best_input_stream(environment, stream);

    return lisp_read_object(environment, input_stream, recursivep);
}


/* MARK: - Parser */

lisp_object_t lisp_read_object(lisp_object_t environment, lisp_object_t stream, lisp_object_t recursivep)
{
    lisp_object_t read_object;

    /* Skip to the first non-whitespace non-comment character. */
    lisp_skip_whitespace_and_comments(stream);

    /* Read another character. */
    lisp_object_t ch = lisp_stream_read_char(stream);
    if (ch == lisp_NIL) {
        return lisp_NIL;
    }
    lisp_char_t ch_value = lisp_char_get_value(ch);

    /* Decide what to do based on it. */
    switch (ch_value) {
        case char_0:
        case char_1:
        case char_2:
        case char_3:
        case char_4:
        case char_5:
        case char_6:
        case char_7:
        case char_8:
        case char_9:{
            /* It's a number! Restore the stream and read the number. */
            lisp_stream_unread_char(stream, ch);
            read_object = lisp_read_fixnum(environment, stream, recursivep);
        } break;

        case char_plus:
        case char_minus: {
            /*
             It *might* be a number! Check the next item in the stream and if
             it's not a number, the + or - is part of an atom.
             */
            lisp_object_t next_ch = lisp_stream_read_char(stream);
            lisp_char_t next_ch_value = lisp_char_get_value(next_ch);
            lisp_stream_unread_char(stream, next_ch);
            lisp_stream_unread_char(stream, ch);
            if ((next_ch_value >= '0') && (next_ch_value <= '9')) {
                read_object = lisp_read_fixnum(environment, stream, recursivep);
            } else {
                read_object = lisp_read_atom(environment, stream, recursivep);
            }
        } break;

        case char_single_quote: {
            /* It's a QUOTE! Restore the stream and read the expression. */
            lisp_stream_unread_char(stream, ch);
            read_object = lisp_read_quote(environment, stream, recursivep);
        } break;

        case char_paren_open: {
            /* It's a list! Restore the stream and read the list. */
            lisp_stream_unread_char(stream, ch);
            read_object = lisp_read_list(environment, stream, recursivep);
        } break;

        case char_paren_close: {
            /*
             It's an EOL marker! If an EOL marker was passed as recursivep,
             return that, otherwise indicate an error by returning NIL.
             */
            return recursivep;
        } break;

        case char_double_quote: {
            /* It's a string! Restore the stream and read the string. */
            lisp_stream_unread_char(stream, ch);
            read_object = lisp_read_string(environment, stream, recursivep);
        } break;

        case char_octothorpe: {
            /* It's either a vector or a character, read another to decide. */
            lisp_object_t ch2 = lisp_stream_read_char(stream);
            if (ch2 == lisp_NIL) {
                return lisp_NIL;
            }
            lisp_char_t ch2_value = lisp_char_get_value(ch2);
            switch (ch2_value) {
                case char_paren_open:
                    /* Restore the stream and read the vector. */
                    lisp_stream_unread_char(stream, ch2);
                    lisp_stream_unread_char(stream, ch);
                    read_object = lisp_read_vector(environment, stream, recursivep);
                    break;

                case char_backslash:
                    /* Restore the stream and read the character. */
                    lisp_stream_unread_char(stream, ch2);
                    lisp_stream_unread_char(stream, ch);
                    read_object = lisp_read_character(stream);
                    break;

                default:
                    /* Anything else is invalid. */
                    assert(0);
                    break;
            }
        } break;

        default: {
            /* It's an atom! Restore the stream and read the atom. */
            lisp_stream_unread_char(stream, ch);
            read_object = lisp_read_atom(environment, stream, recursivep);
        } break;
    }

    return read_object;
}

lisp_object_t lisp_read_atom(lisp_object_t environment, lisp_object_t stream, lisp_object_t recursivep)
{
    lisp_object_t read_object, atom_name;

    lisp_object_t ch;
    lisp_char_t ch_value;

    /* Read all of the characters until whitespace or a common introducer into a string. */
    atom_name = lisp_string_create_empty();
    ch = lisp_stream_read_char(stream);
    if (ch == lisp_NIL) {
        return lisp_NIL;
    }
    ch_value = lisp_char_get_value(ch);
    lisp_string_append_char(atom_name, ch);

    int done = 0;
    do {
        /* Read the next character. */
        ch = lisp_stream_read_char(stream);
        if (ch == lisp_NIL) {
            done = 1;
            break;
        }
        ch_value = lisp_char_get_value(ch);

        /* Decide what to do based on it. */
        switch (ch_value) {
            case char_space:
            case char_newline:
            case char_tab:
            case char_semicolon:
            case char_paren_open:
            case char_paren_close:
            case char_octothorpe:
                /*
                 Whitespace, comment, parentheses, octothorpe mean we're done.
                 Restore the stream and finish.
                 */
                lisp_stream_unread_char(stream, ch);
                done = 1;
                break;

            default:
                /* Anything else, append to the read token and continue reading. */
                lisp_string_append_char(atom_name, ch);
                break;
        }
    } while (!done);

    if (atom_name != lisp_NIL) {
        /*
         Once we have a full atom name, create an atom and return it. Since we
         have access to the complete environment, if the read atom is equal to
         an existing atom, return the existing atom. If it doesn't, intern it
         so that reading `(A A)` returns the same atom for both the CAR and
         CADR.
         */
        lisp_object_t read_atom = lisp_atom_create(atom_name);
        lisp_object_t atom_symbol = lisp_environment_find_symbol(environment, read_atom, lisp_T);
        if (atom_symbol != lisp_NIL) {
            read_object = lisp_cell_car(atom_symbol);
        } else {
            read_object = lisp_environment_intern_symbol(environment, read_atom);
        }
    } else {
        read_object = lisp_NIL;
    }

    return read_object;
}

lisp_object_t lisp_read_fixnum(lisp_object_t environment, lisp_object_t stream, lisp_object_t recursivep)
{
    lisp_object_t read_object;

    /*
     Read the sign and digits of the number into a buffer sized to fit the
     maximum size of decimal number we can represent, plus an optional sign
     character. That depends on the bit width of the system:

     - 32-bit: 2^27=134217727, or 9 characters plus a sign.
     - 64-bit: 2^59=576460752303423488, or 18 characters plus a sign.

     Since the maximum size is 18 characters plus a sign, a 20-character
     buffer will be sufficient.
     */
    char buffer[20];
    int buffer_pos = 0;
#if __LP64__
    int buffer_max = 18 + 1;
#else
    int buffer_max = 9 + 1;
#endif
    int done = 0;
    do {
        lisp_object_t ch = lisp_stream_read_char(stream);
        if (ch == lisp_NIL) {
            /* On end-of-stream, parse and return the number. */
            done = 1;
            break;
        }
        lisp_char_t ch_value = lisp_char_get_value(ch);

        switch (ch_value) {
            case char_plus:
            case char_minus: {
                /* Sign, only valid as the first character. */
                if (buffer_pos != 0) {
                    return lisp_NIL;
                } else {
                    buffer[buffer_pos] = (char) ch_value;
                    buffer_pos = buffer_pos + 1;
                }
            } break;

            case char_0:
            case char_1:
            case char_2:
            case char_3:
            case char_4:
            case char_5:
            case char_6:
            case char_7:
            case char_8:
            case char_9: {
                /* Numeric character, just add it to the buffer if possible. */
                if (buffer_pos == buffer_max) {
                    return lisp_NIL;
                } else {
                    buffer[buffer_pos] = (char) ch_value;
                    buffer_pos = buffer_pos + 1;
                }
            } break;

            default: {
                /* Non-numeric character, unread it and finish. */
                lisp_stream_unread_char(stream, ch);
                done = 1;
            } break;
        }
    } while (!done);

    /* The buffer is larger than the max so it's always safe to terminate. */
    buffer[buffer_pos] = '\0';

    /* Just use stdlib for now. */
    long value = atol(buffer);
    lisp_fixnum_t fixnum = value;

    read_object = lisp_fixnum_create(fixnum);

    return read_object;
}

lisp_object_t lisp_read_list(lisp_object_t environment, lisp_object_t stream, lisp_object_t recursivep)
{
    lisp_object_t read_object = lisp_NIL;

    lisp_object_t ch;

    /* Create a list in which to place read objects. */
    lisp_object_t list_head = lisp_NIL;
    lisp_object_t list_cur = lisp_NIL;

    /* Read the list introducer. */
    ch = lisp_stream_read_char(stream);
    if ((ch == lisp_NIL) || (ch != lisp_char_create(char_paren_open))) {
        return lisp_NIL;
    }

    /*
     Read all of the contained objects until the final close parenthesis.
     This is indicated by passing a brand-new, uninterned atom as the
     `recursivep` argument to lisp_read_object, which is then returned as
     the read object.
     */
    lisp_object_t eol_signifier = lisp_atom_create_c("%SI:END-OF-LIST");
    int done = 0;
    do {
        /* Read one object from the stream. */
        lisp_object_t one_object = lisp_read_object(environment, stream, eol_signifier);
        if (one_object != eol_signifier) {
            /*
             If the read object is not our EOL marker, create a cell with the
             read object as the CAR and a NIL CDR, and append that to the list.
             */
            lisp_object_t read_cell = lisp_cell_cons(one_object, lisp_NIL);

            if (list_cur == lisp_NIL) {
                /*
                 If we haven't read any objects, use this as the first cell in
                 the list.
                 */
                list_head = read_cell;
            } else {
                /* Just append to the current cell. */
                lisp_cell_rplacd(list_cur, read_cell);
            }

            /* The read object is now our current cell. */
            list_cur = read_cell;
        } else {
            /*
             If the read object is our EOL marker, we're done.
             */
            read_object = list_head;
            done = 1;
        }
    } while (!done);

    return read_object;
}

lisp_object_t lisp_read_string(lisp_object_t environment, lisp_object_t stream, lisp_object_t recursivep)
{
    lisp_object_t read_object;

    lisp_object_t ch;
    lisp_char_t ch_value;

    read_object = lisp_string_create_empty();

    /* Read the string introducer. */
    ch = lisp_stream_read_char(stream);
    if ((ch == lisp_NIL) || (ch != lisp_char_create(char_double_quote))) {
        return lisp_NIL;
    }

    /* Read all of the characters until the final double quote into a string. */
    int done = 0;
    do {
        /* Read the next character. */
        ch = lisp_stream_read_char(stream);
        if (ch == lisp_NIL) {
            done = 1;
            break;
        }
        ch_value = lisp_char_get_value(ch);

        /* Decide what to do based on it. */
        switch (ch_value) {
            case char_double_quote: {
                /* The string is over, just finish. */
                done = 1;
            } break;

            case char_backslash: {
                /* The the next character is escaped, read and append it. */
                lisp_object_t ch2 = lisp_stream_read_char(stream);
                if (ch2 == lisp_NIL) {
                    done = 1;
                    break;
                }
                lisp_string_append_char(read_object, ch2);
            } break;

            default: {
                /* Append the character to the string and continue. */
                lisp_string_append_char(read_object, ch);
                break;
            }
        }
    } while (!done);

    return read_object;
}

lisp_object_t lisp_read_vector(lisp_object_t environment, lisp_object_t stream, lisp_object_t recursivep)
{
#warning lisp_read_vector: Implement.
    return lisp_NIL;
}

/**
 Read a character token from the input stream.
 */
lisp_object_t lisp_read_character(lisp_object_t stream)
{
    lisp_object_t read_object;

    lisp_object_t ch;
    lisp_char_t ch_value;

    /* Read the character introducer. */
    ch = lisp_stream_read_char(stream);
    if (ch == lisp_NIL) {
        return lisp_NIL;
    }
    ch_value = lisp_char_get_value(ch);
    assert(ch_value == char_octothorpe);
    ch = lisp_stream_read_char(stream);
    if (ch == lisp_NIL) {
        return lisp_NIL;
    }
    ch_value = lisp_char_get_value(ch);
    assert(ch_value == char_backslash);

    /*
     It doesn't matter what the next character is, it's a character object
     or the stream is at end and therefore `NIL`.
     */
    read_object = lisp_stream_read_char(stream);

    return read_object;
}

/**
 Read a quoted object from the input stream, returning the object within
 a `QUOTE` special form. This happens at the reading level to make the
 syntax extra-convenient, otherwise it would add extra parentheses in
 many places.
 */
lisp_object_t lisp_read_quote(lisp_object_t environment, lisp_object_t stream, lisp_object_t recursivep)
{
    lisp_object_t read_object;

    lisp_object_t ch;

    /* Read the QUOTE introducer. */
    ch = lisp_stream_read_char(stream);
    if (ch != lisp_char_create(char_single_quote)) {
        return lisp_NIL;
    }

    /* Read the the object to quote. */
    lisp_object_t object = lisp_read(environment, stream, recursivep);

    /*
     Return a quoted version of the object, which is the CAR of the CDR in
     a QUOTE special form. This is used to prevent evaluation.
     */

    read_object = lisp_cell_list(lisp_symbol_QUOTE, object, lisp_NIL);

    return read_object;
}

/**
 Read the input stream forward until the next non-whitespace character or the end of an end-of-line comment.
 */
void lisp_skip_whitespace_and_comments(lisp_object_t stream)
{
    int done = 0;
    do {
        /* Read the next character. */
        lisp_object_t ch = lisp_stream_read_char(stream);
        if (ch == lisp_NIL) {
            return;
        }
        lisp_char_t ch_value = lisp_char_get_value(ch);

        /* Decide what to do based on it. */
        switch (ch_value) {
            case char_space:
            case char_newline:
            case char_tab:
                /* This is what we're looking for, just keep going. */
                break;

            case char_semicolon:
                /* Comment introducer, skip the comment and continue. */
                lisp_stream_unread_char(stream, ch);
                lisp_skip_comment(stream);
                break;

            default:
                /* Read something else, put it back and return to caller. */
                lisp_stream_unread_char(stream, ch);
                done = 1;
                break;
        }
    } while (!done);
}

/**
 Read the input stream forward from the comment introducer to the next
 */
void lisp_skip_comment(lisp_object_t stream)
{
    lisp_object_t ch;
    lisp_char_t ch_value;

    /* Read the comment introducer. */
    ch = lisp_stream_read_char(stream);
    if (ch == lisp_NIL) {
        return;
    }

    int done = 0;
    do {
        /* Read the next character. */
        ch = lisp_stream_read_char(stream);
        if (ch == lisp_NIL) {
            return;
        }
        ch_value = lisp_char_get_value(ch);

        /* Decide what to do based on it. */
        switch (ch_value) {
            case char_newline:
                /* A newline ends the comment, just return. */
                done = 1;
                break;

            default:
                /* Anything but a newline is part of the comment. Keep reading. */
                break;
        }
    } while (!done);
}

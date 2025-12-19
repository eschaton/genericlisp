/*
    File:       lisp_string.h

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#ifndef __lisp_string__
#define __lisp_string__ 1


#include "lisp_types.h"


/**
 A Lisp "character," which is actually a 28-bit Unicode codepoint in
 UCS-4 encoding, with the high bit extended.

 - Warning: This implies a `uintptr_t` must be at least 32 bits wide.
            If it's 64 bits wide, this is pretty wasteful.
 */
typedef uintptr_t lisp_char_t;

/** Create a Lisp object representing the given `char_value`. */
LISP_EXTERN lisp_object_t lisp_char_create(lisp_char_t char_value);

/** Get the character value of the given Lisp object. */
LISP_EXTERN lisp_char_t lisp_char_get_value(lisp_object_t object);

/** Prints the character to the given output stream with quoting. */
LISP_EXTERN lisp_object_t lisp_char_print(lisp_object_t stream, lisp_char_t char_value);

/** Prints the string to the given output stream with optional quoting. */
LISP_EXTERN lisp_object_t lisp_char_print_quoted(lisp_object_t stream, lisp_char_t char_value, lisp_object_t should_quote);

/** Compare two characters for equality. */
LISP_EXTERN lisp_object_t lisp_char_equal(lisp_object_t a, lisp_object_t b);


/* Well-known character values used in syntax. */

#define char_tab            ((lisp_char_t) 0x09)
#define char_newline        ((lisp_char_t) 0x0a)
#define char_return         ((lisp_char_t) 0x0d)
#define char_space          ((lisp_char_t) 0x20)
#define char_double_quote   ((lisp_char_t) 0x22)
#define char_octothorpe     ((lisp_char_t) 0x23)
#define char_single_quote   ((lisp_char_t) 0x27)
#define char_paren_open     ((lisp_char_t) 0x28)
#define char_paren_close    ((lisp_char_t) 0x29)
#define char_plus           ((lisp_char_t) 0x2b)
#define char_minus          ((lisp_char_t) 0x2d)
#define char_period         ((lisp_char_t) 0x2e)
#define char_0              ((lisp_char_t) 0x30)
#define char_1              ((lisp_char_t) 0x31)
#define char_2              ((lisp_char_t) 0x32)
#define char_3              ((lisp_char_t) 0x33)
#define char_4              ((lisp_char_t) 0x34)
#define char_5              ((lisp_char_t) 0x35)
#define char_6              ((lisp_char_t) 0x36)
#define char_7              ((lisp_char_t) 0x37)
#define char_8              ((lisp_char_t) 0x38)
#define char_9              ((lisp_char_t) 0x39)
#define char_semicolon      ((lisp_char_t) 0x3b)
#define char_less_than      ((lisp_char_t) 0x3c)
#define char_greater_than   ((lisp_char_t) 0x3e)
#define char_backslash      ((lisp_char_t) 0x5c)


/**
 A Lisp string.

 A string is a sequence of zero or more 28-bit code points in UCS-4
 encoding, represented as Lisp character objects.

 - Note: Since Lisp strings are homogeneous and characters are atomic,
         their contents don't need to participate in garbage collection.
 */
typedef struct lisp_string {
    /** The characters in the string., as an interior pointer */
    lisp_object_t chars;

    /** How many characters the string can hold before reallocation  */
    uintptr_t capacity;

    /** The number of characters in the string. */
    uintptr_t length;
} *lisp_string_t;

/** Create a string given a sequence of characters in an interior. */
LISP_EXTERN lisp_object_t lisp_string_create(lisp_object_t chars,
                                             uintptr_t capacity,
                                             uintptr_t length);

/** Create a string given a C string. */
LISP_EXTERN lisp_object_t lisp_string_create_c(const char *cstring);

/** Create an empty string. */
LISP_EXTERN lisp_object_t lisp_string_create_empty(void);

/** Get the string value of the given Lisp object. */
LISP_EXTERN lisp_string_t lisp_string_get_value(lisp_object_t object);

/** Prints the string to the given output stream, with quoting. */
LISP_EXTERN lisp_object_t lisp_string_print(lisp_object_t stream, lisp_string_t string_value);

/** Prints the string to the given output stream with optional quotes. */
LISP_EXTERN lisp_object_t lisp_string_print_quoted(lisp_object_t stream, lisp_string_t string_value, lisp_object_t should_quote);

/**
 Compare two strings for equality.

 Considers first the length, then the content of the strings. Does not
 considert the capacity of the string since that can change with the
 string itself.
 */
LISP_EXTERN lisp_object_t lisp_string_equal(lisp_object_t a, lisp_object_t b);

/**
 Modify a string by appending a character to it.

 - Returns: The modified string.

 - Warning: The string being appended to is modified in place, a new
            string is not created. The return of the string is just for
            convenience.
 */
LISP_EXTERN lisp_object_t lisp_string_append_char(lisp_object_t string,
                                                  lisp_object_t ch);

#endif  /* __lisp_string__ */

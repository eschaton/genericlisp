/*
    File:       lisp_string.c

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#include "lisp_string.h"

#include "lisp_environment.h"
#include "lisp_interior.h"
#include "lisp_memory.h"
#include "lisp_printing.h"
#include "lisp_stream.h"
#include "lisp_utilities.h"

#if LISP_USE_STDLIB
#include <string.h>
#endif


/*
   Characters represent only 28-bit unsigned quantities.

   To get a character from a raw value, shift right by 4
   and mask off everything else.
*/

#if __LP64__
const uintptr_t lisp_char_value_mask = 0x00000000FFFFFFF0;
#else
const uintptr_t lisp_char_value_mask = 0xFFFFFFF0;
#endif

lisp_object_t lisp_char_create(lisp_char_t char_value)
{
    uintptr_t char_unshifted = (uintptr_t)char_value;
    uintptr_t char_shifted = char_unshifted << 4;
    uintptr_t char_masked = char_shifted & lisp_char_value_mask;
    uintptr_t char_mixed = char_masked | ((uintptr_t) lisp_tag_char);
    lisp_object_t object_value = (lisp_object_t) char_mixed;
    return object_value;
}

lisp_char_t lisp_char_get_value(lisp_object_t object)
{
    uintptr_t raw_value = lisp_object_get_raw_value(object);
    uintptr_t char_unshifted = (uintptr_t)raw_value;
    uintptr_t char_masked = (char_unshifted & lisp_char_value_mask);
    uintptr_t char_shifted = char_masked >> 4;
    lisp_char_t char_value = (lisp_char_t)char_shifted;
    return char_value;
}

lisp_object_t lisp_char_print_quoted(lisp_object_t stream, lisp_char_t char_value, lisp_object_t should_quote)
{
    if (should_quote != lisp_NIL) {
        const lisp_object_t octothorpe  = lisp_char_create(char_octothorpe);
        const lisp_object_t backslash   = lisp_char_create(char_backslash);
        lisp_stream_write_char(stream, octothorpe);
        lisp_stream_write_char(stream, backslash);
    }

    lisp_object_t object = lisp_char_create(char_value);
    return lisp_stream_write_char(stream, object);
}

lisp_object_t lisp_char_print(lisp_object_t stream, lisp_char_t char_value)
{
    return lisp_char_print_quoted(stream, char_value, lisp_NIL);
}

lisp_object_t lisp_char_equal(lisp_object_t a, lisp_object_t b)
{
    if (a == b) {
        return lisp_T;
    } else {
        return lisp_NIL;
    }
}


/*
    Strings represent buffers of characters stored in interior pointers.

    They could have been represented as vectors of characters with special
    parsing, however:

    1.  Since characters are not actually heap-allocated we can avoid the
        need to scan the contents of the string too via an interior pointer.

    2.  Strings are truly uniform in storage whereas vectors are just
        typically uniform, so we can potentially switch to storage that does
        not use tagging within the string, and only check/apply tags when
        necessary.
*/

lisp_object_t lisp_string_create(lisp_object_t chars,
                                 uintptr_t capacity,
                                 uintptr_t length)
{
    lisp_string_t string;
    lisp_object_t object = lisp_object_allocate(lisp_tag_string, sizeof(struct lisp_string), (void **)&string);
    string->chars = chars;
    string->capacity = (capacity > 0) ? capacity : length;
    string->length = length;
    return object;
}

lisp_object_t lisp_string_create_c(const char *cstring)
{
    const uintptr_t length = (uintptr_t) strlen(cstring);
    uintptr_t capacity = lisp_round_to_next_multiple(length, 16);
    lisp_object_t *chars_buffer;
    lisp_object_t chars = lisp_interior_create(sizeof(lisp_object_t) * capacity, (void **)&chars_buffer);

    for (uintptr_t i = 0; i < length; i++) {
        char ch = cstring[i];
        lisp_char_t lisp_ch = (lisp_char_t) ch;
        chars_buffer[i] = lisp_char_create(lisp_ch);
    }

    return lisp_string_create(chars, capacity, length);
}

lisp_object_t lisp_string_create_empty(void)
{
    const uintptr_t capacity = 16;
    lisp_object_t chars = lisp_interior_create(sizeof(lisp_object_t) * capacity, NULL);
    return lisp_string_create(chars, capacity, 0);
}

lisp_string_t lisp_string_get_value(lisp_object_t object)
{
    uintptr_t raw_value = lisp_object_get_raw_value(object);
    void *ptr_value = (void *) raw_value;
    return (lisp_string_t)ptr_value;
}

lisp_object_t lisp_string_print(lisp_object_t stream, lisp_string_t string_value)
{
    return lisp_string_print_quoted(stream, string_value, lisp_NIL);
}

lisp_object_t lisp_string_print_quoted(lisp_object_t stream, lisp_string_t string_value, lisp_object_t should_quote)
{
    if (should_quote != lisp_NIL) lisp_char_print_quoted(stream, char_double_quote, lisp_NIL);
    {
        lisp_object_t chars_interior = string_value->chars;
        lisp_object_t *chars = lisp_interior_get_value(chars_interior);
        const uintptr_t length = string_value->length;
        for (uintptr_t i = 0; i < length; i++) {
            lisp_object_t ch_object = chars[i];
            lisp_char_t ch = lisp_char_get_value(ch_object);
            lisp_char_print_quoted(stream, ch, lisp_NIL);
        }
    }
    if (should_quote != lisp_NIL) lisp_char_print_quoted(stream, char_double_quote, lisp_NIL);

    return lisp_T;
}


lisp_object_t lisp_string_equal(lisp_object_t a, lisp_object_t b)
{
    lisp_string_t a_value = lisp_string_get_value(a);
    lisp_string_t b_value = lisp_string_get_value(b);

    /*
     Two strings are equal if they have the same number of characters, and
     the same characters; their capacity doesn't come into play.
     */
    if (a_value->length != b_value->length) {
        return lisp_NIL;
    }

    lisp_object_t *a_chars = (lisp_object_t *)lisp_interior_get_value(a_value->chars);
    lisp_object_t *b_chars = (lisp_object_t *)lisp_interior_get_value(b_value->chars);

    if (memcmp(a_chars, b_chars, a_value->length * sizeof(lisp_object_t)) == 0) {
        return lisp_T;
    } else {
        return lisp_NIL;
    }
}

static int lisp_string_needs_reallocation(lisp_string_t string_value)
{
    return (string_value->length == string_value->capacity);
}

static void lisp_string_reallocate(lisp_string_t string_value)
{
    const uintptr_t old_capacity = string_value->capacity;
    lisp_object_t *old_chars_buffer = (lisp_object_t *)lisp_interior_get_value(string_value->chars);
    const uintptr_t new_capacity = old_capacity + 16;
    lisp_object_t *new_chars_buffer;
    lisp_object_t new_chars = lisp_interior_create(sizeof(lisp_object_t) * new_capacity, (void **)&new_chars_buffer);
    memcpy(new_chars_buffer, old_chars_buffer, sizeof(lisp_object_t) * old_capacity);
    string_value->chars = new_chars;
    string_value->capacity = new_capacity;
}

lisp_object_t lisp_string_append_char(lisp_object_t string, lisp_object_t ch)
{
    lisp_string_t string_value = lisp_string_get_value(string);

    /* Reallocate the string's buffer if necessary. */
    if (lisp_string_needs_reallocation(string_value)) {
        lisp_string_reallocate(string_value);
    }

    lisp_object_t *chars = (lisp_object_t *)lisp_interior_get_value(string_value->chars);
    chars[string_value->length] = ch;
    string_value->length += 1;

    return string;
}

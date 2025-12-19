/*
    File:       lisp_stream.c

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#include "lisp_stream.h"

#include "lisp_environment.h"
#include "lisp_interior.h"
#include "lisp_memory.h"
#include "lisp_string.h"

#if LISP_USE_STDLIB
#include <stdio.h>
#endif


lisp_object_t lisp_stream_create(lisp_object_t functions)
{
    lisp_stream_t underlying;
    lisp_object_t object = lisp_object_allocate(lisp_tag_stream, sizeof(struct lisp_stream), (void **)&underlying);
    underlying->functions = functions;
    underlying->flags = 0;
    return object;
}

lisp_stream_t lisp_stream_get_value(lisp_object_t object)
{
    uintptr_t raw_value = lisp_object_get_raw_value(object);
    void *ptr_value = (void *)raw_value;
    return (lisp_stream_t)ptr_value;
}

lisp_stream_functions_t lisp_stream_get_functions(lisp_object_t object)
{
    lisp_stream_t stream = lisp_stream_get_value(object);
    lisp_interior_t functions_interior = lisp_interior_get_value(stream->functions);
    lisp_stream_functions_t functions = (lisp_stream_functions_t) functions_interior;
    return functions;
}

lisp_object_t lisp_stream_open(lisp_object_t stream, lisp_object_t readable, lisp_object_t writable)
{
    lisp_stream_functions_t functions = lisp_stream_get_functions(stream);
    lisp_object_t result = functions->open(stream, readable, writable);

    if (result != lisp_NIL) {
        lisp_stream_t stream_value = lisp_stream_get_value(stream);
        if (readable == lisp_T) {
            stream_value->flags |= lisp_stream_flags_readable;
        }
        if (writable == lisp_T) {
            stream_value->flags |= lisp_stream_flags_writable;
        }
    }

    return result;
}

lisp_object_t lisp_stream_close(lisp_object_t stream)
{
    lisp_stream_functions_t functions = lisp_stream_get_functions(stream);
    lisp_object_t result = functions->close(stream);

    /* Update flags to indicate file is closed. */
    lisp_stream_t stream_value = lisp_stream_get_value(stream);
    lisp_stream_flags_t flags = stream_value->flags;
    flags = flags & ~lisp_stream_flags_readable;
    flags = flags & ~lisp_stream_flags_writable;
    stream_value->flags = flags;

    return result;
}

lisp_object_t lisp_stream_read_char(lisp_object_t stream)
{
    lisp_stream_functions_t functions = lisp_stream_get_functions(stream);
    return functions->read_char(stream);
}

lisp_object_t lisp_stream_unread_char(lisp_object_t stream, lisp_object_t character)
{
    lisp_stream_functions_t functions = lisp_stream_get_functions(stream);
    return functions->unread_char(stream, character);
}

lisp_object_t lisp_stream_peek_char(lisp_object_t stream)
{
    lisp_object_t character = lisp_stream_read_char(stream);
    lisp_stream_unread_char(stream, character);
    return character;
}

lisp_object_t lisp_stream_write_char(lisp_object_t stream, lisp_object_t value)
{
    lisp_stream_functions_t functions = lisp_stream_get_functions(stream);
    return functions->write_char(stream, value);
}

lisp_object_t lisp_stream_write_string(lisp_object_t stream, lisp_object_t value)
{
    lisp_string_t string_value = lisp_string_get_value(value);
    const uintptr_t length = string_value->length;
    if (length > 0) {
        lisp_interior_t chars_interior = lisp_interior_get_value(string_value->chars);
        lisp_object_t *chars = (lisp_object_t *)chars_interior;
        for (uintptr_t i = 0; i < length; i++) {
            lisp_stream_write_char(stream, chars[i]);
        }
    }
    return stream;
}

lisp_object_t lisp_stream_eofp(lisp_object_t stream)
{
    /* If the stream is already at EOF, just indicate that. */
    lisp_stream_t stream_value = lisp_stream_get_value(stream);
    if ((stream_value->flags & lisp_stream_flags_at_eof) != 0) {
        return lisp_T;
    }

    /* Call the underlying function. */
    lisp_stream_functions_t functions = lisp_stream_get_functions(stream);
    lisp_object_t at_eof = functions->eofp(stream);

    /*
     If we are at EOF, record that so we don't have to call the underlying
     function again.
     */
    if (at_eof == lisp_NIL) {
        return lisp_NIL;
    } else {
        stream_value->flags |= lisp_stream_flags_at_eof;
        return lisp_T;
    }
}

lisp_object_t lisp_stream_openp(lisp_object_t stream)
{
    lisp_stream_t stream_value = lisp_stream_get_value(stream);
    lisp_stream_flags_t flags = stream_value->flags;
    if (   ((flags & lisp_stream_flags_readable) != 0)
        || ((flags & lisp_stream_flags_writable) != 0))
    {
        return lisp_T;
    } else {
        return lisp_NIL;
    }
}


lisp_object_t lisp_stream_print(lisp_object_t stream, lisp_stream_t stream_value)
{
    uintptr_t raw = (uintptr_t) stream_value;
    char buffer[32];
    snprintf(buffer, 31, "#<STREAM 0x%llX>", (unsigned long long)raw);
    lisp_object_t buffer_string = lisp_string_create_c(buffer);
    lisp_string_t buffer_string_value = lisp_string_get_value(buffer_string);
    return lisp_string_print_quoted(stream, buffer_string_value, lisp_NIL);
}


lisp_object_t lisp_stream_equal(lisp_object_t a, lisp_object_t b)
{
    if (a == b) {
        return lisp_T;
    } else {
        return lisp_NIL;
    }
}


lisp_object_t lisp_stream_best_input_stream(lisp_object_t environment,
                                            lisp_object_t stream_designator)
{
    lisp_object_t input_stream;

    if (stream_designator == lisp_T) {
        input_stream = lisp_environment_get_symbol_value(environment, lisp_TERMINAL_IO, lisp_APVAL, lisp_T);
    } else if (stream_designator == lisp_NIL) {
        input_stream = lisp_environment_get_symbol_value(environment, lisp_STANDARD_INPUT, lisp_APVAL, lisp_T);
    } else if (lisp_streamp(stream_designator)) {
        input_stream = stream_designator;
    } else {
        input_stream = lisp_NIL;
    }

    return input_stream;
}


lisp_object_t lisp_stream_best_output_stream(lisp_object_t environment,
                                             lisp_object_t stream_designator)
{
    lisp_object_t output_stream;

    if (stream_designator == lisp_T) {
        output_stream = lisp_environment_get_symbol_value(environment, lisp_TERMINAL_IO, lisp_APVAL, lisp_T);
    } else if (stream_designator == lisp_NIL) {
        output_stream = lisp_environment_get_symbol_value(environment, lisp_STANDARD_OUTPUT, lisp_APVAL, lisp_T);
    } else if (lisp_streamp(stream_designator)) {
        output_stream = stream_designator;
    } else {
        output_stream = lisp_NIL;
    }

    return output_stream;
}


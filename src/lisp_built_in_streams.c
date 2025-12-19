/*
    File:       lisp_built_in_streams.c

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#include "lisp_built_in_streams.h"

#include "lisp_atom.h"
#include "lisp_environment.h"
#include "lisp_interior.h"
#include "lisp_string.h"


#if LISP_USE_STDLIB


/* MARK: C Standard Library Streams */

static FILE *lisp_stream_stdio_get_FILE(lisp_object_t stream)
{
    lisp_stream_functions_t functions = lisp_stream_get_functions(stream);
    lisp_interior_t metadata = lisp_interior_get_value(functions->metadata);
    FILE **metadata_FILE = (FILE **)metadata;
    FILE *file = *metadata_FILE;
    return file;
}

static lisp_object_t lisp_stream_stdio_open(lisp_object_t stream, lisp_object_t readable, lisp_object_t writable)
{
    /* The underlying FILE * must already be open. */
    return stream;
}

static lisp_object_t lisp_stream_stdio_close(lisp_object_t stream)
{
    FILE *file = lisp_stream_stdio_get_FILE(stream);
    fclose(file);
    return stream;
}

static lisp_object_t lisp_stream_stdio_read_char(lisp_object_t stream)
{
    FILE *file = lisp_stream_stdio_get_FILE(stream);
    int ich = fgetc(file);
    if (ich == EOF) {
        return lisp_NIL;
    } else {
        lisp_char_t char_value = (lisp_char_t)ich;
        return lisp_char_create(char_value);
    }
}

static lisp_object_t lisp_stream_stdio_unread_char(lisp_object_t stream, lisp_object_t value)
{
    FILE *file = lisp_stream_stdio_get_FILE(stream);
    lisp_char_t lch = lisp_char_get_value(value);
    char ch = (char)lch;
    int result = ungetc(ch, file);
    if (result == EOF) {
        return lisp_NIL;
    } else {
        lisp_char_t char_value = (lisp_char_t)result;
        return lisp_char_create(char_value);
    }
}

static lisp_object_t lisp_stream_stdio_write_char(lisp_object_t stream, lisp_object_t value)
{
    FILE *file = lisp_stream_stdio_get_FILE(stream);
    lisp_char_t lch = lisp_char_get_value(value);
    char ch = (char)lch;
    fputc(ch, file);
    return stream;
}

static lisp_object_t lisp_stream_stdio_eofp(lisp_object_t stream)
{
    /* Check whether the underlying FILE is at EOF. */
    FILE *file = lisp_stream_stdio_get_FILE(stream);
    int at_eof = feof(file);
    if (at_eof) {
        return lisp_T;
    } else {
        return lisp_NIL;
    }
}

lisp_object_t lisp_stream_functions_stdio(FILE *file)
{
    lisp_stream_functions_t underlying_functions;
    lisp_object_t functions = lisp_interior_create(sizeof(struct lisp_stream_functions), (void **)&underlying_functions);
    underlying_functions->open = lisp_stream_stdio_open;
    underlying_functions->close = lisp_stream_stdio_close;
    underlying_functions->read_char = lisp_stream_stdio_read_char;
    underlying_functions->unread_char = lisp_stream_stdio_unread_char;
    underlying_functions->write_char = lisp_stream_stdio_write_char;
    underlying_functions->eofp = lisp_stream_stdio_eofp;
    FILE **underlying_FILE;
    underlying_functions->metadata = lisp_interior_create(sizeof(FILE *), (void **)&underlying_FILE);
    *underlying_FILE = file;
    return functions;
}


/**
 A C Standard I/O `FILE *` pair, where a different `FILE *` is used for
 input and output, as would be used for *e.g.* `*TERMINAL-IO*`.
 */
typedef struct lisp_stdio_FILE_pair {
    FILE *input;
    FILE *output;
} *lisp_stdio_FILE_pair_t;

static lisp_stdio_FILE_pair_t lisp_stream_stdio_get_FILE_pair(lisp_object_t stream)
{
    lisp_stream_functions_t functions = lisp_stream_get_functions(stream);
    lisp_interior_t metadata = lisp_interior_get_value(functions->metadata);
    lisp_stdio_FILE_pair_t metadata_FILE_pair = (lisp_stdio_FILE_pair_t)metadata;
    return metadata_FILE_pair;
}

static lisp_object_t lisp_stream_stdio_pair_open(lisp_object_t stream, lisp_object_t readable, lisp_object_t writable)
{
    lisp_stdio_FILE_pair_t files = lisp_stream_stdio_get_FILE_pair(stream);
    /* The underlying FILE * must already be open. */
    return stream;
}

static lisp_object_t lisp_stream_stdio_pair_close(lisp_object_t stream)
{
    lisp_stdio_FILE_pair_t files = lisp_stream_stdio_get_FILE_pair(stream);
    fclose(files->input);
    fclose(files->output);
    return stream;
}

static lisp_object_t lisp_stream_stdio_pair_read_char(lisp_object_t stream)
{
    lisp_stdio_FILE_pair_t files = lisp_stream_stdio_get_FILE_pair(stream);
    int ich = fgetc(files->input);
    if (ich == EOF) {
        return lisp_NIL;
    } else {
        lisp_char_t char_value = (lisp_char_t)ich;
        return lisp_char_create(char_value);
    }
}

static lisp_object_t lisp_stream_stdio_pair_unread_char(lisp_object_t stream, lisp_object_t value)
{
    lisp_stdio_FILE_pair_t files = lisp_stream_stdio_get_FILE_pair(stream);
    lisp_char_t lch = lisp_char_get_value(value);
    char ch = (char)lch;
    int result = ungetc(ch, files->input);
    if (result == EOF) {
        return lisp_NIL;
    } else {
        lisp_char_t char_value = (lisp_char_t)result;
        return lisp_char_create(char_value);
    }
}

static lisp_object_t lisp_stream_stdio_pair_write_char(lisp_object_t stream, lisp_object_t value)
{
    lisp_stdio_FILE_pair_t files = lisp_stream_stdio_get_FILE_pair(stream);
    lisp_char_t lch = lisp_char_get_value(value);
    char ch = (char)lch;
    fputc(ch, files->output);
    return stream;
}

static lisp_object_t lisp_stream_stdio_pair_eofp(lisp_object_t stream)
{
    /* Check whether the underlying FILE is at EOF. */
    lisp_stdio_FILE_pair_t files = lisp_stream_stdio_get_FILE_pair(stream);
    int at_input_eof = feof(files->input);
    if (at_input_eof) {
        return lisp_T;
    } else {
        int at_output_eof = feof(files->output);
        if (at_output_eof) {
            return lisp_T;
        } else {
            return lisp_NIL;
        }
    }
}

lisp_object_t lisp_stream_functions_stdio_pair(FILE *input, FILE *output)
{
    lisp_stream_functions_t underlying_functions;
    lisp_object_t functions = lisp_interior_create(sizeof(struct lisp_stream_functions), (void **)&underlying_functions);
    underlying_functions->open = lisp_stream_stdio_pair_open;
    underlying_functions->close = lisp_stream_stdio_pair_close;
    underlying_functions->read_char = lisp_stream_stdio_pair_read_char;
    underlying_functions->unread_char = lisp_stream_stdio_pair_unread_char;
    underlying_functions->write_char = lisp_stream_stdio_pair_write_char;
    underlying_functions->eofp = lisp_stream_stdio_pair_eofp;
    lisp_stdio_FILE_pair_t underlying_FILE_pair;
    underlying_functions->metadata = lisp_interior_create(sizeof(struct lisp_stdio_FILE_pair), (void **)&underlying_FILE_pair);
    underlying_FILE_pair->input = input;
    underlying_FILE_pair->output = output;
    return functions;
}


void lisp_environment_add_built_in_streams(lisp_object_t mutable_environment)
{
    lisp_object_t lisp_TERMINAL_IO_stream = lisp_stream_create(lisp_stream_functions_stdio_pair(stdin, stdout));
    lisp_stream_open(lisp_TERMINAL_IO_stream, lisp_T, lisp_T);
    lisp_object_t lisp_TERMINAL_IO_name = lisp_string_create_c("*TERMINAL-IO*");
    lisp_TERMINAL_IO = lisp_atom_create(lisp_TERMINAL_IO_name);
    lisp_environment_set_symbol_value(mutable_environment, lisp_TERMINAL_IO,
                                      lisp_PNAME, lisp_TERMINAL_IO_name,
                                      lisp_NIL);
    lisp_environment_set_symbol_value(mutable_environment, lisp_TERMINAL_IO,
                                      lisp_APVAL, lisp_TERMINAL_IO_stream,
                                      lisp_NIL);

    lisp_object_t lisp_STANDARD_INPUT_stream = lisp_stream_create(lisp_stream_functions_stdio(stdin));
    lisp_stream_open(lisp_STANDARD_INPUT_stream, lisp_T, lisp_NIL);
    lisp_object_t lisp_STANDARD_INPUT_name = lisp_string_create_c("*STANDARD-INPUT*");
    lisp_STANDARD_INPUT = lisp_atom_create(lisp_STANDARD_INPUT_name);
    lisp_environment_set_symbol_value(mutable_environment, lisp_STANDARD_INPUT,
                                      lisp_PNAME, lisp_STANDARD_INPUT_name,
                                      lisp_NIL);
    lisp_environment_set_symbol_value(mutable_environment, lisp_STANDARD_INPUT,
                                      lisp_APVAL, lisp_STANDARD_INPUT_stream,
                                      lisp_NIL);

    lisp_object_t lisp_STANDARD_OUTPUT_stream = lisp_stream_create(lisp_stream_functions_stdio(stdout));
    lisp_stream_open(lisp_STANDARD_OUTPUT_stream, lisp_NIL, lisp_T);
    lisp_object_t lisp_STANDARD_OUTPUT_name = lisp_string_create_c("*STANDARD-OUTPUT*");
    lisp_STANDARD_OUTPUT = lisp_atom_create(lisp_STANDARD_OUTPUT_name);
    lisp_environment_set_symbol_value(mutable_environment, lisp_STANDARD_OUTPUT,
                                      lisp_PNAME, lisp_STANDARD_OUTPUT_name,
                                      lisp_NIL);
    lisp_environment_set_symbol_value(mutable_environment, lisp_STANDARD_OUTPUT,
                                      lisp_APVAL, lisp_STANDARD_OUTPUT_stream,
                                      lisp_NIL);
}


#else


#error Implement built-in streams without the C Standard Library.


#endif /* LISP_USE_STDLIB */

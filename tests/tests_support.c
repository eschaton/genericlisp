/*
    File:       tests_support.c

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#include "tests_support.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


lisp_object_t tests_root_environment = NULL;

lisp_object_t tests_read_stream = NULL;
char *tests_read_buffer = NULL;
lisp_object_t tests_read_buffer_functions = NULL;

lisp_object_t tests_write_stream = NULL;
char *tests_write_buffer = NULL;
lisp_object_t tests_write_buffer_functions = NULL;


static lisp_object_t tests_charbuf_stream_functions(char *buf, size_t len);


void tests_shared_setup(void)
{
    lisp_heap_initialize(1048576);

    tests_root_environment = lisp_environment_create_root();

    tests_read_buffer = calloc(4096, sizeof(char));
    tests_read_buffer_functions = tests_charbuf_stream_functions(tests_read_buffer, 4096);
    tests_read_stream = lisp_stream_create(tests_read_buffer_functions);

    tests_write_buffer = calloc(4096, sizeof(char));
    tests_write_buffer_functions = tests_charbuf_stream_functions(tests_write_buffer, 4096);
    tests_write_stream = lisp_stream_create(tests_write_buffer_functions);

    lisp_stream_open(tests_read_stream, lisp_T, lisp_NIL);
    lisp_environment_set_symbol_value(tests_root_environment, lisp_STANDARD_INPUT, lisp_APVAL, tests_read_stream, lisp_NIL);
    lisp_stream_open(tests_write_stream, lisp_NIL, lisp_T);
    lisp_environment_set_symbol_value(tests_root_environment, lisp_STANDARD_OUTPUT, lisp_APVAL, tests_write_stream, lisp_NIL);
}


void tests_shared_teardown(void)
{
    lisp_stream_close(tests_read_stream);
    free(tests_read_buffer);

    lisp_stream_close(tests_write_stream);
    free(tests_write_buffer);

    lisp_environment_dispose(tests_root_environment);

    lisp_heap_finalize();
}


/* Character-buffer stream implementation. */

struct tests_charbuf_stream_metadata {
    char *buf;
    size_t r_pos;
    size_t w_pos;
    size_t len;
    size_t max;
    bool is_open;
    bool is_readable;
    bool is_writable;
};


static lisp_object_t tests_charbuf_stream_open(lisp_object_t stream, lisp_object_t readable, lisp_object_t writable)
{
    lisp_stream_functions_t functions = lisp_stream_get_functions(stream);
    lisp_interior_t metadata_object = lisp_interior_get_value(functions->metadata);
    struct tests_charbuf_stream_metadata *metadata = (struct tests_charbuf_stream_metadata *)metadata_object;

    if (metadata->is_open) {
        // TODO: Signal an error
        return lisp_NIL;
    }

    metadata->is_open = true;
    metadata->is_readable = (readable != lisp_NIL) ? true : false;
    metadata->is_writable = (writable != lisp_NIL) ? true : false;

    return stream;
}

static lisp_object_t tests_charbuf_stream_close(lisp_object_t stream)
{
    lisp_stream_functions_t functions = lisp_stream_get_functions(stream);
    lisp_interior_t metadata_object = lisp_interior_get_value(functions->metadata);
    struct tests_charbuf_stream_metadata *metadata = (struct tests_charbuf_stream_metadata *)metadata_object;

    if (metadata->is_open == false) {
        // TODO: Signal an error
        return lisp_NIL;
    }

    metadata->is_open = false;

    return stream;
}

static lisp_object_t tests_charbuf_stream_read_char(lisp_object_t stream)
{
    lisp_stream_functions_t functions = lisp_stream_get_functions(stream);
    lisp_interior_t metadata_object = lisp_interior_get_value(functions->metadata);
    struct tests_charbuf_stream_metadata *metadata = (struct tests_charbuf_stream_metadata *)metadata_object;

    if (metadata->is_open == false) {
        // TODO: Signal an error
        return lisp_NIL;
    }

    char ch;
    if (metadata->is_readable && (metadata->r_pos < metadata->len)) {
        ch = metadata->buf[metadata->r_pos];
        metadata->r_pos += 1;
    } else {
        // TODO: Raise an error
        return lisp_NIL;
    }

    lisp_object_t character = lisp_char_create(ch);

    return character;
}

static lisp_object_t tests_charbuf_stream_unread_char(lisp_object_t stream, lisp_object_t character)
{
    lisp_stream_functions_t functions = lisp_stream_get_functions(stream);
    lisp_interior_t metadata_object = lisp_interior_get_value(functions->metadata);
    struct tests_charbuf_stream_metadata *metadata = (struct tests_charbuf_stream_metadata *)metadata_object;

    if (metadata->is_open == false) {
        // TODO: Signal an error
        return lisp_NIL;
    }

    if (metadata->is_readable && (metadata->r_pos > 0)) {
        lisp_char_t character_value = lisp_char_get_value(character);
        char ch = (char)character_value;

        metadata->r_pos -= 1;
    } else {
        // TODO: Raise an error
        return lisp_NIL;
    }

    return character;
}

static lisp_object_t tests_charbuf_stream_write_char(lisp_object_t stream, lisp_object_t character)
{
    lisp_stream_functions_t functions = lisp_stream_get_functions(stream);
    lisp_interior_t metadata_object = lisp_interior_get_value(functions->metadata);
    struct tests_charbuf_stream_metadata *metadata = (struct tests_charbuf_stream_metadata *)metadata_object;

    if (metadata->is_open == false) {
        // TODO: Signal an error
        return lisp_NIL;
    }

    if (metadata->is_writable && (metadata->w_pos < metadata->max)) {
        lisp_char_t character_value = lisp_char_get_value(character);
        char ch = (char)character_value;

        metadata->buf[metadata->w_pos] = ch;
        metadata->w_pos += 1;
    }

    return stream;
}

static lisp_object_t tests_charbuf_stream_eofp(lisp_object_t stream)
{
    lisp_stream_functions_t functions = lisp_stream_get_functions(stream);
    lisp_interior_t metadata_object = lisp_interior_get_value(functions->metadata);
    struct tests_charbuf_stream_metadata *metadata = (struct tests_charbuf_stream_metadata *)metadata_object;

    if (metadata->is_open == false) {
        // TODO: Signal an error
        return lisp_NIL;
    }

    // EOF only matters for reading; the buffer is at EOF if the read position
    // is the same as the length.

    if (metadata->is_readable && (metadata->r_pos == metadata->len)) {
        return lisp_T;
    }

    return lisp_NIL;
}


static lisp_object_t tests_charbuf_stream_functions(char *buf, size_t len)
{
    lisp_stream_functions_t underlying_functions;
    lisp_object_t functions = lisp_interior_create(sizeof(struct lisp_stream_functions), (void **)&underlying_functions);
    underlying_functions->open = tests_charbuf_stream_open;
    underlying_functions->close = tests_charbuf_stream_close;
    underlying_functions->read_char = tests_charbuf_stream_read_char;
    underlying_functions->unread_char = tests_charbuf_stream_unread_char;
    underlying_functions->write_char = tests_charbuf_stream_write_char;
    underlying_functions->eofp = tests_charbuf_stream_eofp;
    struct tests_charbuf_stream_metadata *metadata;
    underlying_functions->metadata = lisp_interior_create(sizeof(struct tests_charbuf_stream_metadata), (void **)&metadata);
    metadata->buf = buf;
    metadata->r_pos = 0;
    metadata->w_pos = 0;
    metadata->len = 0;
    metadata->max = len - 1; // always leaves room for trailing '\0'
    metadata->is_open = false;
    metadata->is_readable = false;
    metadata->is_writable = false;
    return (lisp_object_t)functions;
}


void tests_set_read_buffer(char *value)
{
    lisp_stream_functions_t functions = lisp_stream_get_functions(tests_read_stream);
    lisp_interior_t metadata_object = lisp_interior_get_value(functions->metadata);
    struct tests_charbuf_stream_metadata *metadata = (struct tests_charbuf_stream_metadata *)metadata_object;

    strncpy(metadata->buf, value, metadata->max);
    metadata->r_pos = 0;
    metadata->w_pos = 0;
    metadata->len = strlen(value);
}


void tests_clear_read_buffer(void)
{
    lisp_stream_functions_t functions = lisp_stream_get_functions(tests_read_stream);
    lisp_interior_t metadata_object = lisp_interior_get_value(functions->metadata);
    struct tests_charbuf_stream_metadata *metadata = (struct tests_charbuf_stream_metadata *)metadata_object;

    metadata->buf[0] = '\0';
    metadata->r_pos = 0;
    metadata->w_pos = 0;
    metadata->len = 0;
}


bool tests_eofp_read_buffer(void)
{
    lisp_stream_functions_t functions = lisp_stream_get_functions(tests_read_stream);
    lisp_interior_t metadata_object = lisp_interior_get_value(functions->metadata);
    struct tests_charbuf_stream_metadata *metadata = (struct tests_charbuf_stream_metadata *)metadata_object;

    return (metadata->r_pos == metadata->len);
}


void tests_clear_write_buffer(void)
{
    lisp_stream_functions_t functions = lisp_stream_get_functions(tests_write_stream);
    lisp_interior_t metadata_object = lisp_interior_get_value(functions->metadata);
    struct tests_charbuf_stream_metadata *metadata = (struct tests_charbuf_stream_metadata *)metadata_object;

    metadata->buf[0] = '\0';
    metadata->r_pos = 0;
    metadata->w_pos = 0;
    metadata->len = 0;
}


int main(int argc, char **argv)
{
    Suite *s = suite_create("genericlisp");
    SRunner *sr = srunner_create(s);

    srunner_add_suite(sr, atom_suite());
    srunner_add_suite(sr, cell_suite());
    srunner_add_suite(sr, char_suite());
    srunner_add_suite(sr, environment_suite());
    srunner_add_suite(sr, evaluation_suite());
    srunner_add_suite(sr, fixnum_suite());
    srunner_add_suite(sr, plist_suite());
    srunner_add_suite(sr, stream_suite());
    srunner_add_suite(sr, string_suite());

    srunner_run_all(sr, CK_VERBOSE);
    int number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

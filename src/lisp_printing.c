/*
    File:       lisp_printing.c

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#include "lisp_printing.h"

#include "lisp_atom.h"
#include "lisp_cell.h"
#include "lisp_environment.h"
#include "lisp_fixnum.h"
#include "lisp_interior.h"
#include "lisp_memory.h"
#include "lisp_stream.h"
#include "lisp_string.h"
#include "lisp_struct.h"
#include "lisp_subr.h"
#include "lisp_vector.h"


lisp_object_t lisp_print(lisp_object_t environment, lisp_object_t stream, lisp_object_t object)
{
    lisp_object_t output_stream = lisp_stream_best_output_stream(environment, stream);

    lisp_tag_t tag = lisp_object_get_tag(object);

    switch (tag) {
        case lisp_tag_fixnum: {
            lisp_fixnum_t fixnum_value = lisp_fixnum_get_value(object);
            return lisp_fixnum_print(output_stream, fixnum_value);
        } break;

        case lisp_tag_atom: {
            const lisp_atom_t atom_value = lisp_atom_get_value(object);
            return lisp_atom_print(output_stream, atom_value);
        } break;

        case lisp_tag_cell: {
            lisp_cell_t cell_value = lisp_cell_get_value(object);
            return lisp_cell_print(environment, output_stream, cell_value);
        } break;

        case lisp_tag_struct: {
            lisp_struct_t struct_value = lisp_struct_get_value(object);
            return lisp_struct_print(environment, output_stream, struct_value);
        } break;

        case lisp_tag_vector: {
            lisp_vector_t vector_value = lisp_vector_get_value(object);
            return lisp_vector_print(environment, output_stream, vector_value);
        } break;

        case lisp_tag_char: {
            lisp_char_t char_value = lisp_char_get_value(object);
            return lisp_char_print(output_stream, char_value);
        } break;

        case lisp_tag_string: {
            lisp_string_t string_value = lisp_string_get_value(object);
            return lisp_string_print(output_stream, string_value);
        } break;

        case lisp_tag_stream: {
            lisp_stream_t stream_value = lisp_stream_get_value(object);
            return lisp_stream_print(output_stream, stream_value);
        } break;

        case lisp_tag_subr: {
            lisp_subr_t subr_value = lisp_subr_get_value(object);
            return lisp_subr_print(output_stream, subr_value);
        } break;

        case lisp_tag_interior: {
            lisp_interior_t interior_value = lisp_interior_get_value(object);
            return lisp_interior_print(output_stream, interior_value);
        } break;
    }
}

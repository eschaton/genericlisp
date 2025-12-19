/*
    File:       lisp_types.c

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#include "lisp_types.h"

#include "lisp_atom.h"
#include "lisp_cell.h"
#include "lisp_environment.h"
#include "lisp_fixnum.h"
#include "lisp_interior.h"
#include "lisp_stream.h"
#include "lisp_string.h"
#include "lisp_struct.h"
#include "lisp_subr.h"
#include "lisp_vector.h"


/* The mask to get a tag. */
#define LISP_TAG_MASK   ((uintptr_t) 0xF)

/* The mask to get a value. */
#define LISP_VALUE_MASK (~LISP_TAG_MASK)


lisp_tag_t lisp_object_get_tag(lisp_object_t object)
{
    uintptr_t rawobj = (uintptr_t) object;
    lisp_tag_t tag = (rawobj & LISP_TAG_MASK);
    return tag;
}


uintptr_t lisp_object_get_raw_value(lisp_object_t object)
{
    uintptr_t rawobj = (uintptr_t) object;
    uintptr_t value = (rawobj & LISP_VALUE_MASK);
    return value;
}


static lisp_object_t lisp_object_has_tag(lisp_object_t object, lisp_tag_t matching_tag)
{
    lisp_tag_t object_tag = lisp_object_get_tag(object);
    return object_tag == matching_tag  ? lisp_T : lisp_NIL;
}


lisp_object_t lisp_cellp(lisp_object_t object)
{
    return lisp_object_has_tag(object, lisp_tag_cell);
}

lisp_object_t lisp_atomp(lisp_object_t object)
{
    return lisp_object_has_tag(object, lisp_tag_atom);
}

lisp_object_t lisp_fixnump(lisp_object_t object)
{
    return lisp_object_has_tag(object, lisp_tag_fixnum);
}

lisp_object_t lisp_structp(lisp_object_t object)
{
    return lisp_object_has_tag(object, lisp_tag_struct);
}

lisp_object_t lisp_vectorp(lisp_object_t object)
{
    return lisp_object_has_tag(object, lisp_tag_vector);
}

lisp_object_t lisp_charp(lisp_object_t object)
{
    return lisp_object_has_tag(object, lisp_tag_char);
}

lisp_object_t lisp_stringp(lisp_object_t object)
{
    return lisp_object_has_tag(object, lisp_tag_string);
}

lisp_object_t lisp_streamp(lisp_object_t object)
{
    return lisp_object_has_tag(object, lisp_tag_stream);
}

lisp_object_t lisp_subrp(lisp_object_t object)
{
    return lisp_object_has_tag(object, lisp_tag_subr);
}

lisp_object_t lisp_interiorp(lisp_object_t object)
{
    return lisp_object_has_tag(object, lisp_tag_interior);
}


lisp_object_t lisp_eq(lisp_object_t a, lisp_object_t b)
{
    if (a == b) {
        return lisp_T;
    } else {
        return lisp_NIL;
    }
}

lisp_object_t lisp_equal(lisp_object_t a, lisp_object_t b)
{
    /* Check whether they're EQ, for quick acceptance. */

    if (lisp_eq(a, b) != lisp_NIL) {
        return lisp_T;
    }

    /* Check whether their types are the same, for quick rejection. */

    lisp_tag_t a_tag = lisp_object_get_tag(a);
    lisp_tag_t b_tag = lisp_object_get_tag(b);
    if (a_tag != b_tag) {
        return lisp_NIL;
    }

    /* Check value equivalence based on type. */

    switch (a_tag) {
        case lisp_tag_cell:
            return lisp_cell_equal(a, b);

        case lisp_tag_atom:
            return lisp_atom_equal(a, b);

        case lisp_tag_char:
            return lisp_char_equal(a, b);

        case lisp_tag_fixnum:
            return lisp_fixnum_equal(a, b);

        case lisp_tag_stream:
            return lisp_stream_equal(a, b);

        case lisp_tag_struct:
            return lisp_struct_equal(a, b);

        case lisp_tag_vector:
            return lisp_vector_equal(a, b);

        case lisp_tag_string:
            return lisp_string_equal(a, b);
            break;

        case lisp_tag_subr:
            return lisp_subr_equal(a, b);
            break;

        case lisp_tag_interior:
            return lisp_interior_equal(a, b);
            break;
    }

    /* If we get all the way through here, they're not equal. */

    return lisp_NIL;
}

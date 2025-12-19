/*
    File:       lisp_interior.c

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#include "lisp_interior.h"

#include "lisp_environment.h"
#include "lisp_memory.h"
#include "lisp_string.h"

#if LISP_USE_STDLIB
#include <stdio.h>
#endif


lisp_object_t lisp_interior_create(uintptr_t size, void **underlying)
{
    return lisp_object_allocate(lisp_tag_interior, size, underlying);
}


lisp_interior_t lisp_interior_get_value(lisp_object_t object)
{
    uintptr_t raw_value = lisp_object_get_raw_value(object);
    return (lisp_interior_t)raw_value;
}


lisp_object_t lisp_interior_print(lisp_object_t stream, lisp_interior_t interior_value)
{
    /*
     Print an interior as #<INTERIOR 0xPOINTER>, our typical syntax for
     anything that cannot be directly read.
     */
    uintptr_t raw = (uintptr_t) interior_value;
    char buffer[32];
    snprintf(buffer, 31, "#<INTERIOR 0x%llX>", (unsigned long long)raw);
    lisp_object_t buffer_string = lisp_string_create_c(buffer);
    lisp_string_t buffer_string_value = lisp_string_get_value(buffer_string);
    return lisp_string_print_quoted(stream, buffer_string_value, lisp_NIL);
}


lisp_object_t lisp_interior_equal(lisp_object_t a, lisp_object_t b)
{
    /* Interior pointers are fully opaque, so only compare identity. */

    if (a == b) {
        return lisp_T;
    } else {
        return lisp_NIL;
    }
}

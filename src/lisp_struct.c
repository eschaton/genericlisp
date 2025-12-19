/*
    File:       lisp_struct.c

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#include "lisp_struct.h"

#include "lisp_environment.h"
#include "lisp_memory.h"
#include "lisp_string.h"

#if LISP_USE_STDLIB
#include <stdio.h>
#endif


lisp_object_t lisp_struct_create(void *value, uintptr_t size, uintptr_t type)
{
    lisp_struct_t struct_value;
    lisp_object_t object = lisp_object_allocate(lisp_tag_struct, sizeof(struct lisp_struct), (void **)&struct_value);
    struct_value->value = value;
    struct_value->size = size;
    struct_value->type = type;
    return object;
}

lisp_struct_t lisp_struct_get_value(lisp_object_t object)
{
    uintptr_t raw_value = lisp_object_get_raw_value(object);
    return (lisp_struct_t)raw_value;
}

lisp_object_t lisp_struct_print(lisp_object_t environment,
                                lisp_object_t stream,
                                lisp_struct_t struct_value)
{
    /*
     Print a structure as #<STRUCT 0xPOINTER>, our typical syntax for
     anything that cannot be directly read.
     */
    uintptr_t raw = (uintptr_t) struct_value;
    char buffer[32];
    snprintf(buffer, 31, "#<STRUCT 0x%llX>", (unsigned long long)raw);
    lisp_object_t buffer_string = lisp_string_create_c(buffer);
    lisp_string_t buffer_string_value = lisp_string_get_value(buffer_string);
    return lisp_string_print_quoted(stream, buffer_string_value, lisp_NIL);
}

lisp_object_t lisp_struct_equal(lisp_object_t a, lisp_object_t b)
{
#warning lisp_struct_equal: Implement.
    return lisp_NIL;
}

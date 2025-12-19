/*
    File:       lisp_atom.c

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#include "lisp_atom.h"

#include "lisp_environment.h"
#include "lisp_interior.h"
#include "lisp_memory.h"
#include "lisp_string.h"

#if LISP_USE_STDLIB
#include <ctype.h>
#include <string.h>
#endif


lisp_object_t lisp_atom_create(lisp_object_t atom_name)
{
    /* Get the underlying value of the passed string. */
    lisp_string_t atom_name_string = lisp_string_get_value(atom_name);

    /* Size of heap buffer including terminator, not length of string. */
    const uintptr_t atom_name_length = atom_name_string->length;
    const uintptr_t atom_name_size = atom_name_length + 1;

    /* Create a buffer that's large enough. */
    lisp_atom_t atom_value;
    lisp_object_t atom = lisp_object_allocate(lisp_tag_atom, sizeof(char) * atom_name_size, (void **)&atom_value);

    /* Copy the name to the heap buffer, uppercasing any lower-case characters. */
    lisp_object_t *char_objects = (lisp_object_t *) lisp_interior_get_value(atom_name_string->chars);
    for (uintptr_t i = 0; i < atom_name_length; i++) {
        lisp_object_t char_object = char_objects[i];
        lisp_char_t char_value = lisp_char_get_value(char_object);
        char ch = (char)char_value;
        if (isalpha(ch) && !isupper(ch)) {
            atom_value[i] = toupper(ch);
        } else {
            atom_value[i] = ch;
        }
    }
    atom_value[atom_name_length] = '\0';

    return atom;
}


lisp_object_t lisp_atom_create_c(const char *atom_name)
{
    /* Size of heap buffer including terminator, not length of string. */
    const uintptr_t atom_name_size = strlen(atom_name) + 1;

    /* Create a buffer that's large enough. */
    lisp_atom_t atom_value;
    lisp_object_t object = lisp_object_allocate(lisp_tag_atom, sizeof(char) * atom_name_size, (void **)&atom_value);

    /* Copy the name to the heap buffer, uppercasing any lower-case characters. */
    for (uintptr_t i = 0; i < atom_name_size; i++) {
        char ch = atom_name[i];
        if (isalpha(ch) && !isupper(ch)) {
            atom_value[i] = toupper(ch);
        } else {
            atom_value[i] = ch;
        }
    }

    return object;
}


lisp_atom_t lisp_atom_get_value(lisp_object_t object)
{
    uintptr_t raw_value = lisp_object_get_raw_value(object);
    return (lisp_atom_t)raw_value;
}


lisp_object_t lisp_atom_print(lisp_object_t stream, const lisp_atom_t atom_value)
{
    lisp_object_t name_value = lisp_string_create_c(atom_value);
    lisp_string_t name_string = lisp_string_get_value(name_value);
    return lisp_string_print_quoted(stream, name_string, lisp_NIL);
}


lisp_object_t lisp_atom_equal(lisp_object_t a, lisp_object_t b)
{
    const char *atom_a = (const char *)lisp_atom_get_value(a);
    const char *atom_b = (const char *)lisp_atom_get_value(b);
    if (strcmp(atom_a, atom_b) == 0) {
        return lisp_T;
    } else {
        return lisp_NIL;
    }
}

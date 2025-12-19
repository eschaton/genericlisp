/*
    File:       lisp_atom.h

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#ifndef __lisp_atom__
#define __lisp_atom__ 1


#include "lisp_types.h"


/**
 A Lisp atom.

 An _atom_ is an opaque structure that compares equal to itself and not
 to anything else. It can be used as a pure token or as a key in an
 _environment_ to find a _property list_ (generally referred to as a
 _plist_) that may have associated values.

 In this implementation, an atom is represented by an uppercase C string
 containing its name, allocated on the Lisp heap.
 */
typedef char *lisp_atom_t;

/** Creates a Lisp atom with the given name. */
LISP_EXTERN lisp_object_t lisp_atom_create(lisp_object_t atom_name);

/** Creates a Lisp atom with the given name as a C string. */
LISP_EXTERN lisp_object_t lisp_atom_create_c(const char *name);

/**  Gets the atom value of the given Lisp object. */
LISP_EXTERN lisp_atom_t lisp_atom_get_value(lisp_object_t object);

/** Prints the atom to the given output stream. */
LISP_EXTERN lisp_object_t lisp_atom_print(lisp_object_t stream, const lisp_atom_t atom_value);

/**
 Compare two atoms for equality.

 This may sound contradictory, but atoms are created ephemerally, and so
 their contents must be compared to determine equality rather than just
 their pointers.

 Another way to say this is that atoms are not uniqued, symbols are; an
 atom is used to represent or reference a symbol, it is not itself the
 symbol. (That better describes the plist associated with the symbol.)
 */
LISP_EXTERN lisp_object_t lisp_atom_equal(lisp_object_t a, lisp_object_t b);

#endif  /* __lisp_atom__ */

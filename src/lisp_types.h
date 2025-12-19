/*
    File:       lisp_types.h

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#ifndef __lisp_types__
#define __lisp_types__ 1


#if LISP_USE_STDLIB
#include <stdint.h>
#else
/* Assume sizeof(long) == sizeof(void *). */
typedef unsigned long uintptr_t;
typedef long intptr_t;
#endif

#include "lisp_base.h"


/**
 A pointer to a Lisp object.  To use one, you MUST get its type and
 choose what to do with it based on that.
 */
typedef struct lisp_object *lisp_object_t;


/**
 The tag of a Lisp object, representing its "top-level" type. These are
 the types about which Lisp can reason, for example for garbage
 collection.
 */
typedef enum lisp_tag {
    /** A cell. */
    lisp_tag_cell       = 0x0,

    /** An atom. */
    lisp_tag_atom       = 0x1,

    /** A signed fixnum of 28 or 60 bits. */
    lisp_tag_fixnum     = 0x2,

    /**
     A struct of arbitrary contents, with layout details exposed to Lisp in
     order to allow participation in garbage collection.
     */
    lisp_tag_struct     = 0x3,

    /**
     A vector of Lisp objects in contiguous memory. The objects need not be
     of the same type, though they will all be represented by a
     `lisp_object_t`.
     */
    lisp_tag_vector     = 0x4,

    /**
     An unsigned 28-bit codepoint in UCS-4 encoding, in system endianness.
     */
    lisp_tag_char       = 0x5,

    /**
     A vector of Lisp characters. */
    lisp_tag_string     = 0x6,

    /**
     A stream, to which raw data can be written and/or from which raw data
     can be read.
     */
    lisp_tag_stream     = 0x7,

    /**
     A compiled Lisp function, or one supplied by the kernel.
     */
    lisp_tag_subr       = 0x8,

    /** Reserved.  Commented out to avoid warnings. */
    /*
    lisp_tag_reserved_9 = 0x9,
    lisp_tag_reserved_A = 0xA,
    lisp_tag_reserved_B = 0xB,
    lisp_tag_reserved_C = 0xC,
    lisp_tag_reserved_D = 0xD,
    lisp_tag_reserved_E = 0xE,
     */

    /**
     A special "interior pointer" owned by another object, that Lisp cannot
     examine without that object's assistance.
     */
    lisp_tag_interior   = 0xF,
} lisp_tag_t;


/** Gets the type portion of the given Lisp object. */
LISP_EXTERN lisp_tag_t lisp_object_get_tag(lisp_object_t object);

/** Gets the "value" portion of the given Lisp object. */
LISP_EXTERN uintptr_t lisp_object_get_raw_value(lisp_object_t object);


/** Tests whether a Lisp object is a cell. */
LISP_EXTERN lisp_object_t lisp_cellp(lisp_object_t object);

/** Tests whether a Lisp object is an atom. */
LISP_EXTERN lisp_object_t lisp_atomp(lisp_object_t object);

/** Tests whether a Lisp object is a fixnum. */
LISP_EXTERN lisp_object_t lisp_fixnump(lisp_object_t object);

/** Tests whether a Lisp object is a struct. */
LISP_EXTERN lisp_object_t lisp_structp(lisp_object_t object);

/** Tests whether a Lisp object is a vector. */
LISP_EXTERN lisp_object_t lisp_vectorp(lisp_object_t object);

/** Tests whether a Lisp object is a char. */
LISP_EXTERN lisp_object_t lisp_charp(lisp_object_t object);

/** Tests whether a Lisp object is a string. */
LISP_EXTERN lisp_object_t lisp_stringp(lisp_object_t object);

/** Tests whether a Lisp object is a stream. */
LISP_EXTERN lisp_object_t lisp_streamp(lisp_object_t object);

/** Tests whether a Lisp object is a compiled or kernel function. */
LISP_EXTERN lisp_object_t lisp_subrp(lisp_object_t object);

/** Tests whether a Lisp object is an interior pointer. */
LISP_EXTERN lisp_object_t lisp_interiorp(lisp_object_t object);


/**
 Tests whether two Lisp objects are _the same_.
 */
LISP_EXTERN lisp_object_t lisp_eq(lisp_object_t a, lisp_object_t b);

/**
 Tests whether two objects are _equivalent_.
 */
LISP_EXTERN lisp_object_t lisp_equal(lisp_object_t a, lisp_object_t b);


#endif /* __lisp_types__ */

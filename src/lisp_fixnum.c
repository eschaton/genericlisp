/*
    File:       lisp_fixnum.c

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#include "lisp_fixnum.h"

#include "lisp_environment.h"
#include "lisp_printing.h"
#include "lisp_string.h"

#include <stdio.h>


/*
   A fixnum represents an integer in the inclusive range

      [ -2^(BITS-(TAG+SIGN)),
         2^(BITS-(TAG+SIGN))-1 ]

   where BITS is 32 or 64, TAG is 4 bits, and SIGN is 1. This means a
   fixnum can represent a 28-bit signed quantity on a 32-bit system and
   a 60-bit signed quantity on a 64-bit system.

   To create a fixnum from a raw value:

   1. Mask off and save the highest bit as the sign.
   2. Shift left by four.
   3. If the sign is set, mix it in.
   4. Mix in the tag (currently 0, so a no-op).

   To get a fixnum from a raw value:

   1. Mask off and save the highest bit as the sign.
   2. Shift right by four.
   3. If the sign is set, mix it *and intermediate bits* in.
 */

#if __LP64__
const uintptr_t lisp_fixnum_highbit_mask =  0x8000000000000000;
const uintptr_t lisp_fixnum_negative_mask = 0xF800000000000000;
const uintptr_t lisp_fixnum_no_mask =       0x0000000000000000;
#else
const uintptr_t lisp_fixnum_highbit_mask =  0x80000000;
const uintptr_t lisp_fixnum_negative_mask = 0xF8000000;
const uintptr_t lisp_fixnum_no_mask =       0x00000000;
#endif


lisp_object_t lisp_fixnum_create(lisp_fixnum_t value)
{
    uintptr_t fixnum_unshifted = value;
    uintptr_t fixnum_highbit = (fixnum_unshifted &  lisp_fixnum_highbit_mask);
    uintptr_t fixnum_shifted = (fixnum_unshifted & ~lisp_fixnum_highbit_mask) << 4;
    uintptr_t fixnum_mixed = ((fixnum_highbit ? lisp_fixnum_highbit_mask : lisp_fixnum_no_mask)
                              | fixnum_shifted
                              | ((uintptr_t) lisp_tag_fixnum));
    return (lisp_object_t) fixnum_mixed;
}


intptr_t lisp_fixnum_get_value(lisp_object_t object)
{
    uintptr_t raw_value = lisp_object_get_raw_value(object);

    uintptr_t fixnum_unshifted = (uintptr_t)raw_value;
    uintptr_t fixnum_highbit = (fixnum_unshifted &  lisp_fixnum_highbit_mask);
    uintptr_t fixnum_shifted = (fixnum_unshifted & ~lisp_fixnum_highbit_mask) >> 4;
    uintptr_t fixnum_mixed = (fixnum_highbit ? lisp_fixnum_negative_mask : lisp_fixnum_no_mask) | fixnum_shifted;
    intptr_t fixnum_value = (intptr_t)fixnum_mixed;
    return fixnum_value;
}


lisp_object_t lisp_fixnum_print(lisp_object_t stream, lisp_fixnum_t fixnum_value)
{
    char buffer[21];
    long long print_value = (long long)fixnum_value;
    snprintf(buffer, 21, "%lld", print_value);
    lisp_object_t string = lisp_string_create_c(buffer);
    lisp_string_t string_value = lisp_string_get_value(string);
    return lisp_string_print_quoted(stream, string_value, lisp_NIL);
}


lisp_object_t lisp_fixnum_equal(lisp_object_t a, lisp_object_t b)
{
    /* Equal fixnum have identical representations. */
    if (a == b) {
        return lisp_T;
    } else {
        return lisp_NIL;
    }
}

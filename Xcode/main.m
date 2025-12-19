/*
    File:       main.m

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#include "genericlisp.h"


LISP_EXTERN int genericlisp_main(int argc, char **argv);


/**
 When building with Xcode, genericlisp is built as a library for testability.
 Thus it needs a separate main() to call the one that's in the library.
 */
int main(int argc, char **argv)
{
    return genericlisp_main(argc, argv);
}

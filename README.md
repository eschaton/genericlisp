# genericlisp

Welcome to `genericlisp`, a straightforward and generic Lisp-2 based on the
Lisp described in the _LISP 1.5 Programmer's Manual_ by McCarthy _et al_,
written for 32/64-bit architectures in as close to pure ANSI C89 as is
reasonably practical.


## Dependencies

The only non-C89 dependency that I know of is on `<stdint.h>`, which should be
straightforward to handle for compilers that do not have it. The only
"problematic" C behavior that should be used is type-punning between integers
and pointer types through the use of `uintptr_t` and `intptr_t`.


## Why?

The overall goal is to be the kind of Lisp that can be used to bootstrap
a better Lisp, rather than to make `genericlisp` that better Lisp
itself.

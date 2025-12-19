# To-Do List

This is stuff remaining to be done for `genericlisp` to get it to 1.0.


## Major Features

- Get `TAGBODY`/`GO` working.
- Implement some form of error handling.
- Get `BLOCK`/`RETURN-FROM`/`RETURN` working.
- Add a macro facility.


## Minor Features

- Support arbitrary number of parameters to appropriate `SUBR`s.
- Support `&rest` in `LAMBDA` lists.
- Rework tests to themselves be C89.


## Bugs

- Interning a symbol should leave its `APVAL` unset.

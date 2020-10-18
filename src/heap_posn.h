// Example of a small API with ownership: heap-allocated `struct posn`s

#pragma once

// Defines `posn_t` to be a pointer to a mutable `struct posn`
// and `const_posn_t` to be a pointer to a const `struct posn`:
typedef        struct posn*        posn_t;
typedef  const struct posn*  const_posn_t;
// Note: C's type system will allow using a `posn_t` where a
// `const_posn_t` is needed, but not the opposite. Why does that make
// sense?


// Declares `ORIGIN` to be a const pointer to a const `struct posn`.
// (It must be defined in the implementation file.)
extern const const_posn_t ORIGIN;
// (Ownership note: Client code can only borrow `ORIGIN`.)


// Creates a new `struct posn` with the given coordinates and returns
// ownership to the caller.
//
// POSTCONDITIONS (return q):
//  - caller owns *q
//  - posn_x(q) == x0
//  - posn_y(q) == y0
//
// ERRORS:
//  - returns NULL if memory can't be allocated.
posn_t posn_create(double x0, double y0);
// (Ownership note: The caller is responsible for deallocating the
// result using `posn_destroy()`.)


// Takes ownership of `*p` (the `struct posn` object pointed to by
// `p`) and deallocates it. (If `p` is NULL, does nothing.)
//
// PRECONDITIONS:
//  - caller owns *p
//
// POSTCONDITIONS:
//  - caller no longer owns *p
//  - *p no longer exists (and p dangles)
void posn_destroy(posn_t p);
// (Ownership note: Thus, the caller must own the object before
// calling this function, and no longer owns it afterward.)


// Returns the x or y coordinate of the argument.
//
// PRECONDITIONS:
//  - p != NULL      (UB otherwise)
double posn_x(const_posn_t);
double posn_y(const_posn_t);
// (Ownership note: Borrows `*p` transiently, meaning only for the
// duration of the call. Since a `const_posn_t` can't be passed to
// `posn_destroy()`, we know that `const_posn_t` are always borrowing
// pointers.)

// Set the x coordinate of `*p` to be `nx`. Borrows `*p` transiently.
//
// PRECONDITIONS:
//  - p != NULL      (UB otherwise)
//
// POSTCONDITIONS:
//  - posn_x(p) == nx
void posn_set_x(posn_t p, double nx);


// Clones `p`; that is, returns ownership of a copy of `*p` to the
// caller. Borrows `*p` transiently
//
// PRECONDITIONS:
//  - p != NULL      (UB otherwise)
//
// POSTCONDITIONS (return q):
//  - caller owns *q
//  - p != q
//  - posn_x(p) == posn_x(q)
//  - posn_y(p) == posn_y(q)
//
// ERRORS:
//  - returns NULL if memory can't be allocated.
posn_t posn_clone(const_posn_t p);
// (Ownership note: The caller need not own the argument, but it
// owns the result.)


// Assigns the components of `*src` (by-value) to the components of
// `*dst`. Both must be non-NULL.
void posn_assign(posn_t dst, const_posn_t src);


/*
 * WEIRD OPERATION for demo purposes
 */

// Returns a pointer borrowing the `double` object that stores the `y`
// coordinate of `*p`.
//
// PRECONDITIONS:
//  - p != NULL      (UB otherwise)
double* posn_borrow_y(posn_t p);
// (Ownership note: The `double*` result is borrowed from `*p`, so
// it must not be freed by the caller. Furthermore, deallocating `*p`
// invalidates the `double*` result.)


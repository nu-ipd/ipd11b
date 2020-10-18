// Example of a heap object that borrows heap objects.

#pragma once

#include "heap_posn.h"

/*
 * A borrowing triangle borrows three `posn_t`s (each of which may be
 * NULL). Because it borrows them, its deallocation function (bt_destroy)
 * does NOT deallocate them.
 */

typedef        struct borrow_tri*    borrow_tri_t;
typedef  const struct borrow_tri*  c_borrow_tri_t;


// Returns a new borrowing triangle whose posns are NULL.
// Returns NULL on allocation error.
borrow_tri_t bt_create(void);

// Deallocates a borrowing triangle. Allows NULL.
void bt_destroy(borrow_tri_t);

// Borrows the vertex `v` (0-2), mutably.
posn_t bt_get_borrowed(borrow_tri_t t, int v);

/*
 * THIS ISN'T POSSIBLE:
 *
// Returns ownership of vertex `v`'s (0-2) posn, leaving the vertex in
// triangle `t` empty (NULL). Allows NULL result but not NULL `t`.
posn_t bt_take_owned(borrow_tri_t t, int v);
 *
 * BUT WE CAN HAVE THIS INSTEAD:
 */

// Returns ownership of a clone of triangle `t`s `v`th vertex.
//
// ERRORS:
//  - If vertex `v` of `t` is NULL, returns a clone of `ORIGIN`.
//  - Returns NULL if memory allocation fails.
posn_t bt_clone_owned(borrow_tri_t t, int v);

/*
 * THIS ISN'T POSSIBLE:
 *
// Takes ownership of `p` and stores it as vertex `v` of triangle `t`.
// If the vertex is already occupied, frees the old one. NULL allowed
// for `p` but not for `t`.
void bt_put_owned(borrow_tri_t t, int v, posn_t p);
 *
 * BUT WE CAN HAVE THIS INSTEAD:
 */

// Sets the coordinates of vertex `v` of triangle `t` to match
// those of position `p`.
//
// Undefined behavior if any of `t`, `p`, or the existing vertex `v`
// of `t` is NULL.
void bt_set_borrowed(borrow_tri_t t, int v, const_posn_t p);

// Sets vertex `v` of triangle `t` to be `p`. NULL okay for `posn_t`s
// but not for `t`.
void bt_put_borrowed(borrow_tri_t t, int v, posn_t p);

// Borrows the vertex `v` (0-2) from a const triangle. Result may be
// NULL, but `t` may not be.
const_posn_t bt_const_get_borrowed(c_borrow_tri_t, int v);

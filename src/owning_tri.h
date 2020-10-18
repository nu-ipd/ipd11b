// Example of a heap object that owns heap objects.

#pragma once

#include "heap_posn.h"

/*
 * An owning triangle owns three `posn_t`s (each of which may be
 * NULL). Because it owns them, its deallocation function (ot_destroy)
 * deallocates the `posn_t`s as well.
 */

typedef        struct owning_tri*    owning_tri_t;
typedef  const struct owning_tri*  c_owning_tri_t;


// Returns a new owning triangle whose posns are all (0, 0).
// NULL on allocation error.
owning_tri_t ot_create(void);

// Deallocates an owning triangle and its owned posns. Allows NULLs.
void ot_destroy(owning_tri_t);

// Borrows the vertex `v` (0-2), mutably.
posn_t ot_get_borrowed(owning_tri_t t, int v);

// Returns ownership of vertex `v`'s (0-2) posn, leaving the vertex in
// triangle `t` empty (NULL). Allows NULL result but not NULL `t`.
posn_t ot_take_owned(owning_tri_t t, int v);

// Takes ownership of `p` and stores it as vertex `v` of triangle `t`.
// If the vertex is already occupied, frees the old one. NULL allowed
// for `p` but not for `t`.
void ot_put_owned(owning_tri_t t, int v, posn_t p);

// Sets vertex `v` of triangle `t` to be a copy of position `p`, which
// it borrows. Returns a pointer to the same vertex `v`, borrowed from
// triangle `t`.
//
// This operation may allocate, and will return NULL to indicate
// allocation failure.
posn_t ot_put_borrowed(owning_tri_t t, int v, const_posn_t p);

// Borrows the vertex `v` (0-2) from a const triangle.
const_posn_t ot_const_get_borrowed(c_owning_tri_t t, int v);

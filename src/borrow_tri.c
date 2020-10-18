#include "borrow_tri.h"

#include <stdlib.h>

#define  N  3

struct borrow_tri
{
    // These `posn_t`s are borrowed. In owning_tri.c they're owned.
    posn_t vertices[N];
};

// Returns a new borrowing triangle whose posns are NULL.
// Returns NULL on allocation error.
borrow_tri_t bt_create(void)
{
    borrow_tri_t result = malloc(sizeof(struct borrow_tri));
    if (!result) return NULL;

    for (int i = 0; i < N; ++i) {
        result->vertices[i] = NULL;
    }

    return result;
}

// Deallocates a borrowing triangle. Allows NULL.
void bt_destroy(borrow_tri_t t)
{
    free(t);
}

// Borrows the vertex `v` (0-2), mutably.
posn_t bt_get_borrowed(borrow_tri_t t, int v)
{
    return t->vertices[v];
}

/*
 * THIS ISN'T POSSIBLE:
 *
// Returns ownership of vertex `v`'s (0-2) posn, leaving the vertex in
// triangle `t` empty (NULL). Allows NULL result but not NULL `t`.
 *
 * We don't own the `posn_t`s in `t`, so we can't give
 * ownership of them to the caller.
 *
posn_t bt_take_owned(borrow_tri_t t, int v);
 */

// Returns ownership of a clone of triangle `t`s `v`th vertex.
//
// ERRORS:
//  - Returns NULL if memory allocation fails.
//  - If vertex `v` of `t` is NULL, returns a clone of `ORIGIN`.
posn_t bt_clone_owned(borrow_tri_t t, int v)
{
    return posn_clone(t->vertices[v] ? t->vertices[v] : ORIGIN);
}

/*
 * THIS ISN'T POSSIBLE:
 *
// Takes ownership of `p` and stores it as vertex `v` of triangle `t`.
// If the vertex is already occupied, frees the old one. NULL allowed
// for `p` but not for `t`.
 *
 * Triangle `t` only stores borrowing pointers, but `p` is an owning
 * pointer. If triangle `t` were to store it, it would lead, since
 * triangle `t` doesn't own (and thus doesn't deallocate) its vertices.
 * The caller would not deallocate `p` because according to the
 * contract, the caller gives up ownership of `p`.
 *
void bt_put_owned(borrow_tri_t t, int v, posn_t p);
 */


// Sets the coordinates of vertex `v` of triangle `t` to match
// those of position `p`.
//
// Undefined behavior if any of `t`, `p`, or the existing vertex `v`
// of `t` is NULL.
void bt_set_borrowed(borrow_tri_t t, int v, const_posn_t src)
{
    posn_assign(t->vertices[v], src);
}

// Sets vertex `v` of triangle `t` to be `p`. NULL okay for `posn_t`s
// but not for `t`.
void bt_put_borrowed(borrow_tri_t t, int v, posn_t p)
{
    t->vertices[v] = p;
}

// Borrows the vertex `v` (0-2) from a const triangle. Result may be
// NULL, but `t` may not be.
const_posn_t bt_const_get_borrowed(c_borrow_tri_t t, int v)
{
    return t->vertices[v];
}

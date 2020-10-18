#include "owning_tri.h"

#include <stdlib.h>

#define  N  3

struct owning_tri
{
    // These `posn_t`s are owned. In borrow_tri.c they're borrowed.
    posn_t vertices[N];
};

// Returns a new triangle whose posns are (0, 0).
// Returns NULL on allocation error.
owning_tri_t ot_create(void)
{
    owning_tri_t result = malloc(sizeof *result);
    if (!result) return NULL;

    for (int i = 0; i < N; ++i)
        result->vertices[i] = posn_clone(ORIGIN);

    return result;
}

// Deallocates a triangle and its owned posns. Allows NULLs.
void ot_destroy(owning_tri_t t)
{
    if (!t) return;

    for (int i = 0; i < N; ++i) {
        posn_destroy(t->vertices[i]);
    }

    free(t);
}

// Borrows the vertex `v` (0-2), mutably.
posn_t ot_get_borrowed(owning_tri_t t, int v)
{
    return t->vertices[v];
}

// Returns ownership of vertex `v`'s (0-2) posn, leaving the vertex in
// triangle `t`. Allows NULL result but not NULL `t`.
posn_t ot_take_owned(owning_tri_t t, int v)
{
    posn_t result = t->vertices[v];
    t->vertices[v] = NULL;
    return result;
}

// Takes ownership of `p` and stores it as vertex `v` of triangle `t`.
// If the vertex is already occupied, frees the old one. NULL allowed
// for `p` but not for `t`.
void ot_put_owned(owning_tri_t t, int v, posn_t p)
{
    posn_destroy(t->vertices[v]);
    t->vertices[v] = p;
}

// Sets vertex `v` of triangle `t` to be a copy of position `p`, which
// it borrows. Returns a pointer to the same vertex `v`, borrowed from
// triangle `t`.
//
// This operation may allocate, and will return NULL to indicate
// allocation failure.
posn_t ot_put_borrowed(owning_tri_t t, int v, const_posn_t p)
{
    // A pointer to the pointer we want to look at and possibly replace:
    posn_t* resultp = &t->vertices[v];

    if (*resultp) {
        posn_assign(*resultp, p);
    } else {
        *resultp = posn_clone(p);
    }

    return *resultp;
}

// Borrows the vertex `v` (0-2) from a const triangle.
const_posn_t ot_const_get_borrowed(c_owning_tri_t t, int v)
{
    return t->vertices[v];
}

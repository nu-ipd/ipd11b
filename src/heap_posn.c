/*
 * See owned_geometry.h first
 */
#include "heap_posn.h"

#include <stdlib.h>

// Clients can't see this definition, so they can only handle
// `struct posn`s via pointers.
struct posn
{
    double x, y;
};

static const struct posn origin_object = {0, 0};
const const_posn_t ORIGIN = &origin_object;

posn_t posn_create(double x, double y)
{
    posn_t result = malloc(sizeof *result);
    if (result) {
        result->x = x;
        result->y = y;
    }
    return result;
}

void posn_destroy(posn_t p)
{
    free(p);
}

double posn_x(const_posn_t p)
{
    return p->x;
}

double posn_y(const_posn_t p)
{
    return p->y;
}

void posn_set_x(posn_t p, double nx)
{
    p->x = nx;
}

posn_t posn_clone(const_posn_t p)
{
    return posn_create(p->x, p->y);
}

void posn_assign(posn_t dst, const_posn_t src)
{
    dst->x = src->x;
    dst->y = src->y;
}

double* posn_borrow_y(posn_t p)
{
    return &p->y;
}

// Example client program for geometry library. Reads a sequence of
// triangles from the specified file or stdin, and writes them to the
// specified file or stdout:
//
//   stdin  to stdout:  % ./geo_client
//   INFILE to stdout:  % ./geo_client INFILE
//   stdin  to OUTFILE: % ./geo_client -      OUTFILE
//   INFILE to OUTFILE: % ./geo_client INFILE OUTFILE
//
// The format for each triangle is:
//
//     "tri: ( %lf , %lf ) ( %lf , %lf ) ( %lf , %lf )"
//
// (Spaces are optional.)
//
// Can use either owning triangles (owning_tri.h) or borrowing triangles
// (borrow_tri.h), as determined by a preprocessor #define. The owning-
// triangle version is built by default; `make geo_client_bt` will
// build the borrowing-triangle version as `geo_client_bt`.

#ifdef BORROWING_TRI
#   include "borrow_tri.h"
#   define  tri_t                   borrow_tri_t
#   define  c_tri_t                 c_borrow_tri_t
#   define  tri_create              bt_create
#   define  tri_destroy             bt_destroy
#   define  tri_get_borrowed        bt_get_borrowed
#   define  tri_const_get_borrowed  bt_const_get_borrowed
#else // BORROWING_TRI
#   include "owning_tri.h"
#   define  tri_t                   owning_tri_t
#   define  c_tri_t                 c_owning_tri_t
#   define  tri_create              ot_create
#   define  tri_destroy             ot_destroy
#   define  tri_get_borrowed        ot_get_borrowed
#   define  tri_const_get_borrowed  ot_const_get_borrowed
#endif // BORROWING_TRI


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//
// Exit codes
//

#define BAD_INFILE     1
#define BAD_OUTFILE    2
#define TOO_MANY_ARGS  3
#define ALLOC_ERROR    4
#define FORMAT_ERROR   5
#define WRITE_ERROR    6


//
// String constants
//

#define READ_POSN_FMT   " ( %lf , %lf ) "
#define WRITE_POSN_FMT  "(%g,%g)"
#define TRIANGLE_HDR    "tri:"


//
// Forward declarations
//

// Reads triangles from `fin` and writes them to `fout`; returns
// the number of triangles copied.
static size_t
copy_triangles(FILE* fin, FILE* fout);

// Attempts to reads a triangle from a stream. Returns positive for
// success, or `EOF` for end-of-file. Bails out on badly formatted
// input.
//
// PRECONDITION [for borrowing-triangle version only]:
//  - The vertices are non-NULL. (UB if violated.)
static int
read_tri(tri_t, FILE*);

// Writes a triangle to a file in the same format as `read_tri()`.
static void
write_tri(c_tri_t, FILE*);

// Reads a posn from a stream.
static posn_t
read_posn(FILE*);

// Writes a posn to a stream in the same format as `read_posn()`.
static void
write_posn(const_posn_t, FILE*);

// Sets `*inp` and `*outp` to where to read from and write to, based on
// the command-line arguments.
static void
process_args(FILE** inp, FILE** outp, int argc, char* argv[]);

// Prints an error message and exits.
static void
bail(int exit_code, const char* optional_msg);

// Prints an error message about bad input and exits.
static void
bail_format(void);


//
// Function definitions
//

int main(int argc, char* argv[])
{
    FILE *fin, *fout;
    process_args(&fin, &fout, argc, argv);

    size_t count = copy_triangles(fin, fout);
    fprintf(stderr, "%zu %s copied\n", count,
            count == 1 ? "triangle" : "triangles");

    if (fin != stdin) fclose(fin);
    if (fout != stdout) fclose(fout);
}


static size_t copy_triangles(FILE* fin, FILE* fout)
{
    // We're going to create one `tri_t` object to read each input
    // triangle into:
    tri_t triangle = tri_create();
    if (!triangle) bail(ALLOC_ERROR, NULL);

#ifdef BORROWING_TRI
    // For the borrowing triangle, we need a place other than the
    // triangle to hold ownership of the posns. We allocate a separate
    // array and fill it with fresh, owned `posn_t`s here; then we store
    // a borrowed pointer to each owned `posn_t` as a vertex of the
    // triangle.
    posn_t posn_owner[3];

    for (int i = 0; i < 3; ++i) {
        posn_owner[i] = posn_clone(ORIGIN);
        if (!posn_owner[i]) bail(ALLOC_ERROR, NULL);

        bt_put_borrowed(triangle, i, posn_owner[i]);
    }
#endif // BORROWING_TRI

    size_t count = 0;

    // The main event!
    while (read_tri(triangle, fin) > 0) {
        write_tri(triangle, fout);
        ++count;
    }

    tri_destroy(triangle);

#ifdef BORROWING_TRI
    // For the borrowing-triangle version, we need to destroy our owned
    // posns (which it borrowed) when we're finished.
    for (int i = 0; i < 3; ++i) {
        posn_destroy(posn_owner[i]);
    }
#endif // BORROWING_TRI

    return count;
}


static int read_tri(tri_t t, FILE* fin)
{
    int res = fscanf(fin, TRIANGLE_HDR);
    if (res != 0) return EOF;

    for (int i = 0; i < 3; ++i) {
        posn_t p = read_posn(fin);
        if (!p) bail_format();

#if BORROWING_TRI
        bt_set_borrowed(t, i, p);
        posn_destroy(p);
#else
        ot_put_owned(t, i, p);
#endif
    }

    return 1;
}


static void write_tri(c_tri_t t, FILE* fout)
{
    int res;

    res = fputs(TRIANGLE_HDR, fout);
    if (res == EOF) bail(WRITE_ERROR, NULL);

    for (int i = 0; i < 3; ++i)
        write_posn(tri_const_get_borrowed(t, i), fout);

    res = fputs("\n", fout);
    if (res == EOF) bail(WRITE_ERROR, NULL);
}


static posn_t read_posn(FILE* stream)
{
    double x, y;

    if (2 != fscanf(stream, READ_POSN_FMT, &x, &y)) {
        return NULL;
    }

    posn_t result = posn_create(x, y);
    if (!result) bail(ALLOC_ERROR, NULL);

    return result;
}


static void write_posn(const_posn_t p, FILE* stream)
{
    int res = fprintf(stream, "(%g,%g)", posn_x(p), posn_y(p));
    if (res < 0) bail(WRITE_ERROR, NULL);
}


static void
process_args(FILE** inp, FILE** outp, int argc, char* argv[])
{
    *inp  = stdin;
    *outp = stdout;

    switch (argc) {
    case 3:
        if ( !(*outp = fopen(argv[2], "w")) )
            bail(BAD_OUTFILE, argv[2]);
        // FALL THROUGH //
    case 2:
        if ( strcmp(argv[1], "-") &&
                 !(*inp = fopen(argv[1], "r")) )
            bail(BAD_INFILE, argv[1]);
        // FALL THROUGH //
    case 1:
        return;

    default:
        fprintf(stderr, "Error: too many arguments\n");
        fprintf(stderr, "Usage: %s [INFILE [OUTFILE]]\n", argv[0]);
        exit(TOO_MANY_ARGS);
    }
}


static void bail_format(void)
{
    fprintf(stderr, "error: bad input format\n");
    exit(FORMAT_ERROR);
}


static void bail(int exit_code, const char* optional_msg)
{
    perror(optional_msg);
    exit(exit_code);
}

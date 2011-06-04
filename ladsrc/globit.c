/* globit.c - globs all of its arguments, and displays the matches */
#include <errno.h>
#include <glob.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* This is the error function we pass to glob(). It just displays
   an error and returns success, which allows the glob() to 
   continue. */
int errfn(const char * pathname, int theerr) {
    fprintf(stderr, "error accessing %s: %s\n", pathname, 
            strerror(theerr));

    /* We want the glob operation to continue, so return 0 */
    return 0;
}

int main(int argc, char ** argv) {
    glob_t result;
    int i, rc, flags;
   
    if (argc < 2) {
        printf("at least one argument must be given\n");
        return 1;
    }

    /* set flags to 0; it gets changed to GLOB_APPEND later */
    flags = 0;

    /* iterate over all of the command-line arguments */
    for (i = 1; i < argc; i++) {
        rc = glob(argv[i], flags, errfn, &result);

        /* GLOB_ABEND can't happen thanks to errfn */
        if (rc == GLOB_NOSPACE) {
            fprintf(stderr, "out of space during glob operation\n");
            return 1;
        }

        flags |= GLOB_APPEND;
    }

    if (!result.gl_pathc) {
        fprintf(stderr, "no matches\n");
        rc = 1;
    } else {
        for (i = 0; i < result.gl_pathc; i++)
            puts(result.gl_pathv[i]);
        rc = 0;
    }

    /* the glob structure uses memory from the malloc() pool, which 
       needs to be freed */
    globfree(&result);

    return rc;
}

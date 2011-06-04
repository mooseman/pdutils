/* mknod.c - equivalent to standard /sbin/mknod command */

/* Create the device or named pipe specified on the command line.
   See the mknod(1) man page for details on the command line
   parameters. */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <unistd.h>

void usage(void) {
     fprintf(stderr, "usage: mknod <path> [b|c|u|p] <major> <minor>\n");
     exit(1);
}

int main(int argc, char ** argv) {
    int major = 0, minor = 0;
    char * path;
    int mode = 0666;
    char *end;
    int args;

    /* We always need at least the type of inode to create, and
       the path for it. */
    if (argc < 3) usage();

    path = argv[1];

    /* the second argument tells us the type of node to create */
    if (!strcmp(argv[2], "b")) {
        mode |= S_IFBLK;
        args = 5;
    } else if (!strcmp(argv[2], "c") || !strcmp(argv[2], "u")) {
        mode |= S_IFCHR;
        args = 5;
    } else if (!strcmp(argv[2], "p")) {
        mode |= S_IFIFO;
        args = 3;
    } else {
        fprintf(stderr, "unknown node type %s\n", argv[2]);
        return 1;
    } 

    /* args tells us how many parameters to expect, as we need more
       information to create device files then named pipes */
    if (argc != args) usage();

    if (args == 5) {
	/* get the major and minor numbers for the device file to
	   create */
        major = strtol(argv[3], &end, 0);
        if (*end) {
            fprintf(stderr, "bad major number %s\n", argv[3]);
            return 1;
        }

        minor = strtol(argv[4], &end, 0);
        if (*end) {
            fprintf(stderr, "bad minor number %s\n", argv[4]);
            return 1;
        }
    }

    /* if we're creating a named pipe, the final parameter is
       ignored */
    if (mknod(path, mode, makedev(major, minor))) {
        fprintf(stderr, "mknod failed: %s\n", strerror(errno));
        return 1;
    }

    return 0;
}

/* checkflags.c - display info on ext2 extended attributes */

/* For each file name passed on the command line, display information
   on that file's ext2 attributes. */

#include <errno.h>
#include <fcntl.h>
#include <linux/ext2_fs.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

int main(int argc, char ** argv) {
    char ** filename = argv + 1;
    int fd;
    int flags;

    /* Iterate over each file name on the command line. The last
       pointer in argv[] is NULL, so this while() loop is legal. */
    while (*filename) {
	/* Unlike normal attributes, ext2 attributes can only
	   be queried if we have a file descriptor (a file name
	   isn't sufficient). We don't need write access to query
	   the ext2 attributes, so O_RDONLY is fine. */
        fd = open(*filename, O_RDONLY);
        if (fd < 0) {
            fprintf(stderr, "cannot open %s: %s\n", *filename,
                    strerror(errno));
            return 1;
        }

	/* This gets the attributes, and puts them into flags */
        if (ioctl(fd, EXT2_IOC_GETFLAGS, &flags)) {
            fprintf(stderr, "ioctl failed on %s: %s\n", *filename,
                    strerror(errno));
            return 1;
        }

        printf("%s:", *filename++);

	/* Check for each attribute, and display a message for each
	   one which is turned on. */
        if (flags & EXT2_APPEND_FL) printf(" Append");
        if (flags & EXT2_IMMUTABLE_FL) printf(" Immutable");
        if (flags & EXT2_SYNC_FL) printf(" Sync");
        if (flags & EXT2_NODUMP_FL) printf(" Nodump");

        printf("\n");
        close(fd);
    } 

    return 0;
};

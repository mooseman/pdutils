/* statsamp.c -- display information returned by lstat() */

/* For each file name passed on the command line, we display all of
   the information lstat() returns on the file. */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define TIME_STRING_BUF 50

/* Make the user pass buf (of minimum length TIME_STRING_BUF) rather
   then using a static buf local to the function to avoid the use of
   static local variables and dynamic memory.  No error should ever occur,
   so we don't do any error checking. */
char * timeString(time_t t, char * buf) {
    struct tm * local;

    local = localtime(&t);
    strftime(buf, TIME_STRING_BUF, "%c", local);

    return buf;
}

/* Display all of the information we get from lstat() on the file
   named as our sole parameter. */
int statFile(char * file) {
    struct stat statbuf; 
    char timeBuf[TIME_STRING_BUF];

    if (lstat(file, &statbuf)) {
        fprintf(stderr, "could not lstat %s: %s\n", file, 
                strerror(errno));
        return 1;
    }

    printf("Filename : %s\n", file);
    printf("On device: major %d/minor %d    Inode number: %ld\n",
           major(statbuf.st_dev), minor(statbuf.st_dev), statbuf.st_ino);
    printf("Size     : %-10ld         Type: %07o       "
           "Permissions: %05o\n", statbuf.st_size, 
           statbuf.st_mode & S_IFMT, statbuf.st_mode & ~(S_IFMT));
    printf("Owner    : %d                Group: %d"
           "          Number of links: %d\n", 
           statbuf.st_uid, statbuf.st_gid, statbuf.st_nlink);
    printf("Creation time: %s\n", timeString(statbuf.st_ctime, timeBuf));
    printf("Modified time: %s\n", timeString(statbuf.st_mtime, timeBuf));
    printf("Access time  : %s\n", timeString(statbuf.st_atime, timeBuf));

    return 0;
}

int main(int argc, char ** argv) {
    int i;
    int rc = 0;

    /* Call statFile() for each file name passed on the command line. */
    for (i = 1; i < argc; i++) {
	/* If statFile() ever fails, rc will end up non-zero. */
        rc |= statFile(argv[i]);

        /* this prints a blank line between entries, but not after
           the last entry */
        if ((argc - i) > 1) printf("\n");
    }

    return rc;
}

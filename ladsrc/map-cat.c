/* map-cat.c - simple cat-like program which uses memory mapping */

#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char ** argv) {
    int fd;
    struct stat sb;
    void * region;

    if ((fd = open(argv[1], O_RDONLY)) < 0) {
        perror("open");
        return 1;
    }
    
    /* stat the file so we know how much of it to map into memory */
    if (fstat(fd, &sb)) {
        perror("fstat");
        return 1;
    }

    /* we could just as well map it MAP_PRIVATE as we aren't writing
       to it anyway */
    region = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (region == ((caddr_t) -1)) {
        perror("mmap");
        return 1;
    }

    close(fd);

    if (write(1, region, sb.st_size) != sb.st_size) {
        perror("write");
        return 1;
    }

    return 0;
}

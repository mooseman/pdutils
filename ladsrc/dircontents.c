/* dircontents.c - display all of the files in the current directory */
  
#include <errno.h>
#include <dirent.h>
#include <stdio.h>

int main(void) {
    DIR * dir;
    struct dirent * ent;

    /* "." is the current directory */
    if (!(dir = opendir("."))) {
        perror("opendir");
        return 1;
    }

    /* set errno to 0, so we can tell when readdir() fails */
    errno = 0;
    while ((ent = readdir(dir))) {
        puts(ent->d_name);
        /* reset errno, as puts() could modify it */
        errno = 0;          
    }

    if (errno) {
        perror("readdir");
        return 1;
    }

    closedir(dir);

    return 0;
}

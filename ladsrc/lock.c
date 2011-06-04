/* lock.c -- simple example of record locking */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

/* displays the message, and waits for the user to press
   return */
void waitforuser(char * message) {
    char buf[10];

    printf("%s", message);
    fflush(stdout);

    fgets(buf, 9, stdin);
}

/* Gets a lock of the indicated type on the fd which is passed. 
   The type should be either F_UNLCK, F_RDLCK, or F_WRLCK */
void getlock(int fd, int type) {
    struct flock lockinfo;
    char message[80];

    /* we'll lock the entire file */
    lockinfo.l_whence = SEEK_SET;
    lockinfo.l_start = 0;
    lockinfo.l_len = 0;

    /* keep trying until we succeed */
    while (1) {
        lockinfo.l_type = type;
	/* if we get the lock, return immediately */
        if (!fcntl(fd, F_SETLK, &lockinfo)) return;

	/* find out who holds the conflicting lock */
        fcntl(fd, F_GETLK, &lockinfo);

	/* there's a chance the lock was freed between the F_SETLK
	   and F_GETLK; make sure there's still a conflict before
	   complaining about it */
        if (lockinfo.l_type != F_UNLCK) {
            sprintf(message, "conflict with process %d... press "
                    "<return> to retry:", lockinfo.l_pid);
            waitforuser(message);
        }
    }
}

int main(void) {
    int fd;

    /* set up a file to lock */
    fd = open("testlockfile", O_RDWR | O_CREAT, 0666);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    printf("getting read lock\n");
    getlock(fd, F_RDLCK);
    printf("got read lock\n");

    waitforuser("\npress <return> to continue:");
    
    printf("releasing lock\n");
    getlock(fd, F_UNLCK);

    printf("getting write lock\n");
    getlock(fd, F_WRLCK);
    printf("got write lock\n");

    waitforuser("\npress <return> to exit:");

    /* locks are released when the file is closed */

    return 0;
}

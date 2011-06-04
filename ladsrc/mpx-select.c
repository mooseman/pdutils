/* mpx-select.c -- reads input from pipes p1, p2 using 
   select() for multiplexing */  

#include <fcntl.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
    int fds[2];
    char buf[4096];
    int i, rc, maxfd;
    fd_set watchset;       /* fds to read from */
    fd_set inset;          /* updated by select() */

    /* open both pipes */
    if ((fds[0] = open("p1", O_RDONLY | O_NONBLOCK)) < 0) {
        perror("open p1");
        return 1;
    }

    if ((fds[1] = open("p2", O_RDONLY | O_NONBLOCK)) < 0) {
        perror("open p2");
        return 1;
    }
 
    /* start off reading from both file descriptors */
    FD_ZERO(&watchset);
    FD_SET(fds[0], &watchset);
    FD_SET(fds[1], &watchset);
 
    /* find the maximum file descriptor */
    maxfd = fds[0] > fds[1] ? fds[0] : fds[1];

    /* while we're watching one of fds[0] or fds[1] */
    while (FD_ISSET(fds[0], &watchset) || 
           FD_ISSET(fds[1], &watchset)) {
	/* we copy watchset here because select() updates it */
        inset = watchset;
        if (select(maxfd + 1, &inset, NULL, NULL, NULL) < 0) {
            perror("select");
            return 1;
        } 

	/* check to see which file descriptors are ready to be
           read from */
        for (i = 0; i < 2; i++) {
            if (FD_ISSET(fds[i], &inset)) {
		/* fds[i] is ready for reading, go ahead... */
                rc = read(fds[i], buf, sizeof(buf) - 1);
                if (rc < 0) {
                    perror("read");
                    return 1;
                } else if (!rc) {
                    /* this pipe has been closed, don't try
		       to read from it again */
                    close(fds[i]);
                    FD_CLR(fds[i], &watchset);
                } else {
                    buf[rc] = '\0';
                    printf("read: %s", buf);
                }
            }
        } 
    }

    return 0;
}

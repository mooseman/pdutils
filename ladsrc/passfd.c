/* passfd.c -- sample program which passes a file descriptor */

/* We behave like a simple /bin/cat, which only handles one 
   argument (a file name). We create Unix domain sockets through
   socketpair(), and then fork(). The child opens the file whose 
   name is passed on the command line, passes the file descriptor 
   and file name back to the parent, and then exits. The parent waits 
   for the file descriptor from the child, then copies data from that 
   file descriptor to stdout until no data is left. The parent then 
   exits. */

#include <alloca.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>

#include "sockutil.h"		/* simple utility functions */

/* The child process. This sends the file descriptor. */
int childProcess(char * filename, int sock) {
    int fd;
    struct iovec vector;        /* some data to pass w/ the fd */
    struct msghdr msg;          /* the complete message */
    struct cmsghdr * cmsg;      /* the control message, which will */
                                /* include the fd */

    /* Open the file whose descriptor will be passed. */
    if ((fd = open(filename, O_RDONLY)) < 0) {
        perror("open");
        return 1;
    }

    /* Send the file name down the socket, including the trailing
       '\0' */
    vector.iov_base = filename;
    vector.iov_len = strlen(filename) + 1;

    /* Put together the first part of the message. Include the
       file name iovec */
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = &vector;
    msg.msg_iovlen = 1;

    /* Now for the control message. We have to allocate room for
       the file descriptor. */
    cmsg = alloca(sizeof(struct cmsghdr) + sizeof(fd));
    cmsg->cmsg_len = sizeof(struct cmsghdr) + sizeof(fd);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
  
    /* copy the file descriptor onto the end of the control 
       message */
    memcpy(CMSG_DATA(cmsg), &fd, sizeof(fd));

    msg.msg_control = cmsg;
    msg.msg_controllen = cmsg->cmsg_len;

    if (sendmsg(sock, &msg, 0) != vector.iov_len)
        die("sendmsg");

    return 0;
}

/* The parent process. This receives the file descriptor. */
int parentProcess(int sock) {
    char buf[80];               /* space to read file name into */
    struct iovec vector;	/* file name from the child */
    struct msghdr msg;		/* full message */
    struct cmsghdr * cmsg;      /* control message with the fd */
    int fd;

    /* set up the iovec for the file name */
    vector.iov_base = buf;
    vector.iov_len = 80;

    /* the message we're expecting to receive */

    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = &vector;
    msg.msg_iovlen = 1;

    /* dynamically allocate so we can leave room for the file
       descriptor */
    cmsg = alloca(sizeof(struct cmsghdr) + sizeof(fd));
    cmsg->cmsg_len = sizeof(struct cmsghdr) + sizeof(fd);
    msg.msg_control = cmsg;
    msg.msg_controllen = cmsg->cmsg_len;

    if (!recvmsg(sock, &msg, 0)) 
        return 1;

    printf("got file descriptor for '%s'\n", 
           (char *) vector.iov_base);

    /* grab the file descriptor from the control structure */
    memcpy(&fd, CMSG_DATA(cmsg), sizeof(fd));
   
    copyData(fd, 1);

    return 0;
}

int main(int argc, char ** argv) {
    int socks[2];
    int status;

    if (argc != 2) {
        fprintf(stderr, "only a single argument is supported\n");
        return 1;
    }

    /* Create the sockets. The first is for the parent and the
       second is for the child (though we could reverse that
       if we liked. */
    if (socketpair(PF_UNIX, SOCK_STREAM, 0, socks)) 
        die("socketpair");

    if (!fork()) {
	/* child */
        close(socks[0]);
        return childProcess(argv[1], socks[1]);
    }

    /* parent */
    close(socks[1]);
    parentProcess(socks[0]);

    /* reap the child */
    wait(&status);

    if (WEXITSTATUS(status))
        fprintf(stderr, "child failed\n");

    return 0;
}

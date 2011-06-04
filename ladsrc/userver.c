/* userver.c - simple server for Unix domain sockets */

/* Waits for a connection on the ./sample-socket Unix domain
   socket. Once a connection has been established, copy data
   from the socket to stdout until the other end closes the
   connection, and then wait for another connection to the socket. */

#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "sockutil.h"          /* some utility functions */

int main(void) {
    struct sockaddr_un address;
    int sock, conn;
    size_t addrLength;

    if ((sock = socket(PF_UNIX, SOCK_STREAM, 0)) < 0)
        die("socket");

    /* Remove any preexisting socket (or other file) */
    unlink("./sample-socket");

    address.sun_family = AF_UNIX;	/* Unix domain socket */
    strcpy(address.sun_path, "./sample-socket");

    /* The total length of the address includes the sun_family 
       element */
    addrLength = sizeof(address.sun_family) + 
                 strlen(address.sun_path);

    if (bind(sock, (struct sockaddr *) &address, addrLength))
        die("bind");

    if (listen(sock, 5))
        die("listen");

    while ((conn = accept(sock, (struct sockaddr *) &address, 
                          &addrLength)) >= 0) {
        printf("---- getting data\n");
        copyData(conn, 1);
        printf("---- done\n");
        close(conn);
    }

    if (conn < 0) 
        die("accept");
    
    close(sock);
    return 0;
}

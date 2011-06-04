/* uclient.c - simple client for Unix domain sockets */

/* Connect to the ./sample-socket Unix domain socket, copy stdin
   into the socket, and then exit. */

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "sockutil.h"          /* some utility functions */

int main(void) {
    struct sockaddr_un address;
    int sock;
    size_t addrLength;

    if ((sock = socket(PF_UNIX, SOCK_STREAM, 0)) < 0)
        die("socket");

    address.sun_family = AF_UNIX;    /* Unix domain socket */
    strcpy(address.sun_path, "./sample-socket");

    /* The total length of the address includes the sun_family 
       element */
    addrLength = sizeof(address.sun_family) + 
                 strlen(address.sun_path);

    if (connect(sock, (struct sockaddr *) &address, addrLength))
        die("connect");

    copyData(0, sock);

    close(sock);
    
    return 0;
}

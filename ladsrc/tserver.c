/* tserver.c - simple server for TCP/IP sockets */

/* Waits for a connection on port 1234. Once a connection has been
   established, copy data from the socket to stdout until the other 
   end closes the connection, and then wait for another connection to 
   the socket. */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "sockutil.h"          /* some utility functions */

int main(void) {
    struct sockaddr_in address;
    int sock, conn, i;
    size_t addrLength = sizeof(struct sockaddr_in);

    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
        die("socket");

    /* Let the kernel reuse the socket address. This lets us run
       twice in a row, without waiting for the (ip, port) tuple
       to time out. */
    i = 1;					
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i)); 

    address.sin_family = AF_INET;
    address.sin_port = htons(1234);
    memset(&address.sin_addr, 0, sizeof(address.sin_addr));

    if (bind(sock, (struct sockaddr *) &address, sizeof(address)))
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

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>

/* display the TCP port number and any aliases for the service 
   which is named on the command line */

/* services.c - finds the port number for a service */
int main(int argc, char ** argv) {
    struct servent * service;
    char ** ptr;

    if (argc != 2) {
        fprintf(stderr, "only a single argument is supported\n");
        return 1;
    }

    /* look up the service in /etc/services, give an error if
       we fail */
    service = getservbyname(argv[1], "tcp");
    if (!service) {
        herror("getservbyname failed");
        return 1;
    }

    printf("service: %s\n", service->s_name);
    printf("tcp port: %d\n", ntohs(service->s_port));

    /* display any aliases this service has */
    if (*service->s_aliases) {
        printf("aliases:");
        for (ptr = service->s_aliases; *ptr; ptr++)
            printf(" %s", *ptr);
        printf("\n");
    }

    return 0;
}

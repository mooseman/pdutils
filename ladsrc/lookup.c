/* lookup.c -- print basic DNS information on an Internet host */

/* Given either a hostname or IP address on the command line, print
   the canonical hostname for that host and all of the IP numbers 
   and hostnames associated with it. */

#include <netdb.h>              /* for gethostby* */
#include <sys/socket.h> 
#include <netinet/in.h>         /* for address structures */
#include <arpa/inet.h>          /* for inet_ntoa() */
#include <stdio.h>

int main(int argc, char ** argv) {
    struct hostent * answer;
    struct in_addr address, ** addrptr;
    char ** next;

    if (argc != 2) {
        fprintf(stderr, "only a single argument is supported\n");
        return 1;
    }

    /* If the argument looks like an IP, assume it was one */
    if (inet_aton(argv[1], &address))
        answer = gethostbyaddr((char *) &address, sizeof(address), 
                               AF_INET);
    else
        answer = gethostbyname(argv[1]);

    /* the hostname lookup failed :-( */
    if (!answer) {
        herror("error looking up host");
        return 1;
    }

    printf("Canonical hostname: %s\n", answer->h_name);

    /* if there are any aliases, print them all out */
    if (answer->h_aliases[0]) {
        printf("Aliases:");
        for (next = answer->h_aliases; *next; next++)
            printf(" %s", *next);
        printf("\n");
    }

    /* display all of the IP addresses for this machine */
    printf("Addresses:");
    for (addrptr = (struct in_addr **) answer->h_addr_list; 
                *addrptr; addrptr++)
        printf(" %s", inet_ntoa(**addrptr));
    printf("\n");

    return 0;
}

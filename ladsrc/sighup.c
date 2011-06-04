/* sighup.c -- sample program which illustrates how to use sighup to
   force a daemon to reopen its log files */

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

volatile int reopenLog = 0;	/* volatile as it is modified by a signal
				   handler */

/* write a line to the log */
void logstring(int logfd, char * str) {
    write(logfd, str, strlen(str));
}

/* When SIGHUP occurs, make a note of it and continue. */
void hupHandler(int signum) {
    reopenLog = 1;
}

int main() {
    int done = 0;
    struct sigaction sa;
    int rc;
    int logfd;

    logfd = STDOUT_FILENO;

    /* Set up a signal handler for SIGHUP. Use memset() to initialize
       the struct sigaction to be sure we clear all of it. */
    memset(&sa, 0, sizeof(sa)); 
    sa.sa_handler = hupHandler;

    if (sigaction(SIGHUP, &sa, NULL)) perror("sigaction");

    /* Log a message every two seconds, and reopen the log file
       as requested by SIGHUP. */
    while (!done) {
        /* sleep() returns nonzero if it didn't sleep long enough */
        rc = sleep(2);
        if (rc) {
            if (reopenLog) {
                logstring(logfd, "* reopening log files at SIGHUP's request\n");
                reopenLog = 0;
            } else {
                logstring(logfd, "* sleep interrupted by unknown signal "
                          "-- dying\n");
                done = 1;
            }
        } else {
            logstring(logfd, "Periodic message\n");
        }
    }

    return 0;
}

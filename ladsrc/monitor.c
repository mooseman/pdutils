/* monitor.c -- watch job control signals */
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void catchSignal(int sigNum, int useDefault);

void handler(int signum) {
    if (signum == SIGTSTP) {
        write(STDOUT_FILENO, "got SIGTSTP\n", 12);
        catchSignal(SIGTSTP, 1);
        kill(getpid(), SIGTSTP);
    } else {
        write(STDOUT_FILENO, "got SIGCONT\n", 12);
        catchSignal(SIGTSTP, 0);
    }
}

void catchSignal(int sigNum, int useDefault) {
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa)); 

    if (useDefault)
        sa.sa_handler = SIG_DFL;
    else
        sa.sa_handler = handler;

    if (sigaction(sigNum, &sa, NULL)) perror("sigaction");
}

int main() {
    catchSignal(SIGTSTP, 0);
    catchSignal(SIGCONT, 0);

    while (1) ;

    return 0;
}

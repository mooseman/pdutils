#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>
#include "ptypair.h"


volatile int propagate_sigwinch = 0;

/* sigwinch_handler
 * propagate window size changes from input file descriptor to
 * master side of pty.
 */
void sigwinch_handler(int signal) { 
   propagate_sigwinch = 1;
}


/* ptytest tries to open a pty pair with a shell running
 * underneath the slave pty. 
 */
int main (void) {
   int master;
   int pid;
   char *name;
   fd_set ready;
   int i;
#define BUFSIZE 1024
   char buf[1024];
   struct termios ot, t;
   struct winsize ws;
   int done = 0;
   struct sigaction act;

   if ((master = get_master_pty(&name)) < 0) {
      perror("ptypair: could not open master pty");
      exit(1);
   }

   /* set up SIGWINCH handler */
   act.sa_handler = sigwinch_handler;
   sigemptyset(&(act.sa_mask));
   act.sa_flags = 0;
   if (sigaction(SIGWINCH, &act, NULL) < 0) {
      perror("ptypair: could not handle SIGWINCH ");
      exit(1);
   }

   if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) < 0) {
      perror("ptypair: could not get window size");
      exit(1);
   }

   if ((pid = fork()) < 0) {
      perror("ptypair");
      exit(1);
   }

   if (pid == 0) { 
      int slave;  /* file descriptor for slave pty */

      /* We are in the child process */
      close(master);

      if ((slave = get_slave_pty(name)) < 0) {
         perror("ptypair: could not open slave pty");
         exit(1);
      }
      free(name);

      /* We need to make this process a session group leader, because
       * it is on a new PTY, and things like job control simply will
       * not work correctly unless there is a session group leader
       * and process group leader (which a session group leader
       * automatically is). This also disassociates us from our old
       * controlling tty. 
       */
      if (setsid() < 0) {
         perror("could not set session leader");
      }

      /* Tie us to our new controlling tty. */
      if (ioctl(slave, TIOCSCTTY, NULL)) {
         perror("could not set new controlling tty");
      }

      /* make slave pty be standard in, out, and error */
      dup2(slave, STDIN_FILENO);
      dup2(slave, STDOUT_FILENO);
      dup2(slave, STDERR_FILENO);

      /* at this point the slave pty should be standard input */
      if (slave > 2) {
         close(slave);
      }


      /* Try to restore window size; failure isn't critical */
      if (ioctl(STDOUT_FILENO, TIOCSWINSZ, &ws) < 0) {
         perror("could not restore window size");
      }

      /* now start the shell */
      execl("/bin/sh", "/bin/sh", 0);

      /* should never be reached */
      exit(1);
   }

   /* parent */
   free(name);

   /* Note that we only set termios settings for standard input;
    * the master side of a pty is NOT a tty.
    */
   tcgetattr(STDIN_FILENO, &ot);
   t = ot;
   t.c_lflag &= ~(ICANON | ISIG | ECHO | ECHOCTL | ECHOE | \
                  ECHOK | ECHOKE | ECHONL | ECHOPRT );
   t.c_iflag |= IGNBRK;
   t.c_cc[VMIN] = 1;
   t.c_cc[VTIME] = 0;
   tcsetattr(STDIN_FILENO, TCSANOW, &t);

   /* This code comes nearly verbatim from robin.c
    * If the child exits, reading master will return -1 and we exit.
    */
   do {
      FD_ZERO(&ready);
      FD_SET(STDIN_FILENO, &ready);
      FD_SET(master, &ready);
      select(master+1, &ready, NULL, NULL, NULL);

      if (propagate_sigwinch) { 
         /* signal handler has asked for SIGWINCH propagation */
         if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) < 0) {
            perror("ptypair: could not get window size");
         }
         if (ioctl(master, TIOCSWINSZ, &ws) < 0) {
            perror("could not restore window size");
         }

         /* now do not do this again until next SIGWINCH */
         propagate_sigwinch = 0;

         /* select may have been interrupted by SIGWINCH,
          * so try again. */
         continue;
      }

      if (FD_ISSET(master, &ready)) {
         i = read(master, buf, BUFSIZE);
         if (i >= 1) {
            write(STDOUT_FILENO, buf, i);
         } else {
            done = 1;
         }
      }

      if (FD_ISSET(STDIN_FILENO, &ready)) {
         i = read(STDIN_FILENO, buf, BUFSIZE);
         if (i >= 1) {
            write(master, buf, i);
         } else {
            done = 1;
         }
      }

   } while (!done);

   /* this really doesn't matter because each time a master pty is
    * opened, the corresponding slave pty has its termios settings
    * reset
    */
   tcsetattr(STDIN_FILENO, TCSANOW, &ot);
   exit(0);
}

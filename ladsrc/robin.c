/* robin.c -- implements simple serial port interaction program */
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <popt.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>             /* for strerror() */
#include <termios.h>
#include <unistd.h>

void usage(int exitcode, char *error, char *addl) {
   fprintf(stderr, "Usage: robin [options] <port>\n"
                   " [options] include:\n"
                   "    -H for this help\n"
                   "    -r for raw mode\n"
                   "    -c to add CR with NL on output\n"
                   "    -h for hardware flow control\n"
                   "    -s for software flow control\n"
                   "    -n for no flow control\n"
                   "    -b <bps> for signalling rate\n");
   if (error) fprintf(stderr, "%s: %s\n", error, addl);
   exit(exitcode);
}

speed_t symbolic_speed(int speednum) { 
   if (speednum >= 460800) return B460800;
   if (speednum >= 230400) return B230400;
   if (speednum >= 115200) return B115200;
   if (speednum >= 57600) return B57600;
   if (speednum >= 38400) return B38400;
   if (speednum >= 19200) return B19200;
   if (speednum >= 9600) return B9600;
   if (speednum >= 4800) return B4800;
   if (speednum >= 2400) return B2400;
   if (speednum >= 1800) return B1800;
   if (speednum >= 1200) return B1200;
   if (speednum >= 600) return B600;
   if (speednum >= 300) return B300;
   if (speednum >= 200) return B200;
   if (speednum >= 150) return B150;
   if (speednum >= 134) return B134;
   if (speednum >= 110) return B110;
   if (speednum >= 75) return B75;
   return B50;
}

/* These need to have file scope so that we can use them in
 * signal handlers */
/* old port termios settings to restore */
static struct termios pots; 
/* old stdout/in termios settings to restore */
static struct termios sots; 
/* port file descriptor */
int    pf;

/* restore original terminal settings on exit */
void cleanup_termios(int signal) {
   tcsetattr(pf, TCSANOW, &pots);
   tcsetattr(STDIN_FILENO, TCSANOW, &sots);
   exit(0);
} 

/* handle a single escape character */
void send_escape(int fd, char c) { 
   switch (c) {
   case 'q':
      /* restore termios settings and exit */
      cleanup_termios(0);
      break;
   case 'b':
      /* send a break */
      tcsendbreak(fd, 0);
      break;
   default:
      /* pass the character through */
      /* "C-\ C-\" sends "C-\" */
      write(fd, &c, 1);
      break;
   }
   return;
}

/* handle escape characters, writing to output */
void cook_buf(int fd, char *buf, int num) { 
   int current = 0;
   static int in_escape = 0;

   if (in_escape) {
      /* cook_buf last called with an incomplete escape sequence */
      send_escape(fd, buf[0]);
      num--;
      buf++;
      in_escape = 0;
   }
   while (current < num) {
#     define CTRLCHAR(c) ((c)-0x40)
      while ((current < num) && (buf[current] != CTRLCHAR('\\'))) current++;
      if (current) write (fd, buf, current);
      if (current < num) {
         /* found an escape character */
         current++;
         if (current >= num) {
            /* interpret first character of next sequence */
            in_escape = 1;
            return;
         }
         send_escape(fd, buf[current]);
      }
      num -= current;
      buf += current;
      current = 0;
   }
   return;
}

int main(int argc, char *argv[]) {
   char    c;            /* used for argument parsing */
   struct  termios pts;  /* termios settings on port */
   struct  termios sts;  /* termios settings on stdout/in */
   char   *portname;
   int    speed = 0;     /* used in argument parsing to set speed */
   struct sigaction sact;/* used to initialize the signal handler */
   fd_set  ready;        /* used for select */
   int     raw = 0;      /* raw mode? */ 
   int     i = 0;        /* used in the multiplex loop */
   int     done = 0;
#  define BUFSIZE 1024
   char    buf[BUFSIZE];
   poptContext optCon;   /* context for parsing command-line options */
   struct poptOption optionsTable[] = {
	    { "bps", 'b', POPT_ARG_INT, &speed, 0 },
	    { "crnl", 'c', 0, 0, 'c' },
	    { "help", 'H', 0, 0, 'H' },
	    { "hwflow", 'h', 0, 0, 'h' },
	    { "noflow", 'n', 0, 0, 'n' },
	    { "raw", 'r', 0, &raw, 0 }, 
	    { "swflow", 's', 0, 0, 's' },
	    { NULL, 0, 0, NULL, 0 }
   };

#ifdef DSLEEP
   /* wait 10 minutes so we can attach a debugger */
   sleep(600);
#endif

   if (argc < 2) usage(1, "Not enough arguments", "");

   /* Normally, we'd let popt figure out which argument is the
      port name. Here we're forcing it to be the last one as it
      eases the rest of the option parsing. */
   portname = argv[argc - 1];
   pf = open(portname, O_RDWR);
   if (pf < 0)
      usage(1, strerror(errno), portname);

   /* modify the port configuration */
   tcgetattr(pf, &pts);
   pots = pts;
   /* some things we want to set arbitrarily */
   pts.c_lflag &= ~ICANON; 
   pts.c_lflag &= ~(ECHO | ECHOCTL | ECHONL);
   pts.c_cflag |= HUPCL;
   pts.c_cc[VMIN] = 1;
   pts.c_cc[VTIME] = 0;

   /* Standard CR/LF handling: this is a dumb terminal.
    * Do no translation:
    *  no NL -> CR/NL mapping on output, and
    *  no CR -> NL mapping on input.
    */
   pts.c_oflag &= ~ONLCR;
   pts.c_iflag &= ~ICRNL;

   /* Now deal with the local terminal side */
   tcgetattr(STDIN_FILENO, &sts);
   sots = sts;
   /* again, some arbitrary things */
   sts.c_iflag &= ~(BRKINT | ICRNL);
   sts.c_iflag |= IGNBRK;
   sts.c_lflag &= ~ISIG;
   sts.c_cc[VMIN] = 1;
   sts.c_cc[VTIME] = 0;
   sts.c_lflag &= ~ICANON;
   /* no local echo: allow the other end to do the echoing */
   sts.c_lflag &= ~(ECHO | ECHOCTL | ECHONL);

   optCon = poptGetContext("robin", argc, argv, optionsTable, 0);

   /* option processing will now modify pts and sts */
   while ((c = poptGetNextOpt(optCon)) >= 0) {
      switch (c) {
         case 'H':
            usage(0, NULL, NULL);
            break;
         case 'c': 
            /* send CR with NL */
            pts.c_oflag |= ONLCR;
            break;
         case 'h': 
            /* hardware flow control */
            pts.c_cflag |= CRTSCTS;
            pts.c_iflag &= ~(IXON | IXOFF | IXANY);
            break;
         case 's':
            /* software flow control */
            pts.c_cflag &= ~CRTSCTS;
            pts.c_iflag |= IXON | IXOFF | IXANY;
            break;
         case 'n':
            /* no flow control */
            pts.c_cflag &= ~CRTSCTS;
            pts.c_iflag &= ~(IXON | IXOFF | IXANY);
            break;
      }
   }

   if (c < -1) {
      /* an error occurred during option processing */
      fprintf(stderr, "%s: %s\n", 
              poptBadOption(optCon, POPT_BADOPTION_NOALIAS),
              poptStrerror(c));
      return 1;
   }
   poptFreeContext(optCon);

   /* speed is not modified unless -b is specified */ 
   if (speed) {
      cfsetospeed(&pts, symbolic_speed(speed));
      cfsetispeed(&pts, symbolic_speed(speed));
   }

   /* set the signal handler to restore the old
    * termios handler */
   sact.sa_handler = cleanup_termios; 
   sigaction(SIGHUP, &sact, NULL);
   sigaction(SIGINT, &sact, NULL);
   sigaction(SIGPIPE, &sact, NULL);
   sigaction(SIGTERM, &sact, NULL);

   /* Now set the modified termios settings */
   tcsetattr(pf, TCSANOW, &pts);
   tcsetattr(STDIN_FILENO, TCSANOW, &sts);

   do {
      FD_ZERO(&ready);
      FD_SET(STDIN_FILENO, &ready);
      FD_SET(pf, &ready);
      select(pf+1, &ready, NULL, NULL, NULL);
      if (FD_ISSET(pf, &ready)) {
         /* pf has characters for us */
         i = read(pf, buf, BUFSIZE);
         if (i >= 1) {
            write(STDOUT_FILENO, buf, i);
         } else {
            done = 1;
         }
      }
      if (FD_ISSET(STDIN_FILENO, &ready)) {
         /* standard input has characters for us */
         i = read(STDIN_FILENO, buf, BUFSIZE);
         if (i >= 1) {
            if (raw) {
               write(pf, buf, i);
            } else {
               cook_buf(pf, buf, i);
            }
         } else {
            done = 1;
         }
      }
   } while (!done);

   /* restore original terminal settings and exit */
   tcsetattr(pf, TCSANOW, &pots);
   tcsetattr(STDIN_FILENO, TCSANOW, &sots);
   exit(0);
}

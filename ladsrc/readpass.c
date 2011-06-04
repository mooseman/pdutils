/* readpass - read a password without echoing it */
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

int main(void) {
   struct termios ts, ots;
   char passbuf[1024];

   /* get and save current termios settings */
   tcgetattr(STDIN_FILENO, &ts);
   ots = ts;

   /* change and set new termios settings */
   ts.c_lflag &= ~ECHO;
   ts.c_lflag |= ECHONL;
   tcsetattr(STDIN_FILENO, TCSAFLUSH, &ts);

   /* paranoia: check that the settings took effect */
   tcgetattr(STDIN_FILENO, &ts);
   if (ts.c_lflag & ECHO) {
      fprintf(stderr, "Failed to turn off echo\n");
      tcsetattr(STDIN_FILENO, TCSANOW, &ots);
      exit(1);
   }

   /* get and print the password */
   printf("enter password: ");
   fflush(stdout);
   fgets(passbuf, 1024, stdin);
   printf("read password: %s", passbuf);
   /* there was a terminal \n in passbuf */

   /* restore old termios settings */
   tcsetattr(STDIN_FILENO, TCSANOW, &ots);

   exit(0);
}

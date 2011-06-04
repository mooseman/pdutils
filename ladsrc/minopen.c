#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/vt.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

int main (int argc, char **argv) {
   int vtnum;
   int vtfd;
   struct vt_stat vtstat;
   char device[32];
   int child;

   vtfd = open("/dev/tty", O_RDWR, 0);
   if (vtfd < 0) {
      perror("minopen: could not open /dev/tty");
      exit (1);
   }
   if (ioctl(vtfd, VT_GETSTATE, &vtstat) < 0) {
      perror("minopen: tty is not virtual console");
      exit (1);
   }
   if (ioctl(vtfd, VT_OPENQRY, &vtnum) < 0) {
      perror("minopen: no free virtual consoles");
      exit (1);
   }
   sprintf(device, "/dev/tty%d", vtnum);
   if (access(device, (W_OK|R_OK)) < 0) {
      perror("minopen: insufficient permission on tty");
      exit (1);
   }
   child = fork();
   if (child == 0) {
      ioctl(vtfd, VT_ACTIVATE, vtnum);
      ioctl(vtfd, VT_WAITACTIVE, vtnum);
      setsid();
      close (0); close (1); close (2);
      close (vtfd);
      vtfd = open(device, O_RDWR, 0); dup(vtfd); dup(vtfd);
      execlp("/bin/bash", "bash", NULL);
   }
   wait (&child);
   ioctl(vtfd, VT_ACTIVATE, vtstat.v_active);
   ioctl(vtfd, VT_WAITACTIVE, vtstat.v_active);
   ioctl(vtfd, VT_DISALLOCATE, vtnum);
   exit(0);
}

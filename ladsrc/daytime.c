#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

int main() {
   struct timeval tv;
   struct timezone tz;
   time_t now;
   /* beginning_of_time is smallest time_t-sized value */
   time_t beginning_of_time = 1L<<(sizeof(time_t)*8 - 1);
   /* end_of_time is largest time_t-sized value */
   time_t end_of_time = ~beginning_of_time;

   printf("time_t is %d bits long\n\n", sizeof(time_t)*8);

   gettimeofday(&tv, &tz);
   now = tv.tv_sec;
   printf("Current time of day represented as a struct timeval:\n"
          "tv.tv_sec = 0x%08x, tv.tv_usec = 0x%08x\n"
          "tz.tz_minuteswest = 0x%08x, tz.tz_dsttime = 0x%08x\n\n",
          tv.tv_sec, tv.tv_usec, tz.tz_minuteswest, tz.tz_dsttime);

   printf("Demonstrating ctime()%s:\n", sizeof(time_t)*8 <= 32 ? "" :
          " (may hang after printing first line; press control-C)");
   printf("time is now %s", ctime(&now));
   printf("time begins %s", ctime(&beginning_of_time));
   printf("time ends %s", ctime(&end_of_time));

   exit (0);
}

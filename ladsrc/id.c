/* id.c -- implements simple version of id command */
#include <grp.h>
#include <pwd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void usage(int die, char *error) {
   fprintf(stderr, "Usage: id [<username>]\n");
   if (error) fprintf(stderr, "%s\n", error);
   if (die) exit(die);
}

void die(char *error) {
   if (error) fprintf(stderr, "%s\n", error);
   exit(3);
}

int main(int argc, char *argv[]) {
   struct passwd *pw;
   struct group   *gp;
   int      current_user = 0;
   uid_t   id;
   int i;

   if (argc > 2) 
         usage(1, NULL);

   if (argc == 1) {
      id = getuid();
      current_user = 1;
      if (!(pw = getpwuid(id)))
         usage(1, "Username does not exist");
   } else {
      if (!(pw = getpwnam(argv[1])))
         usage(1, "Username does not exist");
      id = pw->pw_uid;
   }

   printf("uid=%d(%s)", id, pw->pw_name);
   if (gp = getgrgid(pw->pw_gid))
      printf(" gid=%d(%s)", pw->pw_gid, gp->gr_name);

   if (current_user) {
      gid_t *gid_list;
      int      gid_size;

      if (getuid() != geteuid()) {
         id = geteuid();
         if (!(pw = getpwuid(id)))
            usage(1, "Username does not exist");
         printf(" euid=%d(%s)", id, pw->pw_name);
      }

      if (getgid() != getegid()) {
         id = getegid();
         if (!(gp = getgrgid(id)))
            usage(1, "Group does not exist");
         printf(" egid=%d(%s)", id, gp->gr_name);
      }

      /* use getgroups interface to get current groups */
      gid_size = getgroups(0, NULL);
      if (gid_size) {
         gid_list = malloc(gid_size * sizeof(gid_t));
         getgroups(gid_size, gid_list);

         for (i = 0; i < gid_size; i++) {
            if (!(gp = getgrgid(gid_list[i])))
               die("Group does not exist");
            printf("%s%d(%s)", (i == 0) ? " groups=" : ",",
                               gp->gr_gid, gp->gr_name);
         }

         free(gid_list);
      }
   } else {
      /* get list of groups from group database */
      i = 0;
      while (gp = getgrent()) {
         char *c = *(gp->gr_mem);

         while (c && *c) {
            if (!strncmp(c, pw->pw_name, 16)) {
               printf("%s%d(%s)", (i++ == 0) ? " groups=" : ",",
                                  gp->gr_gid, gp->gr_name);
               c = NULL;
            } else {
               c++;
            }
         }
      }
      endgrent();
   }

   printf("\n");
   exit(0);
}


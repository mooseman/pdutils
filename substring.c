

/*  substring.c  */ 
/*  This code is by dwhitney67 from the */ 
/*  Ubuntu forum.  */  


#include <string.h>
#include <stdlib.h>
#include <stdio.h>


char* substring(const char* string, size_t start, size_t end)
{
   const size_t len = (string ? strlen(string) : 0);

   if (string == NULL || start >= len || start > end)
      return NULL;

   char* rtnstr = NULL;

   if (end >= len)
   {
      rtnstr = malloc(len - start + 1);
      strncpy(rtnstr, string + start, len - start);
   }
   else
   {
      rtnstr = malloc(len - (end - start) + 1);
      strncpy(rtnstr, string + start, (end - start + 1));
   }

   return rtnstr;
}



int main(int argc, char** argv)
{
   if (argc < 3)
   {
      fprintf(stderr, "Usage: %s <string> <start> [end]\n", argv[0]);
      return -1;
   }

   const char*  str   = argv[1];
   const size_t start = atoi(argv[2]);
   const size_t end   = (argc == 4 ? atoi(argv[3]) : strlen(str));

   char* substr = substring(str, start, end);

   fprintf(stdout, "substring: %s\n", (substr ? substr : "bad input!"));

   free(substr);

   return 0;
}






/*  ungets.c  */  
/*  Pushes a NUL-terminated string back into an */ 
/*  input stream; returns the number of characters ungotten */ 
/*  This code is public domain - it is from here - */ 
/*  http://foobook.org/jwodder/scraps/ungets/   */  


#include <stdio.h>
#include <string.h>

int ungets(const char* str, FILE* stream) {
 if (!str) return 0;
 size_t len = strlen(str);
 for (int i=len-1; i>=0; i--)
  if (ungetc(str[i], stream) == EOF) return len-i-1;
 return len;
}


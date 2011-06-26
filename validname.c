

/* validname.c                                                  */ 
/* Create a validname() function. This tests to see if a        */ 
/* name starts with a letter or underscore, and is then made up */ 
/* of only letters, digits or underscores.                      */                         
/* This code is released to the public domain.                  */ 
/* "Share and enjoy..."  ;)                                     */ 


#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <ctype.h>


/* A helper function to test for an underscore.  */ 
int isuscoreoralpha(char ch)  
{ 
   return ( (ch == '_') || isalpha(ch) ) ; 
} 

int isuscoreoralnum(char ch) 
{ 
   return ( (ch == '_') || isalnum(ch) ) ;    
} 



/* See if a string is a valid name.  */ 
void validname(const char* str)
{ 
  int i;   
  int err=0;   
    
  for (i=0; i<strlen(str); i++) 
  {     
     if (!isuscoreoralpha(str[0]) ) 
        { puts("Invalid first character. \n");   
          err += 1; 
          break; } 
     else if (!isuscoreoralnum(str[i]) ) 
        { puts("Non-letter-or-underscore found. \n");  
          err += 1;       
          break; }                           
  }     
   if (err == 0) puts("This is a valid name. \n");    
}




int main() { 

char *str1 = "_1test" ; 
char *str2 = "foo_42" ; 
char *str3 = "5bar" ; 
char *str4 = "abf$" ; 

validname(str1); 
validname(str2); 
validname(str3); 
validname(str4); 
       
return 0;   
  
}







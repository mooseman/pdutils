

/* mystrlen.c  */  

/* Find the length of a string without using strlen(). */ 
/* This code is released to the public domain. */ 
/* "Share and enjoy...."  ;)    */  


#include <stdio.h>
#include <string.h>
#include <ctype.h> 


int mystrlen(char *str) 
{   
  int len=0;         
      
  /* Pointer to character before *str. */ 
  /* Used to find first character of string. */  
  char *next; 
  next = str + 1;       
          
  while ( *str != '\0' ) 
  {                      
     if ( *str != ' ' )               
     {                  
        str++; 
        len++;         
        if ( ( *next == ' ' ) || ( *next == '\0' ) ) {break;}  
     } 
     else if ( *str == ' ' ) 
     {                  
        str++;  
        next++;        
     }                  
  }  
  
  return len; 
                      
} 


int main() 
{ 
 
 char *test = "   select    " ; 
 char *test2 = "     * " ; 
           
 printf("%d \n", mystrlen(test) ) ;           
 printf("%d \n", mystrlen(test2) ) ;           
                                      
 return 0;
 
 
}  



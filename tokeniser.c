

/* tokeniser.c */
 
/*
A simple tokeniser which uses strtok() to split the 
text entered by the user. It breaks up the text into tokens and
prints them out. 
This code is released to the public domain.
"Share and enjoy....."  ;)   
*/
 
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
  
    
int main(void)
{
  
  char text[80];
  char *tokens;
  /*  Maximum length of line  */ 
  int maxlen = 80; 
  
    puts("Enter some text :");
    fgets(text, maxlen, stdin);     
    tokens = strtok(text, " ");
    
    while (tokens != NULL) {

    printf("Token = %s\n", tokens);
    
    tokens = strtok(NULL, " ");    
            
    }      
      
  return 0;
 
}
 
 
 
 
